#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>

#define TAMANHO_BUFFER 10
// Intervalo de tempo em segundos para o produtor gerar um item
#define PRODUTOR_DORMIR_TEMPO 1
// Intervalo de tempo em segundos para o consumidor consumir um item
#define CONSUMIDOR_DORMIR_TEMPO 2

typedef struct
{
    char buffer[TAMANHO_BUFFER]; // O buffer circular compartilhado
    int in;                   // Índice onde o produtor insere o próximo item
    int out;                  // Índice de onde o consumidor retira o próximo item

    sem_t mutex;
    sem_t empty;
    sem_t full;
} SharedData;

SharedData *shared_data;

void producer_task()
{
    printf("Processo Produtor iniciado (PID: %d)\n", getpid());

    while (1)
    {
        // Gera uma letra aleatória de 'A' a 'Z'
        char item = 'A' + (rand() % 26);

        sem_wait(&shared_data->empty);
        sem_wait(&shared_data->mutex);

        // --- SEÇÃO CRÍTICA ---
        shared_data->buffer[shared_data->in] = item;
        printf("Produtor produziu: %c na posição %d\n", item, shared_data->in);

        shared_data->in = (shared_data->in + 1) % TAMANHO_BUFFER;
        // --- FIM DA SEÇÃO CRÍTICA ---

        sem_post(&shared_data->mutex);
        sem_post(&shared_data->full);

        sleep(PRODUTOR_DORMIR_TEMPO);
    }
}

void consumer_task()
{
    printf("Processo Consumidor iniciado (PID: %d)\n", getpid());

    while (1)
    {
        sem_wait(&shared_data->full);
        sem_wait(&shared_data->mutex);

        // --- SEÇÃO CRÍTICA ---
        char item = shared_data->buffer[shared_data->out];
        printf("Consumidor consumiu: %c da posição %d\n", item, shared_data->out);

        shared_data->out = (shared_data->out + 1) % TAMANHO_BUFFER;
        // --- FIM DA SEÇÃO CRÍTICA ---

        sem_post(&shared_data->mutex);
        sem_post(&shared_data->empty);

        sleep(CONSUMIDOR_DORMIR_TEMPO);
    }
}

int main()
{
    srand(time(NULL));

    shared_data = mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (shared_data == MAP_FAILED)
    {
        perror("mmap falhou");
        exit(1);
    }

    shared_data->in = 0;
    shared_data->out = 0;

    sem_init(&shared_data->mutex, 1, 1);
    sem_init(&shared_data->empty, 1, TAMANHO_BUFFER);
    sem_init(&shared_data->full, 1, 0);

    // Cria o processo produtor
    pid_t producer_pid = fork();
    if (producer_pid < 0)
    {
        perror("fork do produtor falhou");
        exit(1);
    }
    else if (producer_pid == 0)
    {
        producer_task();
        exit(0);
    }

    // Cria o processo consumidor
    pid_t consumer_pid = fork();
    if (consumer_pid < 0)
    {
        perror("fork do consumidor falhou");
        exit(1);
    }
    else if (consumer_pid == 0)
    {
        consumer_task();
        exit(0); 
    }

    printf("Processo Pai (PID: %d) criou Produtor (PID: %d) e Consumidor (PID: %d)\n", getpid(), producer_pid, consumer_pid);
    printf("Pai esperando os filhos terminarem (neste caso, nunca, pois eles têm loops infinitos).\n");
    printf("Use Ctrl+C para encerrar o programa.\n");

    // Como os filhos estão em um loop infinito, o pai ficará bloqueado aqui para sempre.
    // Em um programa real, haveria uma condição de parada.
    wait(NULL);
    wait(NULL);

    // 5. LIMPEZA DOS RECURSOS (nunca vai chegar por causa do loop infinito)
    printf("Limpando recursos...\n");
    sem_destroy(&shared_data->mutex);
    sem_destroy(&shared_data->empty);
    sem_destroy(&shared_data->full);
    munmap(shared_data, sizeof(SharedData));

    return 0;
}