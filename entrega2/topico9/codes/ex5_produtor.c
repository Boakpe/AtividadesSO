/* produtor_mprotect.c */
#include "common.h"

int main()
{
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("shm_open");
        exit(1);
    }

    ftruncate(shm_fd, sizeof(shared_data_t));

    // --- MUDANÇA EXERCÍCIO 5 ---
    // 1. Inicialmente mapeia com PROT_NONE (sem acesso)
    printf("Mapeando memória com PROT_NONE...\n");
    shared_data_t *shared_mem = mmap(NULL, sizeof(shared_data_t),
                                     PROT_NONE, MAP_SHARED, shm_fd, 0);

    if (shared_mem == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    // Se tentássemos acessar shared_mem aqui, ocorreria um Segmentation Fault.

    printf("Alterando permissões com mprotect para READ|WRITE...\n");

    // 2. Usar mprotect para permitir acesso
    if (mprotect(shared_mem, sizeof(shared_data_t), PROT_READ | PROT_WRITE) == -1)
    {
        perror("mprotect");
        exit(1);
    }
    // ---------------------------

    // O restante do código segue idêntico ao exercício 4...
    shared_mem->in = 0;
    shared_mem->out = 0;

    sem_t *sem_mutex = sem_open(SEM_MUTEX, O_CREAT, 0666, 1);
    sem_t *sem_empty = sem_open(SEM_EMPTY, O_CREAT, 0666, BUFFER_SIZE);
    sem_t *sem_full = sem_open(SEM_FULL, O_CREAT, 0666, 0);

    srand(time(NULL));

    printf("Produtor iniciado (memória protegida e liberada).\n");

    while (1)
    {
        int num = rand() % 100;
        sem_wait(sem_empty);
        sem_wait(sem_mutex);

        shared_mem->buffer[shared_mem->in] = num;
        printf("[Produtor] Escreveu: %d\n", num);
        shared_mem->in = (shared_mem->in + 1) % BUFFER_SIZE;

        sem_post(sem_mutex);
        sem_post(sem_full);
        sleep(1);
    }
    return 0;
}