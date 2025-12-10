/* common.h */
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

// Nomes para a memória compartilhada e semáforos
#define SHM_NAME "/exemplo_shm"
#define SEM_MUTEX "/sem_mutex"
#define SEM_EMPTY "/sem_empty"
#define SEM_FULL  "/sem_full"

// Tamanho do buffer (quantos números cabem na memória)
#define BUFFER_SIZE 5

// Estrutura da memória compartilhada
typedef struct {
    int buffer[BUFFER_SIZE];
    int in;  // Índice de inserção
    int out; // Índice de remoção
} shared_data_t;

#endif