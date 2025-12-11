#include "common.h"

int main()
{
    // 1. Criar/Abrir objeto de memória compartilhada
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("shm_open");
        exit(1);
    }

    // 2. Definir o tamanho da memória
    ftruncate(shm_fd, sizeof(shared_data_t));

    // 3. Mapear na memória do processo
    shared_data_t *shared_mem = mmap(NULL, sizeof(shared_data_t),
                                     PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    // Inicializar índices
    shared_mem->in = 0;
    shared_mem->out = 0;

    // 4. Criar/Abrir Semáforos
    sem_t *sem_mutex = sem_open(SEM_MUTEX, O_CREAT, 0666, 1);
    sem_t *sem_empty = sem_open(SEM_EMPTY, O_CREAT, 0666, BUFFER_SIZE);
    sem_t *sem_full = sem_open(SEM_FULL, O_CREAT, 0666, 0);

    srand(time(NULL));

    printf("Produtor iniciado. Gerando números...\n");

    while (1)
    {
        int num = rand() % 100; // Gera número aleatório

        // Espera haver espaço vazio
        sem_wait(sem_empty);
        // Entra na região crítica
        sem_wait(sem_mutex);

        // Escreve na memória
        shared_mem->buffer[shared_mem->in] = num;
        printf("[Produtor] Escreveu: %d na posição %d\n", num, shared_mem->in);
        shared_mem->in = (shared_mem->in + 1) % BUFFER_SIZE;

        // Sai da região crítica
        sem_post(sem_mutex);
        // Sinaliza que há um item novo (full)
        sem_post(sem_full);

        sleep(1); // Unidade de tempo configurável
    }

    // Limpeza (inalcançável no loop infinito)
    munmap(shared_mem, sizeof(shared_data_t));
    close(shm_fd);
    return 0;
}