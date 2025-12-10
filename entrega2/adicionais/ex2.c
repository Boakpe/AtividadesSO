#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

// Função auxiliar para imprimir as permissões (ex: rwxr-xr-x)
void print_permissions(mode_t mode) {
    printf( (S_ISDIR(mode)) ? "d" : "-");
    printf( (mode & S_IRUSR) ? "r" : "-");
    printf( (mode & S_IWUSR) ? "w" : "-");
    printf( (mode & S_IXUSR) ? "x" : "-");
    printf( (mode & S_IRGRP) ? "r" : "-");
    printf( (mode & S_IWGRP) ? "w" : "-");
    printf( (mode & S_IXGRP) ? "x" : "-");
    printf( (mode & S_IROTH) ? "r" : "-");
    printf( (mode & S_IWOTH) ? "w" : "-");
    printf( (mode & S_IXOTH) ? "x" : "-");
}

int main(int argc, char *argv[]) {
    // Verifica se o diretório foi passado
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <caminho_do_diretorio>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *dir_path = argv[1];
    struct dirent *entry;
    struct stat file_stat;
    DIR *dp;
    
    // Buffer para armazenar o caminho completo (diretorio + / + nome_arquivo)
    char full_path[4096];

    // Abre o diretório
    dp = opendir(dir_path);
    if (dp == NULL) {
        perror("Erro ao abrir diretorio");
        exit(EXIT_FAILURE);
    }

    printf("Lendo propriedades do diretório: %s\n\n", dir_path);
    printf("%-11s %-10s %-10s %-10s %-20s %s\n", 
           "PERMISSOES", "USUARIO", "GRUPO", "TAMANHO", "MODIFICADO EM", "NOME");
    printf("-----------------------------------------------------------------------------------\n");

    // Loop para ler cada arquivo dentro do diretório
    while ((entry = readdir(dp))) {
        // Pula os diretórios "." (atual) e ".." (pai)
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Monta o caminho completo para que o stat() funcione
        // Ex: se dir é "/tmp" e arquivo é "a.txt", full_path vira "/tmp/a.txt"
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        // Obtém os metadados do arquivo
        if (stat(full_path, &file_stat) == -1) {
            perror("stat");
            continue;
        }

        // 1. Imprime Permissões
        print_permissions(file_stat.st_mode);
        printf(" ");

        // 2. Obtém Nome do Usuário (Dono)
        struct passwd *pw = getpwuid(file_stat.st_uid);
        if (pw) printf("%-10s ", pw->pw_name);
        else    printf("%-10d ", file_stat.st_uid); // Se não achar nome, imprime ID

        // 3. Obtém Nome do Grupo
        struct group *gr = getgrgid(file_stat.st_gid);
        if (gr) printf("%-10s ", gr->gr_name);
        else    printf("%-10d ", file_stat.st_gid);

        // 4. Tamanho do arquivo (bytes)
        printf("%-10ld ", file_stat.st_size);

        // 5. Data de Modificação
        char date_str[20];
        struct tm *tm_info = localtime(&file_stat.st_mtime);
        strftime(date_str, sizeof(date_str), "%Y-%m-%d %H:%M", tm_info);
        printf("%-20s ", date_str);

        // 6. Nome do Arquivo
        printf("%s\n", entry->d_name);
    }

    closedir(dp);
    return 0;
}