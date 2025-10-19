// leitor.c
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    const char *path = "ints.bin";
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    // Pular os 9 primeiros ints para começar no 10º (offset = 9 * sizeof(int))
    off_t offset = (off_t)(9 * (off_t)sizeof(int));
    if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
        perror("lseek");
        close(fd);
        return 1;
    }

    // 8 buffers (um int em cada)
    int bufs[8] = {0};
    struct iovec iov[8];
    for (int i = 0; i < 8; ++i) {
        iov[i].iov_base = &bufs[i];
        iov[i].iov_len  = sizeof(int);
    }

    ssize_t expected = 8 * (ssize_t)sizeof(int);
    ssize_t nread = readv(fd, iov, 8); // apenas uma chamada a readv
    if (nread < 0) {
        perror("readv");
        close(fd);
        return 1;
    }
    if (nread != expected) {
        fprintf(stderr, "Leitura incompleta: esperado %zd bytes, leu %zd bytes\n", expected, nread);
        close(fd);
        return 1;
    }

    // Imprimir: buffer 1 = 10º, buffer 2 = 11º, ..., buffer 8 = 17º
    for (int i = 0; i < 8; ++i) {
        printf("Buffer %d: %d\n", i + 1, bufs[i]);
    }

    close(fd);
    return 0;
}