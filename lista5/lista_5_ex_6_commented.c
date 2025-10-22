// É necessário definir _GNU_SOURCE antes de qualquer include para ter acesso
// às funções sched_setaffinity, sched_getaffinity e à estrutura cpu_set_t.
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <string.h>
#include <errno.h>
#include <time.h>

// Estrutura para passar os argumentos para cada thread
typedef struct {
    int thread_id;      // Identificador da nossa thread (0 a 3)
    int *vector;        // Ponteiro para o vetor que a thread deve ordenar
    int vector_size;    // Tamanho do vetor
    int num_cores;      // Número total de cores disponíveis no sistema
} thread_args_t;

// Função de comparação para o qsort
int compare_integers(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

// A função que cada thread irá executar
void *sort_thread_function(void *args) {
    thread_args_t *thread_data = (thread_args_t *)args;

    // 1. DEFINIR A AFINIDADE DA THREAD
    // ---------------------------------
    
    // Calcula em qual core esta thread deve ser executada.
    // Usamos o operador módulo para distribuir as threads entre os cores.
    // Ex: Thread 0 no Core 0, Thread 1 no Core 1, ..., Thread 4 no Core 0 novamente.
    int core_id = thread_data->thread_id % thread_data->num_cores;

    // cpu_set_t é um tipo de dado que representa um conjunto de CPUs.
    cpu_set_t cpuset;
    
    // CPU_ZERO inicializa o conjunto para que ele não contenha nenhum CPU.
    CPU_ZERO(&cpuset);
    
    // CPU_SET adiciona o CPU 'core_id' ao conjunto.
    CPU_SET(core_id, &cpuset);

    // sched_setaffinity define a afinidade de CPU da thread.
    // O primeiro argumento '0' significa "a thread atual".
    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) == -1) {
        fprintf(stderr, "Erro ao definir a afinidade da thread %d para o core %d: %s\n",
                thread_data->thread_id, core_id, strerror(errno));
        pthread_exit(NULL);
    }

    printf("Thread %d foi definida para executar no Core %d.\n", thread_data->thread_id, core_id);

    // 2. VERIFICAR A AFINIDADE
    // -------------------------

    cpu_set_t affinity_mask;
    CPU_ZERO(&affinity_mask);

    // sched_getaffinity obtém a afinidade atual da thread.
    if (sched_getaffinity(0, sizeof(cpu_set_t), &affinity_mask) == -1) {
        fprintf(stderr, "Erro ao verificar a afinidade da thread %d: %s\n",
                thread_data->thread_id, strerror(errno));
    } else {
        // CPU_ISSET verifica se um CPU está no conjunto.
        if (CPU_ISSET(core_id, &affinity_mask)) {
            printf("VERIFICAÇÃO: Thread %d confirmada no Core %d.\n", thread_data->thread_id, core_id);
        } else {
            printf("VERIFICAÇÃO FALHOU: Thread %d não está no Core %d.\n", thread_data->thread_id, core_id);
        }
    }
    
    // 3. EXECUTAR A TAREFA (ORDENAÇÃO)
    // --------------------------------
    
    printf("Thread %d iniciando a ordenação do vetor de %d elementos... (observe no 'top')\n",
           thread_data->thread_id, thread_data->vector_size);

    // Usamos a função qsort da biblioteca padrão para ordenar o vetor.
    qsort(thread_data->vector, thread_data->vector_size, sizeof(int), compare_integers);

    printf("Thread %d terminou a ordenação.\n", thread_data->thread_id);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <tamanho_do_vetor>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "O tamanho do vetor deve ser um número positivo.\n");
        return 1;
    }

    const int NUM_THREADS = 4;
    int *vectors[NUM_THREADS];
    pthread_t threads[NUM_THREADS];
    thread_args_t thread_args[NUM_THREADS];

    // Obtém o número de processadores online (cores disponíveis)
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    if (num_cores < 1) {
        fprintf(stderr, "Não foi possível determinar o número de cores.\n");
        return 1;
    }
    printf("Sistema detectado com %d cores.\n\n", num_cores);

    // Inicializa a semente para geração de números aleatórios
    srand(time(NULL));

    // Aloca e preenche os 4 vetores com números aleatórios
    for (int i = 0; i < NUM_THREADS; i++) {
        vectors[i] = (int *)malloc(n * sizeof(int));
        if (vectors[i] == NULL) {
            perror("Falha ao alocar memória para o vetor");
            return 1;
        }
        for (int j = 0; j < n; j++) {
            vectors[i][j] = rand() % 10000; // Números entre 0 e 9999
        }
    }

    // Cria as 4 threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i].thread_id = i;
        thread_args[i].vector = vectors[i];
        thread_args[i].vector_size = n;
        thread_args[i].num_cores = num_cores;

        if (pthread_create(&threads[i], NULL, sort_thread_function, &thread_args[i]) != 0) {
            perror("Falha ao criar a thread");
            return 1;
        }
    }

    // Espera todas as threads terminarem
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Libera a memória alocada para os vetores
    for (int i = 0; i < NUM_THREADS; i++) {
        free(vectors[i]);
    }
    
    printf("\nTodos os vetores foram ordenados. Programa finalizado.\n");

    return 0;
}