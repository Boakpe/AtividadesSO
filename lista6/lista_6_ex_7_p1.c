// escritor.c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    const char *path = "ints.bin";
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    int arr[30];
    for (int i = 0; i < 30; ++i) {
        arr[i] = i + 1; // 1..30
    }

    size_t total = sizeof(arr);
    ssize_t written = write(fd, arr, total);
    if (written < 0) {
        perror("write");
        close(fd);
        return 1;
    }
    if ((size_t)written != total) {
        fprintf(stderr, "Escrita parcial: esperado %zu, escreveu %zd\n", total, written);
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}