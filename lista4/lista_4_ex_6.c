#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>

#define MAX_USERS 10
#define MAX_MESSAGES 100
#define MAX_NAME_LEN 32
#define MAX_MSG_LEN 256
#define SHM_NAME "/chat_shm"
#define SEM_MUTEX "/chat_mutex"
#define SEM_MSG "/chat_msg_sem"

typedef struct {
    char username[MAX_NAME_LEN];
    char text[MAX_MSG_LEN];
    time_t timestamp;
    int active;
} Message;

typedef struct {
    char name[MAX_NAME_LEN];
    pid_t pid;
    int active;
} User;

typedef struct {
    User users[MAX_USERS];
    Message messages[MAX_MESSAGES];
    int msg_count;
    int user_count;
} ChatRoom;

// Variáveis globais
ChatRoom *chat_room = NULL;
sem_t *sem_mutex = NULL;
sem_t *sem_msg = NULL;
int shm_fd = -1;
char my_username[MAX_NAME_LEN];
int my_user_index = -1;
volatile int running = 1;

// Protótipos
void cleanup();
void signal_handler(int sig);
void *message_listener(void *arg);
void display_messages(int last_count);
int add_user(const char *username);
void remove_user();
void send_message(const char *text);

// Função de limpeza
void cleanup() {
    if (my_user_index >= 0 && chat_room != NULL) {
        sem_wait(sem_mutex);
        chat_room->users[my_user_index].active = 0;
        sem_post(sem_mutex);
    }
    
    if (chat_room != NULL) {
        munmap(chat_room, sizeof(ChatRoom));
    }
    
    if (sem_mutex != NULL) {
        sem_close(sem_mutex);
    }
    
    if (sem_msg != NULL) {
        sem_close(sem_msg);
    }
}

// Handler de sinais
void signal_handler(int sig) {
    running = 0;
    printf("\n\nSaindo do chat...\n");
}

// Thread que escuta novas mensagens
void *message_listener(void *arg) {
    int last_msg_count = 0;
    
    while (running) {
        sem_wait(sem_mutex);
        int current_count = chat_room->msg_count;
        sem_post(sem_mutex);
        
        if (current_count > last_msg_count) {
            display_messages(last_msg_count);
            last_msg_count = current_count;
            printf("\n[Você (%s)]: ", my_username);
            fflush(stdout);
        }
        
        usleep(100000); // 100ms
    }
    
    return NULL;
}

// Exibe mensagens novas
void display_messages(int last_count) {
    sem_wait(sem_mutex);
    
    for (int i = last_count; i < chat_room->msg_count; i++) {
        Message *msg = &chat_room->messages[i];
        if (msg->active) {
            struct tm *tm_info = localtime(&msg->timestamp);
            char time_str[20];
            strftime(time_str, sizeof(time_str), "%H:%M:%S", tm_info);
            
            printf("\n[%s] %s: %s", time_str, msg->username, msg->text);
        }
    }
    
    sem_post(sem_mutex);
}

// Adiciona usuário ao chat
int add_user(const char *username) {
    sem_wait(sem_mutex);
    
    int index = -1;
    for (int i = 0; i < MAX_USERS; i++) {
        if (!chat_room->users[i].active) {
            strncpy(chat_room->users[i].name, username, MAX_NAME_LEN - 1);
            chat_room->users[i].pid = getpid();
            chat_room->users[i].active = 1;
            index = i;
            chat_room->user_count++;
            break;
        }
    }
    
    sem_post(sem_mutex);
    return index;
}

// Remove usuário do chat
void remove_user() {
    if (my_user_index < 0) return;
    
    sem_wait(sem_mutex);
    chat_room->users[my_user_index].active = 0;
    chat_room->user_count--;
    sem_post(sem_mutex);
}

// Envia mensagem
void send_message(const char *text) {
    sem_wait(sem_mutex);
    
    int index = chat_room->msg_count % MAX_MESSAGES;
    Message *msg = &chat_room->messages[index];
    
    strncpy(msg->username, my_username, MAX_NAME_LEN - 1);
    strncpy(msg->text, text, MAX_MSG_LEN - 1);
    msg->timestamp = time(NULL);
    msg->active = 1;
    
    chat_room->msg_count++;
    
    sem_post(sem_mutex);
    sem_post(sem_msg);
}

// Lista usuários online
void list_users() {
    sem_wait(sem_mutex);
    
    printf("\n=== Usuários Online (%d) ===\n", chat_room->user_count);
    for (int i = 0; i < MAX_USERS; i++) {
        if (chat_room->users[i].active) {
            printf("  - %s (PID: %d)\n", 
                   chat_room->users[i].name, 
                   chat_room->users[i].pid);
        }
    }
    printf("========================\n");
    
    sem_post(sem_mutex);
}

int main(int argc, char *argv[]) {
    pthread_t listener_thread;
    char input[MAX_MSG_LEN];
    int is_first = 0;
    
    // Configurar handlers de sinal
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Solicitar nome de usuário
    printf("Digite seu nome de usuário: ");
    fgets(my_username, MAX_NAME_LEN, stdin);
    my_username[strcspn(my_username, "\n")] = 0;
    
    if (strlen(my_username) == 0) {
        fprintf(stderr, "Nome de usuário inválido!\n");
        return 1;
    }
    
    // Criar/abrir memória compartilhada
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }
    
    // Verificar se é o primeiro processo
    struct stat shm_stat;
    fstat(shm_fd, &shm_stat);
    if (shm_stat.st_size == 0) {
        is_first = 1;
        if (ftruncate(shm_fd, sizeof(ChatRoom)) == -1) {
            perror("ftruncate");
            close(shm_fd);
            return 1;
        }
    }
    
    // Mapear memória
    chat_room = mmap(NULL, sizeof(ChatRoom), PROT_READ | PROT_WRITE, 
                     MAP_SHARED, shm_fd, 0);
    if (chat_room == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        return 1;
    }
    
    // Abrir/criar semáforos
    sem_mutex = sem_open(SEM_MUTEX, O_CREAT, 0666, 1);
    if (sem_mutex == SEM_FAILED) {
        perror("sem_open mutex");
        cleanup();
        return 1;
    }
    
    sem_msg = sem_open(SEM_MSG, O_CREAT, 0666, 0);
    if (sem_msg == SEM_FAILED) {
        perror("sem_open msg");
        cleanup();
        return 1;
    }
    
    // Inicializar estrutura se for o primeiro
    if (is_first) {
        sem_wait(sem_mutex);
        memset(chat_room, 0, sizeof(ChatRoom));
        chat_room->msg_count = 0;
        chat_room->user_count = 0;
        sem_post(sem_mutex);
        printf("Sala de chat criada!\n");
    }
    
    // Adicionar usuário
    my_user_index = add_user(my_username);
    if (my_user_index == -1) {
        fprintf(stderr, "Sala de chat cheia!\n");
        cleanup();
        return 1;
    }
    
    printf("\n=== Bem-vindo ao Chat, %s! ===\n", my_username);
    printf("Comandos:\n");
    printf("  /users - Lista usuários online\n");
    printf("  /quit  - Sair do chat\n");
    printf("  /clear - Limpar tela\n");
    printf("=============================\n\n");
    
    // Exibir mensagens existentes
    display_messages(0);
    
    // Criar thread para escutar mensagens
    if (pthread_create(&listener_thread, NULL, message_listener, NULL) != 0) {
        perror("pthread_create");
        cleanup();
        return 1;
    }
    
    // Loop principal
    while (running) {
        printf("[Você (%s)]: ", my_username);
        fflush(stdout);
        
        if (fgets(input, MAX_MSG_LEN, stdin) == NULL) {
            break;
        }
        
        input[strcspn(input, "\n")] = 0;
        
        if (strlen(input) == 0) {
            continue;
        }
        
        // Processar comandos
        if (strcmp(input, "/quit") == 0) {
            running = 0;
            break;
        } else if (strcmp(input, "/users") == 0) {
            list_users();
        } else if (strcmp(input, "/clear") == 0) {
            system("clear || cls");
        } else {
            send_message(input);
        }
    }
    
    // Aguardar thread
    pthread_join(listener_thread, NULL);
    
    // Limpar
    remove_user();
    cleanup();
    
    printf("Até logo!\n");
    
    return 0;
}