#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>

/*
 * Conversão de bytes para Gigabytes para facilitar a leitura
 */
double bytes_to_gb(unsigned long blocks, unsigned long block_size) {
    return (double)(blocks * block_size) / (1024.0 * 1024.0 * 1024.0);
}

int main(int argc, char *argv[]) {
    // Verifica se o usuário passou o argumento (nome do fs)
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <nome_do_sistema_de_arquivos>\n", argv[0]);
        fprintf(stderr, "Exemplo: %s ext4\n", argv[0]);
        fprintf(stderr, "Exemplo: %s tmpfs\n", argv[0]);
        exit(1);
    }

    char *target_fs = argv[1];
    
    // Abre o arquivo /proc/mounts para leitura
    FILE *fp = fopen("/proc/mounts", "r");
    if (fp == NULL) {
        perror("Erro ao abrir /proc/mounts");
        exit(1);
    }

    // Variáveis para armazenar os dados de cada linha do /proc/mounts
    char device[256];
    char mount_point[256];
    char fs_type[256];
    char options[256];
    int dump, pass;

    // Estrutura para armazenar o resultado do statvfs
    struct statvfs vfs;
    int found = 0;

    // Cabeçalho da tabela de saída
    printf("%-20s %-25s %-10s %-10s %-10s\n", 
           "Dispositivo", "Ponto de Montagem", "Total(GB)", "Livre(GB)", "Inodes");
    printf("-------------------------------------------------------------------------------------\n");

    // Lê o arquivo linha por linha
    // Formato padrão: device mount_point fs_type options dump pass
    while (fscanf(fp, "%255s %255s %255s %255s %d %d", 
                  device, mount_point, fs_type, options, &dump, &pass) != EOF) {
        
        // Compara o tipo de sistema de arquivos lido com o solicitado pelo usuário
        if (strcmp(fs_type, target_fs) == 0) {
            found = 1;

            // Chama statvfs para obter informações sobre a partição montada
            if (statvfs(mount_point, &vfs) == 0) {
                // f_blocks: Total de blocos
                // f_bavail: Blocos livres para usuários não-privilegiados
                // f_frsize: Tamanho do fragmento (normalmente o tamanho do bloco)
                
                double total_gb = bytes_to_gb(vfs.f_blocks, vfs.f_frsize);
                double free_gb = bytes_to_gb(vfs.f_bavail, vfs.f_frsize);

                printf("%-20s %-25s %-10.2f %-10.2f %lu\n", 
                       device, 
                       mount_point, 
                       total_gb, 
                       free_gb, 
                       (unsigned long)vfs.f_files); // Total de nós de arquivos (inodes)
            } else {
                perror("statvfs falhou para um ponto de montagem");
            }
        }
    }

    if (!found) {
        printf("Nenhuma partição encontrada com o sistema de arquivos: %s\n", target_fs);
    }

    fclose(fp);
    return 0;
}