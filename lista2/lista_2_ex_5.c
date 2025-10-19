#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>

#define NUM_FILHOS 4
#define NUM_THREADS 3

void *funcao_thread(void *arg)
{
    int tempo_aleatorio = (rand() % 9) + 2;
    printf("Sou a thread %lu. Vou dormir por %d segundos\n", pthread_self(), tempo_aleatorio);
    sleep(tempo_aleatorio);
}

int main()
{
    time_t tempo_inicio, tempo_fim;
    pid_t filhos[NUM_FILHOS];
    int pipefd[2];
    pipe(pipefd);

    tempo_inicio = time(NULL);

    for (int i = 0; i < NUM_FILHOS; i++)
    {
        filhos[i] = fork();

        if (filhos[i] < 0)
        {
            perror("Erro ao criar o filho");
            exit(1);
        }
        else if (filhos[i] == 0)
        {
            srand(time(NULL) + getpid());
            close(pipefd[0]);
            printf("Sou o filho %d.\n\n\n", getpid());

            pthread_t threads[NUM_THREADS];
            for (int j = 0; j < NUM_THREADS; j++)
            {
                pthread_create(&threads[j], NULL, funcao_thread, NULL);
            }

            for (int j = 0; j < NUM_THREADS; j++)
            {
                pthread_join(threads[j], NULL);
            }

            pid_t meu_pid = getpid();
            write(pipefd[1], &meu_pid, sizeof(meu_pid));
            close(pipefd[1]);
            exit(0);
        }
    }

    close(pipefd[1]);
    int buffer = 0;

    for (int i = 0; i < NUM_FILHOS; i++)
    {

        read(pipefd[0], &buffer, sizeof(buffer));
        if (i == 0)
        {
            printf("\n========\nPIDs recebidos:\n");
        }
        printf("> %d\n", buffer);
        fflush(stdout);
    }

    close(pipefd[0]);

    for (int i = 0; i < NUM_FILHOS; i++)
    {
        waitpid(filhos[i], NULL, 0);
    }

    tempo_fim = time(NULL);
    printf("\n\nTempo decorrido em segundos: %lu\n", tempo_fim - tempo_inicio);

    return 0;
}