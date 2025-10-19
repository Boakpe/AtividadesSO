#include <stdio.h>
#include <stdlib.h>
#include <sys/statvfs.h>
#include <string.h>
#include <errno.h>

void exibir_informacoes(const char *caminho, struct statvfs *stat) {
    printf("\n╔═══════════════════════════════════════════════════════════╗\n");
    printf("║     INFORMAÇÕES DO SISTEMA DE ARQUIVOS                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
    printf("Caminho: %s\n\n", caminho);
    
    // Informações de blocos
    printf("┌─── BLOCOS ───────────────────────────────────────────────┐\n");
    printf("│ Tamanho do bloco (f_bsize):     %10lu bytes        │\n", stat->f_bsize);
    printf("│ Tamanho do fragmento (f_frsize): %10lu bytes        │\n", stat->f_frsize);
    printf("│ Total de blocos (f_blocks):     %10lu             │\n", stat->f_blocks);
    printf("│ Blocos livres (f_bfree):        %10lu             │\n", stat->f_bfree);
    printf("│ Blocos disponíveis (f_bavail):  %10lu             │\n", stat->f_bavail);
    printf("└──────────────────────────────────────────────────────────┘\n\n");
    
    // Informações de inodes
    printf("┌─── INODES ───────────────────────────────────────────────┐\n");
    printf("│ Total de inodes (f_files):      %10lu             │\n", stat->f_files);
    printf("│ Inodes livres (f_ffree):        %10lu             │\n", stat->f_ffree);
    printf("│ Inodes disponíveis (f_favail):  %10lu             │\n", stat->f_favail);
    printf("└──────────────────────────────────────────────────────────┘\n\n");
    
    // Outras informações
    printf("┌─── OUTRAS INFORMAÇÕES ───────────────────────────────────┐\n");
    printf("│ ID do sistema de arquivos:      %10lu             │\n", stat->f_fsid);
    printf("│ Flags de montagem:              0x%08lx             │\n", stat->f_flag);
    printf("│ Tamanho máx. nome arquivo:      %10lu caracteres  │\n", stat->f_namemax);
    printf("└──────────────────────────────────────────────────────────┘\n\n");
    
    // Cálculos de espaço
    unsigned long long tamanho_total = (unsigned long long)stat->f_blocks * stat->f_frsize;
    unsigned long long espaco_livre = (unsigned long long)stat->f_bfree * stat->f_frsize;
    unsigned long long espaco_disponivel = (unsigned long long)stat->f_bavail * stat->f_frsize;
    unsigned long long espaco_usado = tamanho_total - espaco_livre;
    
    printf("┌─── ESPAÇO EM DISCO ──────────────────────────────────────┐\n");
    printf("│ Tamanho total:          %10.2f GB (%12llu bytes) │\n", 
           tamanho_total / (1024.0 * 1024.0 * 1024.0), tamanho_total);
    printf("│ Espaço usado:           %10.2f GB (%12llu bytes) │\n", 
           espaco_usado / (1024.0 * 1024.0 * 1024.0), espaco_usado);
    printf("│ Espaço livre:           %10.2f GB (%12llu bytes) │\n", 
           espaco_livre / (1024.0 * 1024.0 * 1024.0), espaco_livre);
    printf("│ Disponível (não-root):  %10.2f GB (%12llu bytes) │\n", 
           espaco_disponivel / (1024.0 * 1024.0 * 1024.0), espaco_disponivel);
    
    if (tamanho_total > 0) {
        double percentual_usado = (espaco_usado * 100.0) / tamanho_total;
        printf("│ Uso:                    %10.2f%%                       │\n", percentual_usado);
    }
    printf("└──────────────────────────────────────────────────────────┘\n\n");
    
    // Flags de montagem decodificadas
    printf("┌─── FLAGS DE MONTAGEM ────────────────────────────────────┐\n");
    if (stat->f_flag & ST_RDONLY)
        printf("│ ✓ ST_RDONLY     - Somente leitura                       │\n");
    if (stat->f_flag & ST_NOSUID)
        printf("│ ✓ ST_NOSUID     - SUID/SGID ignorados                   │\n");
#ifdef ST_NODEV
    if (stat->f_flag & ST_NODEV)
        printf("│ ✓ ST_NODEV      - Dispositivos não permitidos           │\n");
#endif
#ifdef ST_NOEXEC
    if (stat->f_flag & ST_NOEXEC)
        printf("│ ✓ ST_NOEXEC     - Execução não permitida                │\n");
#endif
#ifdef ST_SYNCHRONOUS
    if (stat->f_flag & ST_SYNCHRONOUS)
        printf("│ ✓ ST_SYNCHRONOUS - Escritas síncronas                   │\n");
#endif
#ifdef ST_NOATIME
    if (stat->f_flag & ST_NOATIME)
        printf("│ ✓ ST_NOATIME    - Não atualiza tempo de acesso          │\n");
#endif
#ifdef ST_RELATIME
    if (stat->f_flag & ST_RELATIME)
        printf("│ ✓ ST_RELATIME   - Tempo de acesso relativo              │\n");
#endif
    if (stat->f_flag == 0)
        printf("│   Nenhuma flag especial definida                         │\n");
    printf("└──────────────────────────────────────────────────────────┘\n");
}

int main(int argc, char *argv[]) {
    struct statvfs stat;
    
    // Verifica argumentos
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <caminho>\n", argv[0]);
        fprintf(stderr, "\nExemplos:\n");
        fprintf(stderr, "  %s /\n", argv[0]);
        fprintf(stderr, "  %s /home\n", argv[0]);
        fprintf(stderr, "  %s .\n", argv[0]);
        fprintf(stderr, "  %s /tmp\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    const char *caminho = argv[1];
    
    // Chama statvfs
    if (statvfs(caminho, &stat) == -1) {
        fprintf(stderr, "\n❌ Erro ao obter informações do sistema de arquivos '%s'\n", caminho);
        fprintf(stderr, "   Motivo: %s\n\n", strerror(errno));
        return EXIT_FAILURE;
    }
    
    // Exibe as informações
    exibir_informacoes(caminho, &stat);
    
    return EXIT_SUCCESS;
}