#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define NUM_FILHOS 5

// Estrutura para enviar o PID e o número juntos pelo pipe
struct DadosFilho {
    pid_t pid;
    int numero;
};

int main() {
    int fd[2]; // Descritores de arquivo para o pipe: fd[0] para leitura, fd[1] para escrita
    pid_t pids[NUM_FILHOS];

    // Cria o pipe. É crucial criar antes do fork.
    if (pipe(fd) == -1) {
        perror("pipe falhou");
        exit(EXIT_FAILURE);
    }

    // --- Criação dos processos filhos ---
    for (int i = 0; i < NUM_FILHOS; i++) {
        pids[i] = fork();

        if (pids[i] < 0) {
            perror("fork falhou");
            exit(EXIT_FAILURE);
        }

        // --- Código do Processo Filho ---
        if (pids[i] == 0) {
            // O filho não vai ler do pipe, então fecha a ponta de leitura.
            close(fd[0]);

            // Inicializa a semente do gerador de números aleatórios.
            // Usar o PID garante uma semente diferente para cada filho.
            srand(time(NULL) ^ getpid());

            // Sorteia um número entre 0 e 200
            int numero_sorteado = rand() % 201;

            // Prepara os dados para envio
            struct DadosFilho dados;
            dados.pid = getpid();
            dados.numero = numero_sorteado;

            printf("Filho (PID %d) enviou o número %d.\n", dados.pid, dados.numero);

            // Escreve a struct no pipe
            write(fd[1], &dados, sizeof(struct DadosFilho));

            // Fecha a ponta de escrita após o uso.
            close(fd[1]);

            // O trabalho do filho terminou.
            exit(0);
        }
    }

    // --- Código do Processo Pai ---

    // O pai não vai escrever no pipe, então fecha a ponta de escrita.
    close(fd[1]);

    int menor_numero = 201; // Inicia com um valor maior que o máximo possível (200)
    pid_t pid_filho_menor = -1;

    printf("\nPai (PID %d) esperando para receber os números...\n", getpid());

    // O pai lê os dados de todos os 5 filhos
    for (int i = 0; i < NUM_FILHOS; i++) {
        struct DadosFilho dados_recebidos;

        // Lê uma struct do pipe. Esta chamada é bloqueante.
        read(fd[0], &dados_recebidos, sizeof(struct DadosFilho));

        printf("Pai recebeu %d do filho (PID %d).\n", dados_recebidos.numero, dados_recebidos.pid);

        // Verifica se o número recebido é o menor até agora
        if (dados_recebidos.numero < menor_numero) {
            menor_numero = dados_recebidos.numero;
            pid_filho_menor = dados_recebidos.pid;
        }
    }

    // Fecha a ponta de leitura do pipe.
    close(fd[0]);

    // Espera todos os filhos terminarem para evitar processos zumbis.
    for (int i = 0; i < NUM_FILHOS; i++) {
        wait(NULL);
    }

    // Imprime o resultado final
    printf("\n--- Resultado Final ---\n");
    printf("O menor número recebido foi: %d\n", menor_numero);
    printf("Enviado pelo filho com PID: %d\n", pid_filho_menor);

    return 0;
}