#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>

#define MAX_CMD_LEN 1024
#define MAX_ARGS 64

// Função para o comando interno 'ls'
void list_dir() {
    DIR *d;
    struct dirent *dir;
    d = opendir("."); // Abre o diretório atual (".")

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
    char cmd[MAX_CMD_LEN]; // Buffer para armazenar o comando digitado
    char *args[MAX_ARGS]; // Array de ponteiros para os argumentos do comando
    pid_t pid;

    // Loop infinito para manter o shell em execução
    while (1) {
        // 1. Exibir o prompt
        printf("mini-shell> ");
        fflush(stdout); // Garante que o prompt seja exibido imediatamente

        // 2. Ler o comando do usuário
        if (fgets(cmd, sizeof(cmd), stdin) == NULL) {
            break; // Sai do loop se houver um erro ou fim de entrada (Ctrl+D)
        }

        // Remove a quebra de linha final (\n) que o fgets lê
        cmd[strcspn(cmd, "\n")] = 0;

        // 3. Analisar (parse) a linha de comando para separar o comando e seus argumentos
        char *token;
        int i = 0;
        token = strtok(cmd, " "); // Divide a string por espaços
        while (token != NULL) {
            args[i] = token;
            i++;
            token = strtok(NULL, " ");
        }
        args[i] = NULL; // O último elemento do array de argumentos deve ser NULL para o execve

        // 4. Verificar se algum comando foi digitado
        if (args[0] == NULL) {
            continue; // Se o usuário apenas apertar Enter, volta ao início do loop
        }

        // 5. Verificar se é um comando interno
        if (strcmp(args[0], "exit") == 0) {
            break; // Comando para sair do shell
        }

        if (strcmp(args[0], "ls") == 0) {
            list_dir(); // Chama a função interna para listar arquivos
            continue; // Volta ao início do loop após executar o comando interno
        }

        // 6. Criar um novo processo para executar o comando externo
        pid = fork();

        if (pid < 0) {
            // Erro ao criar o processo filho
            perror("Erro no fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // --- Este é o código que o PROCESSO FILHO executa ---
            
            // Tenta executar o comando
            // O primeiro argumento é o caminho para o executável
            // O segundo é o array de argumentos (incluindo o nome do comando)
            // O terceiro são as variáveis de ambiente (NULL para usar as do pai)
            if (execvp(args[0], args) < 0) {
                perror("Comando não encontrado"); // Mensagem de erro se o execvp falhar
                exit(EXIT_FAILURE); // Termina o processo filho
            }
        } else {
            // --- Este é o código que o PROCESSO PAI executa ---
            
            // O processo pai espera o processo filho terminar
            wait(NULL);
        }
    }

    printf("\nSaindo do mini-shell.\n");
    return 0;
}