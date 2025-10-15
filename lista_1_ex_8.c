#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

void filho_executa() {
    int contador = 0;

    while (1)
    {
        printf("Contador: %d\n", contador);
        contador++;
        sleep(1);
    }
}

int main() {
    pid_t filhos[3];
    for (int i = 0; i < 3; i++) {
        filhos[i] = fork();

        if (filhos[i] == 0) {
            printf("Filho %d criado com sucesso!. Meu PID: %d\n", i, getpid());
            filho_executa();
            exit(0);
        } else if (filhos[i] < 0) {
            perror("Erro ao criar o filho!\n");
            exit(1);
        }
    }

    printf("Filhos criados com sucesso.\n");
    sleep(1);

    for (int i = 0; i < 3; i++) {
        kill(filhos[i], SIGSTOP);
    }

    printf("Filhos foram parados.\n");


    for (int i = 0; i < 3; i++) {
        printf("Filho %d:\n", filhos[i]);
        kill(filhos[i], SIGCONT);
        sleep(10);
        kill(filhos[i], SIGSTOP);
    }

    for (int i = 0; i < 3; i++) {
        kill(filhos[i], SIGKILL);
    }

    return 0;
}