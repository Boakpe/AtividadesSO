#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    pid_t filhos[10];

    for (int i = 0; i < 10; i++)
    {
        filhos[i] = fork();

        if (filhos[i] == 0)
        {
            printf("Mayoi! Quem quer dinheiro! 💵. Meu PID: %d\n", getpid());
            exit(0);
        }
        else if (filhos[i] < 0)
        {
            perror("Erro ao criar os filhos!");
            exit(1);
        }
    }

    sleep(1);
    printf("Hello World!\n");
    return 0;
}