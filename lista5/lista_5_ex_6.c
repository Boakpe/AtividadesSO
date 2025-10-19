#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <string.h>

typedef struct {
    int idx;           // índice da thread (0..3)
    int cpu_id;        // core escolhido
    int *data;         // vetor
    size_t n;          // tamanho do vetor
    int hold_seconds;  // seg. para manter a thread viva após ordenar (para inspecionar no top)
} thread_arg_t;

static inline pid_t gettid_linux(void) {
    return (pid_t)syscall(SYS_gettid);
}

static int cmp_int(const void *a, const void *b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

static int is_sorted(const int *v, size_t n) {
    for (size_t i = 1; i < n; i++) {
        if (v[i-1] > v[i]) return 0;
    }
    return 1;
}

static void print_cpuset(const cpu_set_t *set, int ncpus) {
    printf("{");
    int first = 1;
    for (int i = 0; i < ncpus; i++) {
        if (CPU_ISSET(i, set)) {
            if (!first) printf(",");
            printf("%d", i);
            first = 0;
        }
    }
    printf("}");
}

static void fill_random(int *v, size_t n, unsigned int seed) {
    // rand_r é thread-safe
    for (size_t i = 0; i < n; i++) {
        v[i] = (int)rand_r(&seed);
    }
}

void* sort_thread(void *arg) {
    thread_arg_t *a = (thread_arg_t*)arg;

    // Nome da thread (ajuda no top/htop) — opcional
    #ifdef __linux__
    char th_name[16];
    snprintf(th_name, sizeof(th_name), "sort_%d", a->idx);
    pthread_setname_np(pthread_self(), th_name);
    #endif

    pid_t tid = gettid_linux();

    // Fixa afinidade no core a->cpu_id
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(a->cpu_id, &mask);
    if (sched_setaffinity(0, sizeof(mask), &mask) != 0) {
        fprintf(stderr, "[T%d/TID %d] ERRO sched_setaffinity: %s\n",
                a->idx, tid, strerror(errno));
        pthread_exit((void*)1);
    }

    // Verifica afinidade
    cpu_set_t check;
    CPU_ZERO(&check);
    if (sched_getaffinity(0, sizeof(check), &check) != 0) {
        fprintf(stderr, "[T%d/TID %d] ERRO sched_getaffinity: %s\n",
                a->idx, tid, strerror(errno));
        pthread_exit((void*)1);
    }

    int ncpus = sysconf(_SC_NPROCESSORS_CONF);
    printf("[T%d/TID %d] Core atribuído: %d | Mask atual: ", a->idx, tid, a->cpu_id);
    print_cpuset(&check, ncpus);
    printf(" | sched_getcpu() agora: %d\n", sched_getcpu());
    fflush(stdout);

    // Ordena
    qsort(a->data, a->n, sizeof(int), cmp_int);

    // Checa ordenação
    if (!is_sorted(a->data, a->n)) {
        fprintf(stderr, "[T%d/TID %d] ERRO: vetor não está ordenado!\n", a->idx, tid);
    } else {
        printf("[T%d/TID %d] Ordenação concluída. Ainda no core: %d\n",
               a->idx, tid, sched_getcpu());
    }
    fflush(stdout);

    // Mantém a thread viva por alguns segundos (opcional) para inspeção no top
    for (int s = 0; s < a->hold_seconds; s++) {
        printf("[T%d/TID %d] hold %d/%d s | core atual: %d\n",
               a->idx, tid, s+1, a->hold_seconds, sched_getcpu());
        fflush(stdout);
        sleep(1);
    }

    pthread_exit(0);
}

int main(int argc, char **argv) {
    setvbuf(stdout, NULL, _IONBF, 0); // sem buffer para logs aparecerem na hora

    // Parâmetros
    size_t n = 1000000; // padrão: 1 milhão por vetor
    int hold_seconds = 0; // padrão: não segura as threads após ordenar

    if (argc >= 2) {
        n = strtoull(argv[1], NULL, 10);
        if (n == 0) n = 1000000;
    }
    if (argc >= 3) {
        hold_seconds = atoi(argv[2]);
        if (hold_seconds < 0) hold_seconds = 0;
    }

    int ncpus_onln = sysconf(_SC_NPROCESSORS_ONLN);
    int ncpus_conf = sysconf(_SC_NPROCESSORS_CONF);
    if (ncpus_onln <= 0) {
        fprintf(stderr, "Não foi possível obter número de CPUs.\n");
        return 1;
    }
    if (ncpus_conf > CPU_SETSIZE) {
        fprintf(stderr, "Aviso: sistema com %d CPUs > CPU_SETSIZE(%d). Exemplo não cobre CPUs > %d.\n",
                ncpus_conf, CPU_SETSIZE, CPU_SETSIZE);
        ncpus_conf = CPU_SETSIZE;
    }

    printf("PID do processo: %d | CPUs online: %d (configuradas: %d)\n",
           getpid(), ncpus_onln, ncpus_conf);

    // Aloca e gera 4 vetores
    const int T = 4;
    int *vec[T] = {0};
    for (int i = 0; i < T; i++) {
        vec[i] = (int*)malloc(n * sizeof(int));
        if (!vec[i]) {
            fprintf(stderr, "Falha ao alocar vetor %d (n=%zu)\n", i, n);
            return 1;
        }
        // Usando seeds diferentes
        unsigned int seed = (unsigned int)(time(NULL) ^ (i * 0x9e3779b9U) ^ (getpid() << 16));
        fill_random(vec[i], n, seed);
    }

    // Cria threads e distribui cores: i % ncpus_onln
    pthread_t th[T];
    thread_arg_t args[T];

    for (int i = 0; i < T; i++) {
        args[i].idx = i;
        args[i].cpu_id = i % ncpus_onln; // divide entre os cores existentes
        args[i].data = vec[i];
        args[i].n = n;
        args[i].hold_seconds = hold_seconds;

        int rc = pthread_create(&th[i], NULL, sort_thread, &args[i]);
        if (rc != 0) {
            fprintf(stderr, "Erro ao criar thread %d: %s\n", i, strerror(rc));
            return 1;
        }
    }

    // Aguarda fim
    for (int i = 0; i < T; i++) {
        void *ret = NULL;
        pthread_join(th[i], &ret);
        if (ret != 0) {
            fprintf(stderr, "Thread %d finalizou com erro.\n", i);
        }
    }

    // Libera memória
    for (int i = 0; i < T; i++) free(vec[i]);

    printf("Todas as threads finalizaram.\n");
    return 0;
}