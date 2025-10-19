#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// Contador compartilhado
int contador = 0;

// Variáveis para a Solução de Peterson
int flag[2] = {0, 0};  // Indica se cada thread quer entrar na seção crítica
int turn = 0;          // Indica de quem é a vez

// ============================================
// VERSÃO 1: SEM CONTROLE DE CONCORRÊNCIA
// ============================================
void* thread_sem_controle(void* arg) {
    int id = *(int*)arg;
    
    for(int i = 0; i < 5; i++) {
        // SEÇÃO CRÍTICA (sem proteção)
        int valor_local = contador;
        printf("Thread %d: leu contador = %d\n", id, valor_local);
        
        sched_yield(); // Força troca de contexto
        
        contador = valor_local + 1;
        printf("Thread %d: incrementou contador para %d\n", id, contador);
        // FIM DA SEÇÃO CRÍTICA
        
        usleep(100000); // 100ms entre iterações
    }
    
    return NULL;
}

// ============================================
// VERSÃO 2: COM SOLUÇÃO DE PETERSON
// ============================================
void* thread_peterson(void* arg) {
    int id = *(int*)arg;
    int outro = 1 - id;  // ID da outra thread (0->1 ou 1->0)
    
    for(int i = 0; i < 5; i++) {
        // ENTRADA NA SEÇÃO CRÍTICA (Algoritmo de Peterson)
        flag[id] = 1;           // Indica que quer entrar
        turn = outro;           // Dá preferência ao outro
        
        // Espera enquanto o outro quer entrar E é a vez dele
        while(flag[outro] == 1 && turn == outro) {
            // Espera ocupada (busy waiting)
        }
        
        // SEÇÃO CRÍTICA (protegida)
        int valor_local = contador;
        printf("Thread %d: leu contador = %d\n", id, valor_local);
        
        sched_yield(); // Força troca de contexto
        
        contador = valor_local + 1;
        printf("Thread %d: incrementou contador para %d\n", id, contador);
        // FIM DA SEÇÃO CRÍTICA
        
        // SAÍDA DA SEÇÃO CRÍTICA
        flag[id] = 0;  // Indica que saiu da seção crítica
        
        usleep(100000); // 100ms entre iterações
    }
    
    return NULL;
}

// ============================================
// FUNÇÃO PRINCIPAL
// ============================================
int main() {
    pthread_t threads[2];
    int ids[2] = {0, 1};
    
    printf("==============================================\n");
    printf("VERSÃO 1: SEM CONTROLE DE CONCORRÊNCIA\n");
    printf("==============================================\n");
    
    contador = 0;
    
    // Cria as threads sem controle
    for(int i = 0; i < 2; i++) {
        pthread_create(&threads[i], NULL, thread_sem_controle, &ids[i]);
    }
    
    // Aguarda finalização
    for(int i = 0; i < 2; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("\n>>> Valor final do contador (SEM controle): %d\n", contador);
    printf(">>> Valor esperado: 10\n\n");
    
    sleep(2);
    
    printf("==============================================\n");
    printf("VERSÃO 2: COM SOLUÇÃO DE PETERSON\n");
    printf("==============================================\n");
    
    // Reset das variáveis
    contador = 0;
    flag[0] = 0;
    flag[1] = 0;
    turn = 0;
    
    // Cria as threads com Peterson
    for(int i = 0; i < 2; i++) {
        pthread_create(&threads[i], NULL, thread_peterson, &ids[i]);
    }
    
    // Aguarda finalização
    for(int i = 0; i < 2; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("\n>>> Valor final do contador (COM Peterson): %d\n", contador);
    printf(">>> Valor esperado: 10\n\n");
    
    return 0;
}