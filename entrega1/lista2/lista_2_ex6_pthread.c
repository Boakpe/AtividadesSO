#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

// Estrutura para passar parâmetros para as threads
typedef struct {
    unsigned char *img;
    unsigned char *final;
    int w;
    int h;
    int c;
    int canal;
    int filtro[3][3];
} ThreadData;

// Função que cada thread executará
void *processar_canal(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    
    for (int j = 0; j < data->h; j++) {
        for (int i = 0; i < data->w; i++) {
            int novo = 0;
            
            // Aplica a convolução
            for (int ii = -1; ii < 2; ii++) {
                for (int jj = -1; jj < 2; jj++) {
                    int cordx = ii + i;
                    int cordy = jj + j;
                    int tmp = 0;
                    
                    if (cordx >= 0 && cordx < data->w && cordy >= 0 && cordy < data->h) {
                        tmp = *(data->img + data->canal + (cordx * data->c + cordy * data->w * data->c));
                    }
                    
                    novo += tmp * data->filtro[ii + 1][jj + 1];
                }
            }
            
            // Limita os valores entre 0 e 255
            if (novo < 0) novo = 0;
            if (novo > 255) novo = 255;
            
            *(data->final + data->canal + (i * data->c + j * data->w * data->c)) = novo;
        }
    }
    
    return NULL;
}

int main(void) {
    int w, h, c;
    unsigned char *img = stbi_load("imagem.jpg", &w, &h, &c, 0);
    
    if (img == NULL) {
        printf("Nao foi possivel ler a imagem\n");
        exit(1);
    }
    
    unsigned char *final = malloc(w * h * c);
    if (final == NULL) {
        printf("Erro ao alocar memoria\n");
        stbi_image_free(img);
        exit(1);
    }
    
    int filtro[3][3] = {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}};
    
    // Cria arrays para threads e dados
    pthread_t *threads = malloc(c * sizeof(pthread_t));
    ThreadData *thread_data = malloc(c * sizeof(ThreadData));
    
    if (threads == NULL || thread_data == NULL) {
        printf("Erro ao alocar memoria para threads\n");
        free(final);
        stbi_image_free(img);
        exit(1);
    }
    
    // Cria uma thread para cada canal
    for (int canal = 0; canal < c; canal++) {
        thread_data[canal].img = img;
        thread_data[canal].final = final;
        thread_data[canal].w = w;
        thread_data[canal].h = h;
        thread_data[canal].c = c;
        thread_data[canal].canal = canal;
        
        // Copia o filtro para a estrutura
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                thread_data[canal].filtro[i][j] = filtro[i][j];
            }
        }
        
        if (pthread_create(&threads[canal], NULL, processar_canal, &thread_data[canal]) != 0) {
            printf("Erro ao criar thread %d\n", canal);
            free(threads);
            free(thread_data);
            free(final);
            stbi_image_free(img);
            exit(1);
        }
    }
    
    // Aguarda todas as threads terminarem
    for (int canal = 0; canal < c; canal++) {
        pthread_join(threads[canal], NULL);
    }
    
    // Salva a imagem processada
    stbi_write_jpg("imagem2.jpg", w, h, c, final, 100);
    
    // Libera recursos
    free(threads);
    free(thread_data);
    stbi_image_free(img);
    free(final);
    
    printf("Processamento concluido com sucesso!\n");
    
    return 0;
}