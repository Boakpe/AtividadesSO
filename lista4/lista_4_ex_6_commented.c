#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>

// ============= CONFIGURAÇÕES =============
#define MAX_MSGS 100          // Máximo de mensagens armazenadas
#define MAX_MSG_LEN 256       // Tamanho máximo de cada mensagem
#define MAX_USER_LEN 32       // Tamanho máximo do nome de usuário
#define SHM_NAME "/chat_shm"  // Nome da memória compartilhada
#define SEM_NAME "/chat_sem"  // Nome do semáforo

// ============= ESTRUTURAS =============

// Estrutura que representa uma mensagem individual
typedef struct {
    char usuario[MAX_USER_LEN];  // Nome de quem enviou
    char texto[MAX_MSG_LEN];     // Conteúdo da mensagem
    int id;                       // ID único da mensagem
} Mensagem;

// Estrutura da memória compartilhada entre todos os processos
typedef struct {
    Mensagem mensagens[MAX_MSGS];  // Array circular de mensagens
    int total_msgs;                 // Contador total de mensagens enviadas
} ChatMemoria;

// ============= VARIÁVEIS GLOBAIS =============
ChatMemoria *chat_mem = NULL;     // Ponteiro para memória compartilhada
sem_t *sem = NULL;                // Ponteiro para o semáforo
char meu_usuario[MAX_USER_LEN];   // Nome do usuário atual
int ultima_msg_lida = 0;          // Controle de mensagens já exibidas
int rodando = 1;                  // Flag para controlar threads

// ============= FUNÇÕES =============

/**
 * Thread responsável por monitorar e exibir novas mensagens
 * Fica em loop verificando se há mensagens novas na memória compartilhada
 */
void *monitorar_mensagens(void *arg) {
    while (rodando) {
        // ENTRA NA REGIÃO CRÍTICA (protegida pelo semáforo)
        sem_wait(sem);
        
        // Verifica se o total de mensagens aumentou
        if (chat_mem->total_msgs > ultima_msg_lida) {
            // Imprime todas as mensagens novas
            for (int i = ultima_msg_lida; i < chat_mem->total_msgs; i++) {
                // Usa módulo para implementar buffer circular
                int idx = i % MAX_MSGS;
                Mensagem *msg = &chat_mem->mensagens[idx];
                
                // Exibe a mensagem
                printf("\n[%s]: %s\n> ", msg->usuario, msg->texto);
                fflush(stdout);  // Força a exibição imediata
            }
            
            // Atualiza o controle de mensagens lidas
            ultima_msg_lida = chat_mem->total_msgs;
        }
        
        // SAI DA REGIÃO CRÍTICA
        sem_post(sem);
        
        // Pequena pausa para não sobrecarregar CPU
        usleep(100000);  // 100 milissegundos
    }
    return NULL;
}

/**
 * Envia uma mensagem para a memória compartilhada
 */
void enviar_mensagem(const char *texto) {
    // ENTRA NA REGIÃO CRÍTICA
    sem_wait(sem);
    
    // Calcula posição no array circular
    int idx = chat_mem->total_msgs % MAX_MSGS;
    
    // Preenche a estrutura da mensagem
    strncpy(chat_mem->mensagens[idx].usuario, meu_usuario, MAX_USER_LEN - 1);
    strncpy(chat_mem->mensagens[idx].texto, texto, MAX_MSG_LEN - 1);
    chat_mem->mensagens[idx].id = chat_mem->total_msgs;
    
    // Incrementa contador global de mensagens
    chat_mem->total_msgs++;
    
    // SAI DA REGIÃO CRÍTICA
    sem_post(sem);
}

/**
 * Inicializa a memória compartilhada e o semáforo
 */
void inicializar_chat() {
    // PASSO 1: Criar/abrir memória compartilhada
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Erro ao criar memória compartilhada");
        exit(1);
    }
    
    // PASSO 2: Definir tamanho da memória compartilhada
    if (ftruncate(shm_fd, sizeof(ChatMemoria)) == -1) {
        perror("Erro ao dimensionar memória compartilhada");
        exit(1);
    }
    
    // PASSO 3: Mapear a memória compartilhada no espaço de endereçamento
    chat_mem = mmap(NULL, sizeof(ChatMemoria), 
                    PROT_READ | PROT_WRITE,  // Permissões de leitura/escrita
                    MAP_SHARED,              // Compartilhado entre processos
                    shm_fd, 0);
    
    if (chat_mem == MAP_FAILED) {
        perror("Erro ao mapear memória compartilhada");
        exit(1);
    }
    
    // PASSO 4: Criar/abrir semáforo nomeado
    // O valor inicial 1 indica que é um mutex (semáforo binário)
    sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("Erro ao criar semáforo");
        exit(1);
    }
    
    // Não precisamos mais do file descriptor
    close(shm_fd);
}

/**
 * Libera recursos antes de encerrar o programa
 */
void limpar_chat() {
    // Desmapeia a memória compartilhada
    if (chat_mem != NULL) {
        munmap(chat_mem, sizeof(ChatMemoria));
    }
    
    // Fecha o semáforo
    if (sem != NULL) {
        sem_close(sem);
    }
}

/**
 * Função principal
 */
int main() {
    char buffer[MAX_MSG_LEN];
    pthread_t thread_monitor;
    
    // INTERFACE INICIAL
    printf("╔════════════════════════════════════════╗\n");
    printf("║  CHAT COM MEMÓRIA COMPARTILHADA       ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    
    // Solicita nome do usuário
    printf("Digite seu nome de usuário: ");
    if (fgets(meu_usuario, MAX_USER_LEN, stdin) == NULL) {
        fprintf(stderr, "Erro ao ler nome de usuário\n");
        return 1;
    }
    meu_usuario[strcspn(meu_usuario, "\n")] = 0;  // Remove quebra de linha
    
    // Valida nome
    if (strlen(meu_usuario) == 0) {
        fprintf(stderr, "Nome de usuário não pode ser vazio\n");
        return 1;
    }
    
    // INICIALIZAÇÃO
    inicializar_chat();
    
    // Sincroniza posição inicial (não exibe mensagens antigas)
    sem_wait(sem);
    ultima_msg_lida = chat_mem->total_msgs;
    sem_post(sem);
    
    printf("\n✓ Chat iniciado como '%s'\n", meu_usuario);
    printf("✓ Digite suas mensagens (ou 'sair' para encerrar)\n\n");
    
    // CRIA THREAD PARA MONITORAR MENSAGENS
    if (pthread_create(&thread_monitor, NULL, monitorar_mensagens, NULL) != 0) {
        perror("Erro ao criar thread");
        limpar_chat();
        return 1;
    }
    
    // LOOP PRINCIPAL - Lê e envia mensagens do usuário
    while (1) {
        printf("> ");
        fflush(stdout);
        
        // Lê linha do usuário
        if (fgets(buffer, MAX_MSG_LEN, stdin) == NULL) {
            break;
        }
        
        // Remove quebra de linha
        buffer[strcspn(buffer, "\n")] = 0;
        
        // Verifica comando de saída
        if (strcmp(buffer, "sair") == 0) {
            break;
        }
        
        // Envia mensagem se não estiver vazia
        if (strlen(buffer) > 0) {
            enviar_mensagem(buffer);
        }
    }
    
    // ENCERRAMENTO
    printf("\nEncerrando chat...\n");
    rodando = 0;                        // Sinaliza thread para parar
    pthread_join(thread_monitor, NULL);  // Aguarda thread terminar
    limpar_chat();                       // Libera recursos
    
    printf("✓ Chat encerrado!\n");
    
    return 0;
}