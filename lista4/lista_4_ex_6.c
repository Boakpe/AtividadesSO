#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>

#define MAX_MSGS 100
#define MAX_MSG_LEN 256
#define MAX_USER_LEN 32
#define SHM_NAME "/chat_shm"
#define SEM_NAME "/chat_sem"

typedef struct
{
    char usuario[MAX_USER_LEN];
    char texto[MAX_MSG_LEN];
    int id;
} Mensagem;

typedef struct
{
    Mensagem mensagens[MAX_MSGS];
    int total_msgs;
} ChatMemoria;

ChatMemoria *chat_mem = NULL;
sem_t *sem = NULL;
char meu_usuario[MAX_USER_LEN];
int ultima_msg_lida = 0;
int rodando = 1;

void *monitorar_mensagens(void *arg)
{
    while (rodando)
    {
        sem_wait(sem);

        if (chat_mem->total_msgs > ultima_msg_lida)
        {
            for (int i = ultima_msg_lida; i < chat_mem->total_msgs; i++)
            {
                int idx = i % MAX_MSGS;
                Mensagem *msg = &chat_mem->mensagens[idx];

                printf("\n[%s]: %s\n> ", msg->usuario, msg->texto);
                fflush(stdout);
            }

            ultima_msg_lida = chat_mem->total_msgs;
        }

        sem_post(sem);
        usleep(100000);
    }
    return NULL;
}

void enviar_mensagem(const char *texto)
{
    sem_wait(sem);

    int idx = chat_mem->total_msgs % MAX_MSGS;

    strncpy(chat_mem->mensagens[idx].usuario, meu_usuario, MAX_USER_LEN - 1);
    strncpy(chat_mem->mensagens[idx].texto, texto, MAX_MSG_LEN - 1);
    chat_mem->mensagens[idx].id = chat_mem->total_msgs;

    chat_mem->total_msgs++;

    sem_post(sem);
}

void inicializar_chat()
{
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("Erro ao criar memória compartilhada");
        exit(1);
    }

    if (ftruncate(shm_fd, sizeof(ChatMemoria)) == -1)
    {
        perror("Erro ao dimensionar memória compartilhada");
        exit(1);
    }

    chat_mem = mmap(NULL, sizeof(ChatMemoria),
                    PROT_READ | PROT_WRITE,
                    MAP_SHARED,
                    shm_fd, 0);

    if (chat_mem == MAP_FAILED)
    {
        perror("Erro ao mapear memória compartilhada");
        exit(1);
    }

    sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED)
    {
        perror("Erro ao criar semáforo");
        exit(1);
    }

    close(shm_fd);
}

void limpar_chat()
{
    if (chat_mem != NULL)
    {
        munmap(chat_mem, sizeof(ChatMemoria));
    }

    if (sem != NULL)
    {
        sem_close(sem);
    }
}

int main()
{
    char buffer[MAX_MSG_LEN];
    pthread_t thread_monitor;

    printf("╔════════════════════════════════════════╗\n");
    printf("║  CHAT COM MEMÓRIA COMPARTILHADA       ║\n");
    printf("╚════════════════════════════════════════╝\n\n");

    printf("Digite seu nome de usuário: ");
    if (fgets(meu_usuario, MAX_USER_LEN, stdin) == NULL)
    {
        fprintf(stderr, "Erro ao ler nome de usuário\n");
        return 1;
    }
    meu_usuario[strcspn(meu_usuario, "\n")] = 0;

    if (strlen(meu_usuario) == 0)
    {
        fprintf(stderr, "Nome de usuário não pode ser vazio\n");
        return 1;
    }

    inicializar_chat();

    sem_wait(sem);
    ultima_msg_lida = chat_mem->total_msgs;
    sem_post(sem);

    printf("\n✓ Chat iniciado como '%s'\n", meu_usuario);
    printf("✓ Digite suas mensagens (ou 'sair' para encerrar)\n\n");

    if (pthread_create(&thread_monitor, NULL, monitorar_mensagens, NULL) != 0)
    {
        perror("Erro ao criar thread");
        limpar_chat();
        return 1;
    }

    while (1)
    {
        printf("> ");
        fflush(stdout);

        if (fgets(buffer, MAX_MSG_LEN, stdin) == NULL)
        {
            break;
        }

        buffer[strcspn(buffer, "\n")] = 0;

        if (strcmp(buffer, "sair") == 0)
        {
            break;
        }

        if (strlen(buffer) > 0)
        {
            enviar_mensagem(buffer);
        }
    }

    printf("\nEncerrando chat...\n");
    rodando = 0;
    pthread_join(thread_monitor, NULL);
    limpar_chat();

    printf("Chat encerrado!\n");

    return 0;
}