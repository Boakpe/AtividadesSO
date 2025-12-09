#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int contador = 0;

// Variáveis para a Solução de Peterson
int flag[2] = {0, 0};
int turn = 0;

void *thread_sem_controle(void *arg)
{
    int id = *(int *)arg;

    for (int i = 0; i < 5; i++)
    {
        // SEÇÃO CRÍTICA
        int valor_local = contador;
        printf("Thread %d: leu contador = %d\n", id, valor_local);

        sched_yield();

        contador = valor_local + 1;
        printf("Thread %d: incrementou contador para %d\n", id, contador);
        // FIM DA SEÇÃO CRÍTICA

        sleep(1);
    }

    return NULL;
}

void *thread_peterson(void *arg)
{
    int id = *(int *)arg;
    int outro = 1 - id;

    for (int i = 0; i < 5; i++)
    {
        // ENTRADA NA SEÇÃO CRÍTICA
        flag[id] = 1;
        turn = outro;

        while (flag[outro] == 1 && turn == outro)
        {
            // Espera ocupada
        }

        // SEÇÃO CRÍTICA
        int valor_local = contador;
        printf("Thread %d: leu contador = %d\n", id, valor_local);

        sched_yield();

        contador = valor_local + 1;
        printf("Thread %d: incrementou contador para %d\n", id, contador);
        // FIM DA SEÇÃO CRÍTICA

        flag[id] = 0;

        sleep(1);
    }

    return NULL;
}

int main()
{
    pthread_t threads[2];
    int ids[2] = {0, 1};

    printf("==============================================\n");
    printf("VERSÃO 1: SEM CONTROLE DE CONCORRÊNCIA\n");
    printf("==============================================\n");

    contador = 0;

    // Cria as threads sem controle
    for (int i = 0; i < 2; i++)
    {
        pthread_create(&threads[i], NULL, thread_sem_controle, &ids[i]);
    }

    // Aguarda finalização
    for (int i = 0; i < 2; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("\n>>> Valor final do contador (SEM controle): %d\n", contador);
    printf(">>> Valor esperado: 10\n\n");

    sleep(2);

    printf("==============================================\n");
    printf("VERSÃO 2: COM SOLUÇÃO DE PETERSON\n");
    printf("==============================================\n");

    // Reset das variáveis
    contador = 0;
    flag[0] = 0;
    flag[1] = 0;
    turn = 0;

    // Cria as threads com Peterson
    for (int i = 0; i < 2; i++)
    {
        pthread_create(&threads[i], NULL, thread_peterson, &ids[i]);
    }

    // Aguarda finalização
    for (int i = 0; i < 2; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("\n>>> Valor final do contador (COM Peterson): %d\n", contador);
    printf(">>> Valor esperado: 10\n\n");

    return 0;
}