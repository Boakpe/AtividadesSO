#include "common.h"

int main()
{
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("shm_open (Execute o produtor primeiro)");
        exit(1);
    }

    // --- MUDANÇA EXERCÍCIO 5 ---
    // 1. Mapeia inicialmente SEM permissão (PROT_NONE)
    printf("Consumidor: Mapeando memória com PROT_NONE...\n");
    shared_data_t *shared_mem = mmap(NULL, sizeof(shared_data_t),
                                     PROT_NONE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    printf("Consumidor: Alterando permissões com mprotect...\n");

    // 2. Libera acesso de Leitura e Escrita
    // Nota: Precisa de WRITE também porque o consumidor altera a variável 'out'
    if (mprotect(shared_mem, sizeof(shared_data_t), PROT_READ | PROT_WRITE) == -1)
    {
        perror("mprotect");
        exit(1);
    }
    // ---------------------------

    sem_t *sem_mutex = sem_open(SEM_MUTEX, 0);
    sem_t *sem_empty = sem_open(SEM_EMPTY, 0);
    sem_t *sem_full = sem_open(SEM_FULL, 0);

    printf("Consumidor iniciado e memória desbloqueada.\n");

    while (1)
    {
        sem_wait(sem_full);
        sem_wait(sem_mutex);

        int num = shared_mem->buffer[shared_mem->out];
        printf("[Consumidor] Leu: %d\n", num);
        shared_mem->out = (shared_mem->out + 1) % BUFFER_SIZE;

        sem_post(sem_mutex);
        sem_post(sem_empty);

        sleep(2);
    }
    return 0;
}