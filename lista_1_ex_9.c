#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int main()
{
    pid_t filhos[10];
    srand(time(NULL));

    for (int i = 0; i < 10; i++)
    {
        int numero_aleatorio = rand() % 10;
        char buffer[12];
        snprintf(buffer, sizeof(buffer), "%d\n", numero_aleatorio);
        setenv("num", buffer, 1);
        filhos[i] = fork();

        if (filhos[i] == 0)
        {
            char *mensagem = getenv("num");
            printf("Sou o filho com PID: %d. Minha variável de ambiente tem o seguinte número: %s", getpid(), mensagem);
            exit(0);
        }
        else if (filhos[i] < 0)
        {
            perror("Erro ao criar os filhos!");
            exit(1);
        }
    }

    for (int i = 0; i < 10; i++)
    {
        wait(NULL);
    }
    
    return 0;
}