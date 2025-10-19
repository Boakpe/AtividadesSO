#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_CMD_LEN 1024
#define MAX_ARGS 64

// Função para implementar o comando ls interno
void cmd_ls() {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    
    dir = opendir(".");
    if (dir == NULL) {
        perror("Erro ao abrir diretório");
        return;
    }
    
    printf("\nConteúdo do diretório atual:\n");
    printf("-----------------------------\n");
    
    while ((entry = readdir(dir)) != NULL) {
        // Pula os diretórios . e ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
            
        // Obtém informações do arquivo
        if (stat(entry->d_name, &file_stat) == 0) {
            // Indica se é diretório
            if (S_ISDIR(file_stat.st_mode))
                printf("[DIR]  %s\n", entry->d_name);
            else
                printf("[ARQ]  %s\n", entry->d_name);
        } else {
            printf("       %s\n", entry->d_name);
        }
    }
    
    printf("-----------------------------\n\n");
    closedir(dir);
}

// Função para dividir a linha de comando em argumentos
int parse_command(char *cmd, char **args) {
    int i = 0;
    char *token = strtok(cmd, " \t\n");
    
    while (token != NULL && i < MAX_ARGS - 1) {
        args[i++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL;
    
    return i;
}

// Função para executar comandos externos usando fork/execve
void execute_command(char **args) {
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("Erro ao criar processo filho");
        return;
    }
    
    if (pid == 0) {
        // Processo filho
        // Tenta executar o comando
        execvp(args[0], args);
        
        // Se execvp retornar, houve erro
        perror("Erro ao executar comando");
        exit(1);
    } else {
        // Processo pai
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            printf("Comando terminou com código de erro: %d\n", WEXITSTATUS(status));
        }
    }
}

int main() {
    char cmd[MAX_CMD_LEN];
    char *args[MAX_ARGS];
    
    printf("========================================\n");
    printf("    Mini-Shell Unix - Bem-vindo!\n");
    printf("========================================\n");
    printf("Comandos disponíveis:\n");
    printf("  - ls: listar arquivos (comando interno)\n");
    printf("  - exit ou quit: sair do shell\n");
    printf("  - Qualquer outro comando do Linux\n");
    printf("========================================\n\n");
    
    while (1) {
        // Mostra o prompt
        printf("mini-shell> ");
        fflush(stdout);
        
        // Lê o comando
        if (fgets(cmd, MAX_CMD_LEN, stdin) == NULL) {
            printf("\n");
            break;
        }
        
        // Remove newline do final
        cmd[strcspn(cmd, "\n")] = 0;
        
        // Ignora linhas vazias
        if (strlen(cmd) == 0)
            continue;
        
        // Parse do comando
        int argc = parse_command(cmd, args);
        
        if (argc == 0)
            continue;
        
        // Verifica comandos internos
        if (strcmp(args[0], "exit") == 0 || strcmp(args[0], "quit") == 0) {
            printf("Encerrando mini-shell. Até logo!\n");
            break;
        } else if (strcmp(args[0], "ls") == 0) {
            // Comando interno ls
            cmd_ls();
        } else if (strcmp(args[0], "cd") == 0) {
            // Comando cd também pode ser interno
            if (argc < 2) {
                fprintf(stderr, "Uso: cd <diretório>\n");
            } else {
                if (chdir(args[1]) != 0) {
                    perror("cd");
                }
            }
        } else {
            // Comando externo
            execute_command(args);
        }
    }
    
    return 0;
}