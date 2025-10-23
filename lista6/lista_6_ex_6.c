#include <stdio.h>
#include <sys/statvfs.h>
#include <stdlib.h>

// Função para formatar bytes em um formato legível (KB, MB, GB)
void format_bytes(unsigned long long bytes, char *buffer, int size) {
    const char *suffixes[] = {"B", "KB", "MB", "GB", "TB"};
    int i = 0;
    double d_bytes = bytes;

    while (d_bytes >= 1024 && i < 4) {
        d_bytes /= 1024;
        i++;
    }

    snprintf(buffer, size, "%.2f %s", d_bytes, suffixes[i]);
}


int main(int argc, char *argv[]) {
    // Validação dos Argumentos de Entrada
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <caminho_do_sistema_de_arquivos>\n", argv[0]);
        exit(1); 
    }

    // Declaração da Estrutura e Chamada da Função
    struct statvfs buf;

    if (statvfs(argv[1], &buf) != 0) {
        perror("Erro ao chamar statvfs"); 
        exit(2);
    }

    // Cálculo e Apresentação das Informações
    unsigned long long total_space = buf.f_blocks * buf.f_frsize;
    unsigned long long free_space = buf.f_bfree * buf.f_frsize;
    unsigned long long available_space = buf.f_bavail * buf.f_frsize;

    char total_str[32], free_str[32], avail_str[32];
    format_bytes(total_space, total_str, sizeof(total_str));
    format_bytes(free_space, free_str, sizeof(free_str));
    format_bytes(available_space, avail_str, sizeof(avail_str));

    printf("Informações do sistema de arquivos para: %s\n", argv[1]);
    printf("---------------------------------------------------\n");
    printf("Tamanho do bloco fundamental (f_frsize):     %lu bytes\n", buf.f_frsize);
    printf("ID do sistema de arquivos (f_fsid):          %lu\n", buf.f_fsid);
    printf("Número máximo de caracteres em nome de arq.: %lu\n", buf.f_namemax);
    printf("\n");
    printf("Espaço Total:         %s (%llu bytes)\n", total_str, total_space);
    printf("Espaço Livre:         %s (%llu bytes)\n", free_str, free_space);
    printf("Espaço Disponível (não-root): %s (%llu bytes)\n", avail_str, available_space);
    printf("\n");
    printf("Total de nós-i (inodes):      %llu\n", (unsigned long long)buf.f_files);
    printf("Nós-i (inodes) livres:        %llu\n", (unsigned long long)buf.f_ffree);

    return 0;
}