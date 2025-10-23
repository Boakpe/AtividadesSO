#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>

#define MAX_CMD_LEN 1024
#define MAX_ARGS 64



void list_dir() {
    DIR *d;
    struct dirent *dir;
    d = opendir(".");

    if (d) {
        while ((dir = readdir(d)) != NULL) {
            // Não exibe os diretórios "." e ".."
            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                printf("%s\n", dir->d_name);
            }
        }
        closedir(d); // Fecha o diretório
    } else {
        perror("Erro ao abrir diretório");
    }
}

int main() {
    char cmd[MAX_CMD_LEN]; 
    char *args[MAX_ARGS]; 
    pid_t pid;

    while (1) {
        printf("mini-shell> ");
        fflush(stdout);

        
        if (fgets(cmd, sizeof(cmd), stdin) == NULL) {
            break;
        }

        cmd[strcspn(cmd, "\n")] = 0;

        // Analisa a linha de comando para separar o comando e seus argumentos
        char *token;
        int i = 0;
        token = strtok(cmd, " "); // Divide a string por espaços
        while (token != NULL) {
            args[i] = token;
            i++;
            token = strtok(NULL, " ");
        }
        args[i] = NULL; // O último elemento do array de argumentos deve ser NULL para o execve

        // Verifica se algum comando foi digitado
        if (args[0] == NULL) {
            continue; // Se o usuário apenas apertar Enter, volta ao início do loop
        }

        // Verificar se é um comando interno
        if (strcmp(args[0], "exit") == 0) {
            break; // Comando para sair do shell
        }

        if (strcmp(args[0], "ls") == 0) {
            list_dir(); 
            continue; 
        }

        // Cria um novo processo para executar o comando externo
        pid = fork();

        if (pid < 0) {
            perror("Erro no fork");
            exit(1);
        } else if (pid == 0) {
            if (execvp(args[0], args) < 0) {
                perror("Comando não encontrado");
                exit(1);
            }
        } else {
            wait(NULL);
        }
    }

    printf("\nSaindo do mini-shell.\n");
    return 0;
}