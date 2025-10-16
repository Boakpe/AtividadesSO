#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <time.h>

#define NUM_FILHOS 4
#define NUM_THREADS 3

// Função executada por cada thread
void* thread_function(void* arg) {
    int thread_id = *((int*)arg);
    
    // Gera um tempo aleatório entre 2 e 10 segundos
    int sleep_time = 2 + (rand() % 9); // 2 + [0-8] = [2-10]
    
    printf("  Thread %d do processo %d vai dormir por %d segundos\n", 
           thread_id, getpid(), sleep_time);
    
    sleep(sleep_time);
    
    printf("  Thread %d do processo %d acordou\n", thread_id, getpid());
    
    free(arg);
    pthread_exit(NULL);
}

int main() {
    int pipefd[2];
    pid_t pid;
    time_t start_time, end_time;
    pid_t pids_recebidos[NUM_FILHOS];
    
    // Registra o tempo de início
    start_time = time(NULL);
    
    // Cria o pipe para comunicação entre processos
    if (pipe(pipefd) == -1) {
        perror("Erro ao criar pipe");
        exit(EXIT_FAILURE);
    }
    
    printf("========================================\n");
    printf("Processo pai (PID: %d) iniciando...\n", getpid());
    printf("========================================\n\n");
    
    // Cria 4 processos filhos
    for (int i = 0; i < NUM_FILHOS; i++) {
        pid = fork();
        
        if (pid < 0) {
            perror("Erro ao criar processo filho");
            exit(EXIT_FAILURE);
        }
        
        if (pid == 0) {
            // ========== CÓDIGO DO PROCESSO FILHO ==========
            
            // Fecha o lado de leitura do pipe (filho só escreve)
            close(pipefd[0]);
            
            // Inicializa o gerador de números aleatórios
            srand(time(NULL) + getpid());
            
            printf("Processo filho %d iniciado\n", getpid());
            
            pthread_t threads[NUM_THREADS];
            
            // Cria 3 threads
            for (int j = 0; j < NUM_THREADS; j++) {
                int* thread_id = malloc(sizeof(int));
                *thread_id = j + 1;
                
                if (pthread_create(&threads[j], NULL, thread_function, thread_id) != 0) {
                    perror("Erro ao criar thread");
                    exit(EXIT_FAILURE);
                }
            }
            
            // Espera todas as threads terminarem
            for (int j = 0; j < NUM_THREADS; j++) {
                pthread_join(threads[j], NULL);
            }
            
            printf("Processo filho %d: todas as threads terminaram\n", getpid());
            
            // Envia o PID para o processo pai através do pipe
            pid_t meu_pid = getpid();
            write(pipefd[1], &meu_pid, sizeof(pid_t));
            
            // Fecha o lado de escrita do pipe
            close(pipefd[1]);
            
            printf("Processo filho %d terminando\n\n", getpid());
            exit(EXIT_SUCCESS);
        }
        
        // Pequeno delay para garantir sementes aleatórias diferentes
        usleep(100000); // 0.1 segundo
    }
    
    // ========== CÓDIGO DO PROCESSO PAI ==========
    
    // Fecha o lado de escrita do pipe (pai só lê)
    close(pipefd[1]);
    
    // Recebe os PIDs dos 4 filhos
    printf("Processo pai aguardando PIDs dos filhos...\n\n");
    for (int i = 0; i < NUM_FILHOS; i++) {
        read(pipefd[0], &pids_recebidos[i], sizeof(pid_t));
        printf("Processo pai recebeu PID: %d\n", pids_recebidos[i]);
    }
    
    // Fecha o lado de leitura do pipe
    close(pipefd[0]);
    
    printf("\nProcesso pai aguardando término de todos os filhos...\n");
    
    // Espera todos os processos filhos terminarem
    for (int i = 0; i < NUM_FILHOS; i++) {
        wait(NULL);
    }
    
    // Registra o tempo final
    end_time = time(NULL);
    
    // Imprime os resultados finais
    printf("\n========================================\n");
    printf("RESULTADO FINAL\n");
    printf("========================================\n");
    printf("Todos os %d processos filhos terminaram\n", NUM_FILHOS);
    printf("\nPIDs recebidos: ");
    for (int i = 0; i < NUM_FILHOS; i++) {
        printf("%d ", pids_recebidos[i]);
    }
    printf("\n\nTempo total transcorrido: %ld segundos\n", end_time - start_time);
    printf("========================================\n");
    
    return 0;
}