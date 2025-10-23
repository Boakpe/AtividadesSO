#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    // Para fork(), sleep()
#include <sys/wait.h>  // Para wait()
#include <fcntl.h>     // Para O_CREAT
#include <sys/stat.h>  // Para modos de permissão
#include <semaphore.h> // Para semáforos POSIX
#include <string.h>    // Para sprintf()

// Define um nome base para os semáforos dos garfos.
// O número do garfo será anexado a este nome.
#define SEM_GARFO_NOME_BASE "/garfo_filosofo"

// Função que representa o ciclo de vida de um filósofo
// Lógica de execução de cada filósofo
void ciclo_do_filosofo(int id, int num_filosofos)
{
    // Identifica os garfos à esquerda e à direita do filósofo
    int garfo_esquerda = id;
    int garfo_direita = (id + 1) % num_filosofos;

    // Gera os nomes dos semáforos para os garfos
    char nome_sem_esquerda[50], nome_sem_direita[50];
    sprintf(nome_sem_esquerda, "%s%d", SEM_GARFO_NOME_BASE, garfo_esquerda);
    sprintf(nome_sem_direita, "%s%d", SEM_GARFO_NOME_BASE, garfo_direita);

    // Abre os semáforos existentes (criados pelo processo pai)
    sem_t *sem_garfo_esquerda = sem_open(nome_sem_esquerda, 0);
    sem_t *sem_garfo_direita = sem_open(nome_sem_direita, 0);

    if (sem_garfo_esquerda == SEM_FAILED || sem_garfo_direita == SEM_FAILED)
    {
        perror("Filósofo não conseguiu abrir os semáforos");
        exit(EXIT_FAILURE);
    }

    // Loop infinito de pensar e comer
    while (1)
    {
        // PENSAR
        printf("Filósofo %d está PENSANDO.\n", id);
        sleep(rand() % 3 + 1); // Pensa por um tempo aleatório

        printf("Filósofo %d está com FOME e vai tentar pegar os garfos.\n", id);

        // PEGAR GARFOS (Entrada na Seção Crítica)
        // ESTRATÉGIA PARA EVITAR DEADLOCK:
        // O último filósofo pega o garfo da direita primeiro, enquanto os outros
        // pegam o da esquerda primeiro. Isso quebra a dependência circular.
        if (id == num_filosofos - 1)
        {
            sem_wait(sem_garfo_direita); // Pega garfo da direita
            printf("Filósofo %d pegou o garfo da DIREITA (%d).\n", id, garfo_direita);
            sem_wait(sem_garfo_esquerda); // Pega garfo da esquerda
            printf("Filósofo %d pegou o garfo da ESQUERDA (%d).\n", id, garfo_esquerda);
        }
        else
        {
            sem_wait(sem_garfo_esquerda); // Pega garfo da esquerda
            printf("Filósofo %d pegou o garfo da ESQUERDA (%d).\n", id, garfo_esquerda);
            sem_wait(sem_garfo_direita); // Pega garfo da direita
            printf("Filósofo %d pegou o garfo da DIREITA (%d).\n", id, garfo_direita);
        }

        // COMER
        printf(">>> Filósofo %d está COMENDO. <<<\n", id);
        sleep(rand() % 3 + 1); // Come por um tempo aleatório

        // DEVOLVER GARFOS (Saída da Seção Crítica)
        printf("Filósofo %d TERMINOU de comer e vai devolver os garfos.\n", id);
        sem_post(sem_garfo_esquerda); // Libera o garfo da esquerda
        sem_post(sem_garfo_direita);  // Libera o garfo da direita
    }

    // Este código nunca será alcançado devido ao while(1),
    // mas é uma boa prática fechar os semáforos.
    sem_close(sem_garfo_esquerda);
    sem_close(sem_garfo_direita);
}

int main(int argc, char *argv[])
{
    // 1. Validação dos Argumentos de Entrada
    if (argc != 2)
    {
        fprintf(stderr, "Uso: %s <numero_de_filosofos>\n", argv[0]);
        return 1;
    }

    int num_filosofos = atoi(argv[1]);

    if (num_filosofos < 2)
    {
        fprintf(stderr, "O número de filósofos deve ser pelo menos 2.\n");
        return 1;
    }

    // 2. Criação e Inicialização dos Semáforos (Garfos)
    // Cada garfo é um semáforo.
    for (int i = 0; i < num_filosofos; i++)
    {
        char nome_sem[50];
        sprintf(nome_sem, "%s%d", SEM_GARFO_NOME_BASE, i);

        // O_CREAT: cria o semáforo se ele não existir.
        // 0644: permissões de arquivo (leitura/escrita para o dono, leitura para outros).
        // 1: valor inicial do semáforo (1 = garfo está disponível).
        sem_t *sem = sem_open(nome_sem, O_CREAT, 0644, 1);
        if (sem == SEM_FAILED)
        {
            perror("Erro ao criar semáforo no processo pai");
            exit(EXIT_FAILURE);
        }
        // O semáforo foi criado no sistema, podemos fechá-lo neste processo.
        // Os processos filhos irão abri-lo pelo nome.
        sem_close(sem);
    }

    // 3. Criação dos Processos Filósofos
    pid_t pids[num_filosofos];
    for (int i = 0; i < num_filosofos; i++)
    {
        pids[i] = fork();

        if (pids[i] < 0)
        {
            perror("Erro no fork");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0)
        {
            // Este é o processo filho (um filósofo)
            ciclo_do_filosofo(i, num_filosofos);
            exit(0); // O filho termina sua execução aqui
        }
    }

    // 4. Processo Pai Espera por Todos os Filhos
    // O pai só continua após todos os filhos terminarem.
    printf("Pai esperando todos os filósofos terminarem (pressione Ctrl+C para encerrar)...\n");
    for (int i = 0; i < num_filosofos; i++)
    {
        wait(NULL);
    }

    // 5. Limpeza dos Semáforos
    // Após todos os processos terminarem, o pai remove os semáforos do sistema.
    printf("Pai limpando os semáforos...\n");
    for (int i = 0; i < num_filosofos; i++)
    {
        char nome_sem[50];
        sprintf(nome_sem, "%s%d", SEM_GARFO_NOME_BASE, i);
        if (sem_unlink(nome_sem) == -1)
        {
            perror("Erro ao desvincular semáforo");
        }
    }

    printf("Programa finalizado.\n");
    return 0;
}