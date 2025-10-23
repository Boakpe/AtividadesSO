#include <stdio.h>
#include <stdlib.h>
#include <sys/uio.h>  
#include <fcntl.h>    
#include <unistd.h>   

int main() {
    const char *nome_arquivo = "numeros.bin";

    int fd = open(nome_arquivo, O_RDONLY);

    if (fd == -1) {
        perror("Erro ao abrir o arquivo para leitura");
        return 1;
    }
    
    off_t offset = 9 * sizeof(int);
    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("Erro ao posicionar o cursor no arquivo");
        close(fd);
        return 1;
    }

    int numeros_lidos[8];

    struct iovec iov[8];

    for (int i = 0; i < 8; i++) {
        iov[i].iov_base = &numeros_lidos[i]; 
        iov[i].iov_len = sizeof(int);        
    }

    ssize_t bytes_lidos = readv(fd, iov, 8);

    if (bytes_lidos == -1) {
        perror("Erro ao ler o arquivo com readv");
        close(fd);
        return 1;
    }

    printf("Leitura realizada com sucesso a partir do 10º inteiro.\n");
    printf("Total de bytes lidos: %ld\n", bytes_lidos);
    printf("Números lidos do arquivo:\n");

    for (int i = 0; i < 8; i++) {
        printf("Buffer %d: %d\n", i + 1, numeros_lidos[i]);
    }

    close(fd);

    return 0;
}