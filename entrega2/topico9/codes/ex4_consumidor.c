/* consumidor.c */
#include "common.h"

int main() {
    // 1. Abrir objeto de memória compartilhada (já criado pelo produtor)
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) { 
        perror("shm_open (Execute o produtor primeiro)"); 
        exit(1); 
    }

    // 2. Mapear na memória
    shared_data_t *shared_mem = mmap(NULL, sizeof(shared_data_t), 
                                     PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED) { perror("mmap"); exit(1); }

    // 3. Abrir Semáforos existentes
    sem_t *sem_mutex = sem_open(SEM_MUTEX, 0);
    sem_t *sem_empty = sem_open(SEM_EMPTY, 0);
    sem_t *sem_full  = sem_open(SEM_FULL, 0);

    printf("Consumidor iniciado. Aguardando dados...\n");

    while (1) {
        // Espera haver algo para consumir
        sem_wait(sem_full);
        // Entra na região crítica
        sem_wait(sem_mutex);

        // Lê da memória
        int num = shared_mem->buffer[shared_mem->out];
        printf("                  [Consumidor] Leu: %d da posição %d\n", num, shared_mem->out);
        shared_mem->out = (shared_mem->out + 1) % BUFFER_SIZE;

        // Sai da região crítica
        sem_post(sem_mutex);
        // Sinaliza que há um espaço vazio
        sem_post(sem_empty);
        
        // Simula tempo de processamento
        sleep(2); 
    }
    return 0;
}