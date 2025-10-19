#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#define MAX_FILOSOFOS 20
#define TEMPO_PENSAR 2
#define TEMPO_COMER 2
#define NUM_REFEICOES 5

// Nomes dos semáforos
#define SEM_GARFO_PREFIX "/garfo_"
#define SEM_MUTEX "/mutex_mesa"

// Função para pensar
void pensar(int id) {
    printf("Filósofo %d está pensando...\n", id);
    sleep(rand() % TEMPO_PENSAR + 1);
}

// Função para comer
void comer(int id) {
    printf("Filósofo %d está comendo...\n", id);
    sleep(rand() % TEMPO_COMER + 1);
    printf("Filósofo %d terminou de comer.\n", id);
}

// Função que representa o comportamento de um filósofo
void filosofo(int id, int num_filosofos) {
    char sem_garfo_esq[50], sem_garfo_dir[50];
    sem_t *garfo_esq, *garfo_dir, *mutex;
    
    // Determina os garfos esquerdo e direito
    int garfo_esq_id = id;
    int garfo_dir_id = (id + 1) % num_filosofos;
    
    // Cria os nomes dos semáforos dos garfos
    sprintf(sem_garfo_esq, "%s%d", SEM_GARFO_PREFIX, garfo_esq_id);
    sprintf(sem_garfo_dir, "%s%d", SEM_GARFO_PREFIX, garfo_dir_id);
    
    // Abre os semáforos
    garfo_esq = sem_open(sem_garfo_esq, 0);
    garfo_dir = sem_open(sem_garfo_dir, 0);
    mutex = sem_open(SEM_MUTEX, 0);
    
    if (garfo_esq == SEM_FAILED || garfo_dir == SEM_FAILED || mutex == SEM_FAILED) {
        perror("Erro ao abrir semáforos");
        exit(1);
    }
    
    // Seed para números aleatórios (único por processo)
    srand(time(NULL) ^ (getpid() << 16));
    
    // Loop de refeições
    for (int i = 0; i < NUM_REFEICOES; i++) {
        pensar(id);
        
        printf("Filósofo %d está com fome (tentativa %d).\n", id, i + 1);
        
        // Seção crítica para pegar os garfos
        sem_wait(mutex);
        
        // Pega o garfo esquerdo
        printf("Filósofo %d está pegando garfo esquerdo (%d).\n", id, garfo_esq_id);
        sem_wait(garfo_esq);
        printf("Filósofo %d pegou garfo esquerdo (%d).\n", id, garfo_esq_id);
        
        // Pega o garfo direito
        printf("Filósofo %d está pegando garfo direito (%d).\n", id, garfo_dir_id);
        sem_wait(garfo_dir);
        printf("Filósofo %d pegou garfo direito (%d).\n", id, garfo_dir_id);
        
        sem_post(mutex);
        
        // Come
        comer(id);
        
        // Libera os garfos
        printf("Filósofo %d está liberando os garfos.\n", id);
        sem_post(garfo_esq);
        sem_post(garfo_dir);
    }
    
    printf("Filósofo %d terminou todas as refeições e está saindo.\n", id);
    
    // Fecha os semáforos
    sem_close(garfo_esq);
    sem_close(garfo_dir);
    sem_close(mutex);
    
    exit(0);
}

// Função para criar os semáforos
void criar_semaforos(int num_filosofos) {
    char sem_name[50];
    sem_t *sem;
    
    // Cria semáforo mutex para proteção da seção crítica
    sem = sem_open(SEM_MUTEX, O_CREAT | O_EXCL, 0644, 1);
    if (sem == SEM_FAILED) {
        perror("Erro ao criar semáforo mutex");
        exit(1);
    }
    sem_close(sem);
    
    // Cria um semáforo para cada garfo (inicializado com 1)
    for (int i = 0; i < num_filosofos; i++) {
        sprintf(sem_name, "%s%d", SEM_GARFO_PREFIX, i);
        sem = sem_open(sem_name, O_CREAT | O_EXCL, 0644, 1);
        if (sem == SEM_FAILED) {
            perror("Erro ao criar semáforo de garfo");
            exit(1);
        }
        sem_close(sem);
    }
}

// Função para remover os semáforos
void remover_semaforos(int num_filosofos) {
    char sem_name[50];
    
    // Remove semáforo mutex
    sem_unlink(SEM_MUTEX);
    
    // Remove semáforos dos garfos
    for (int i = 0; i < num_filosofos; i++) {
        sprintf(sem_name, "%s%d", SEM_GARFO_PREFIX, i);
        sem_unlink(sem_name);
    }
}

int main(int argc, char *argv[]) {
    int num_filosofos;
    pid_t pids[MAX_FILOSOFOS];
    
    // Verifica argumentos
    if (argc != 2) {
        printf("Uso: %s <numero_de_filosofos>\n", argv[0]);
        printf("Número de filósofos deve estar entre 2 e %d\n", MAX_FILOSOFOS);
        return 1;
    }
    
    num_filosofos = atoi(argv[1]);
    
    if (num_filosofos < 2 || num_filosofos > MAX_FILOSOFOS) {
        printf("Número inválido de filósofos. Deve estar entre 2 e %d\n", MAX_FILOSOFOS);
        return 1;
    }
    
    printf("=== Problema dos Filósofos Glutões ===\n");
    printf("Número de filósofos: %d\n", num_filosofos);
    printf("Número de refeições por filósofo: %d\n\n", NUM_REFEICOES);
    
    // Remove semáforos antigos (caso existam)
    remover_semaforos(num_filosofos);
    
    // Cria os semáforos
    criar_semaforos(num_filosofos);
    
    // Cria os processos filósofos
    for (int i = 0; i < num_filosofos; i++) {
        pids[i] = fork();
        
        if (pids[i] < 0) {
            perror("Erro ao criar processo");
            exit(1);
        } else if (pids[i] == 0) {
            // Processo filho (filósofo)
            filosofo(i, num_filosofos);
        }
    }
    
    // Processo pai aguarda todos os filósofos terminarem
    for (int i = 0; i < num_filosofos; i++) {
        waitpid(pids[i], NULL, 0);
    }
    
    printf("\n=== Todos os filósofos terminaram ===\n");
    
    // Remove os semáforos
    remover_semaforos(num_filosofos);
    
    return 0;
}