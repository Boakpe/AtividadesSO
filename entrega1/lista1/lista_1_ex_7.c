#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define NUM_FILHOS 5

typedef struct  {
    pid_t pid;
    int numero;
} DadosFilho;

int main() {
    int fd[2]; 
    pid_t pids[NUM_FILHOS];


    if (pipe(fd) == -1) {
        perror("pipe falhou");
        exit(1);
    }


    for (int i = 0; i < NUM_FILHOS; i++) {
        pids[i] = fork();

        if (pids[i] < 0) {
            perror("fork falhou");
            exit(1);
        }

        if (pids[i] == 0) {
            close(fd[0]);

            srand(time(NULL) ^ getpid());

            int numero_sorteado = rand() % 201;

            DadosFilho dados;
            dados.pid = getpid();
            dados.numero = numero_sorteado;

            printf("Filho (PID %d) enviou o número %d.\n", dados.pid, dados.numero);

            write(fd[1], &dados, sizeof(DadosFilho));

            close(fd[1]);

            exit(0);
        }
    }

    close(fd[1]);

    int menor_numero = 201; 
    pid_t pid_filho_menor = -1;

    printf("\nPai (PID %d) esperando para receber os números...\n", getpid());

    for (int i = 0; i < NUM_FILHOS; i++) {
        DadosFilho dados_recebidos;

        read(fd[0], &dados_recebidos, sizeof(DadosFilho));

        printf("Pai recebeu %d do filho (PID %d).\n", dados_recebidos.numero, dados_recebidos.pid);

        if (dados_recebidos.numero < menor_numero) {
            menor_numero = dados_recebidos.numero;
            pid_filho_menor = dados_recebidos.pid;
        }
    }

    close(fd[0]);

    for (int i = 0; i < NUM_FILHOS; i++) {
        wait(NULL);
    }

    printf("\n--- Resultado Final ---\n");
    printf("O menor número recebido foi: %d\n", menor_numero);
    printf("Enviado pelo filho com PID: %d\n", pid_filho_menor);

    return 0;
}