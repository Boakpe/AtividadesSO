#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <time.h>
#include <string.h>

#define BUFFER_SIZE 10
#define NUM_ITEMS 20

// Estrutura do buffer compartilhado
typedef struct {
    char buffer[BUFFER_SIZE];
    int in;                    // Índice de inserção
    int out;                   // Índice de remoção
    sem_t mutex;               // Mutex para exclusão mútua
    sem_t empty;               // Semáforo para posições vazias
    sem_t full;                // Semáforo para posições cheias
} SharedBuffer;

// Função produtor
void produtor(SharedBuffer *sb, int intervalo) {
    srand(time(NULL) ^ getpid());
    
    for (int i = 0; i < NUM_ITEMS; i++) {
        // Gera letra aleatória (A-Z)
        char letra = 'A' + (rand() % 26);
        
        // Espera por espaço vazio
        sem_wait(&sb->empty);
        
        // Entra na região crítica
        sem_wait(&sb->mutex);
        
        // Adiciona item ao buffer
        sb->buffer[sb->in] = letra;
        printf("[PRODUTOR] Produziu: %c (posição %d)\n", letra, sb->in);
        sb->in = (sb->in + 1) % BUFFER_SIZE;
        
        // Sai da região crítica
        sem_post(&sb->mutex);
        
        // Sinaliza que há item disponível
        sem_post(&sb->full);
        
        // Aguarda intervalo
        usleep(intervalo * 1000);
    }
    
    printf("[PRODUTOR] Finalizou produção\n");
}

// Função consumidor
void consumidor(SharedBuffer *sb, int intervalo) {
    for (int i = 0; i < NUM_ITEMS; i++) {
        // Espera por item disponível
        sem_wait(&sb->full);
        
        // Entra na região crítica
        sem_wait(&sb->mutex);
        
        // Remove item do buffer
        char letra = sb->buffer[sb->out];
        printf("[CONSUMIDOR] Consumiu: %c (posição %d)\n", letra, sb->out);
        sb->out = (sb->out + 1) % BUFFER_SIZE;
        
        // Sai da região crítica
        sem_post(&sb->mutex);
        
        // Sinaliza que há espaço disponível
        sem_post(&sb->empty);
        
        // Aguarda intervalo
        usleep(intervalo * 1000);
    }
    
    printf("[CONSUMIDOR] Finalizou consumo\n");
}

int main(int argc, char *argv[]) {
    int intervalo_produtor = 100;    // ms
    int intervalo_consumidor = 150;  // ms
    
    // Lê intervalos da linha de comando (opcional)
    if (argc >= 3) {
        intervalo_produtor = atoi(argv[1]);
        intervalo_consumidor = atoi(argv[2]);
    }
    
    printf("=== Sistema Produtor-Consumidor ===\n");
    printf("Intervalo Produtor: %d ms\n", intervalo_produtor);
    printf("Intervalo Consumidor: %d ms\n", intervalo_consumidor);
    printf("Tamanho do Buffer: %d\n", BUFFER_SIZE);
    printf("Total de Itens: %d\n\n", NUM_ITEMS);
    
    // Cria área de memória compartilhada
    SharedBuffer *sb = mmap(NULL, sizeof(SharedBuffer),
                            PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS,
                            -1, 0);
    
    if (sb == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    
    // Inicializa buffer
    sb->in = 0;
    sb->out = 0;
    
    // Inicializa semáforos (segundo parâmetro = 1 para compartilhar entre processos)
    if (sem_init(&sb->mutex, 1, 1) == -1) {
        perror("sem_init mutex");
        exit(1);
    }
    
    if (sem_init(&sb->empty, 1, BUFFER_SIZE) == -1) {
        perror("sem_init empty");
        exit(1);
    }
    
    if (sem_init(&sb->full, 1, 0) == -1) {
        perror("sem_init full");
        exit(1);
    }
    
    // Cria processo produtor
    pid_t pid_produtor = fork();
    
    if (pid_produtor < 0) {
        perror("fork produtor");
        exit(1);
    }
    
    if (pid_produtor == 0) {
        // Processo filho - Produtor
        produtor(sb, intervalo_produtor);
        exit(0);
    }
    
    // Cria processo consumidor
    pid_t pid_consumidor = fork();
    
    if (pid_consumidor < 0) {
        perror("fork consumidor");
        exit(1);
    }
    
    if (pid_consumidor == 0) {
        // Processo filho - Consumidor
        consumidor(sb, intervalo_consumidor);
        exit(0);
    }
    
    // Processo pai espera pelos filhos
    printf("[PAI] Aguardando processos filhos...\n\n");
    
    waitpid(pid_produtor, NULL, 0);
    printf("\n[PAI] Produtor finalizado\n");
    
    waitpid(pid_consumidor, NULL, 0);
    printf("[PAI] Consumidor finalizado\n");
    
    // Destrói semáforos
    sem_destroy(&sb->mutex);
    sem_destroy(&sb->empty);
    sem_destroy(&sb->full);
    
    // Libera memória compartilhada
    if (munmap(sb, sizeof(SharedBuffer)) == -1) {
        perror("munmap");
        exit(1);
    }
    
    printf("\n[PAI] Programa finalizado com sucesso!\n");
    
    return 0;
}