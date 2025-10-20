### **Lista Avaliativa 3 - Respostas**

#### 1)

A exclusão mútua pode ser obtida tanto por TSL quanto pela desabilitação de interrupções, mas ambas são consideradas soluções primitivas com desvantagens significativas.

**TSL (Test and Set Lock):**
A TSL é uma instrução de hardware que funciona de forma atômica (indivisível). Ela lê o valor de uma variável de trava (*lock*) na memória, armazena este valor em um registrador e, simultaneamente, escreve um valor "1" (indicando "travado") de volta na mesma posição de memória.

Para obter exclusão mútua, um processo, antes de entrar em sua região crítica, executa a TSL em um *loop*. Ele verifica o valor retornado pela instrução:
*   Se o valor retornado for "0" (destravado), o processo obteve a trava com sucesso (pois a TSL a alterou para "1") e pode entrar na região crítica.
*   Se o valor retornado for "1" (travado), o processo continua no *loop*, testando a trava repetidamente até que ela seja liberada.
Ao sair da região crítica, o processo simplesmente escreve "0" de volta na variável de trava.

**Desabilitação de Interrupções:**
Em um sistema com um único processador, a exclusão mútua pode ser alcançada desabilitando todas as interrupções de hardware imediatamente antes de entrar na região crítica e reabilitando-as ao sair. Como as trocas de contexto são geralmente disparadas por interrupções (como a do temporizador), desabilitá-las garante que o processo atual não será preemptado, podendo assim acessar recursos compartilhados sem o risco de interferência de outros processos.

**Avaliação e Desvantagens:**
Nenhuma das duas é uma boa solução para uso geral em aplicações modernas:

*   **Desvantagens da TSL:**
    *   **Espera Ocupada (*Busy-Waiting*):** A principal desvantagem é que o processo consome tempo de CPU em um *loop* inútil enquanto espera pela trava. Isso desperdiça recursos e é ineficiente, especialmente se a espera for longa.
    *   **Inversão de Prioridade:** Um processo de alta prioridade pode ficar preso em espera ocupada por uma trava mantida por um processo de baixa prioridade, que por sua vez pode nunca ser escalonado para liberar a trava.

*   **Desvantagens da Desabilitação de Interrupções:**
    *   **Perigoso para o Usuário:** Conceder a um processo de usuário o poder de desabilitar interrupções é extremamente arriscado. Um programa malicioso ou com erro poderia travar todo o sistema ao não reabilitar as interrupções. Por isso, essa técnica só é segura para uso dentro do próprio núcleo do sistema operacional.
    *   **Ineficaz em Sistemas Multiprocessadores:** Desabilitar as interrupções em uma CPU não impede que outra CPU acesse a mesma memória concorrentemente, tornando a solução inútil em hardware moderno.
    *   **Aumento da Latência:** Se as interrupções permanecerem desabilitadas por muito tempo, o sistema pode deixar de responder a eventos importantes, como a chegada de dados da rede ou do disco.

#### 2)

**Mutexes (Mutual Exclusion Locks):**
Um *mutex* é uma primitiva de sincronização de mais alto nível que protege o acesso a uma região crítica. Diferente da espera ocupada da TSL, quando um *thread* tenta adquirir um *mutex* que já está travado, o sistema operacional o coloca em estado de bloqueio. O *thread* é movido para uma fila de espera e não consome CPU. Quando o *thread* que detém o *mutex* o libera, o sistema operacional acorda um dos *threads* que estavam na fila, permitindo que ele adquira a trava e prossiga.

**Implementação em Pthreads:**
A biblioteca Pthreads oferece uma implementação padrão para *mutexes* com as seguintes funções principais:
*   `pthread_mutex_init()`: Inicializa uma variável do tipo *mutex*.
*   `pthread_mutex_lock()`: Tenta adquirir a trava. Se o *mutex* estiver livre, o *thread* o adquire e continua. Se estiver ocupado, o *thread* é bloqueado até que o *mutex* seja liberado.
*   `pthread_mutex_unlock()`: Libera o *mutex*, permitindo que outro *thread* que esteja esperando possa adquiri-lo.
*   `pthread_mutex_destroy()`: Destrói o *mutex* após o uso.

**Variáveis de Condição:**
*Mutexes* são excelentes para garantir a exclusão mútua, mas são insuficientes quando um *thread* precisa esperar por uma condição específica para poder continuar seu trabalho (ex: um consumidor esperando que um buffer deixe de estar vazio). Um *thread* não pode simplesmente manter o *mutex* travado e esperar em um *loop*, pois isso impediria que outros *threads* (como o produtor) alterassem a condição.

As variáveis de condição resolvem esse problema, trabalhando em conjunto com os *mutexes*. Elas permitem que um *thread*:
1.  Trave o *mutex* para verificar uma condição de forma segura.
2.  Se a condição não for satisfeita, ele chama `pthread_cond_wait()`. Essa função, de forma atômica, libera o *mutex* e coloca o *thread* para dormir.
3.  Outro *thread* (ex: o produtor) pode então adquirir o *mutex*, alterar a condição (ex: adicionar um item ao buffer) e chamar `pthread_cond_signal()` ou `pthread_cond_broadcast()` para acordar um ou todos os *threads* que estavam esperando naquela variável de condição.
4.  O *thread* acordado, ao retornar de `pthread_cond_wait()`, automaticamente readquire o *mutex* e pode reavaliar a condição para continuar sua execução.

#### 3)

O Round Robin é um dos algoritmos de escalonamento mais simples e justos, projetado especialmente para sistemas de tempo compartilhado. Ele é um algoritmo preemptivo. Seu funcionamento baseia-se nos seguintes passos:

1.  O sistema operacional mantém uma fila circular de todos os processos no estado "pronto".
2.  A cada processo é alocada uma pequena fatia de tempo fixa, chamada de *quantum*.
3.  O escalonador seleciona o primeiro processo da fila e o despacha para a CPU.
4.  O processo executa até que uma de duas coisas aconteça:
    *   Ele termina ou bloqueia (ex: aguardando uma operação de E/S) antes que o *quantum* expire. Neste caso, o escalonador imediatamente seleciona o próximo processo da fila.
    *   O *quantum* de tempo expira. O sistema operacional é notificado por uma interrupção de temporizador, preempta (interrompe forçadamente) o processo, move-o para o final da fila de "prontos" e despacha o próximo processo da fila.

**Exemplo:**
Considere três processos (P1, P2, P3) que chegam ao mesmo tempo com os seguintes tempos de execução (em milissegundos):
*   P1: 20 ms
*   P2: 5 ms
*   P3: 8 ms

Vamos assumir um quantum de 5 ms. A execução ocorreria da seguinte forma:

| **Processo** | P1 | P2 | P3 | P1 | P3 | P1 | P1 |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **Tempo (ms)** | 0-5 | 5-10 | 10-15 | 15-20 | 20-23 | 23-28 | 28-33 |

*   **0-5 ms:** P1 executa por 5 ms e é preemptado (ainda restam 15 ms). A fila de prontos agora é [P2, P3, P1].
*   **5-10 ms:** P2 executa por 5 ms e termina. A fila de prontos é [P3, P1].
*   **10-15 ms:** P3 executa por 5 ms e é preemptado (ainda restam 3 ms). A fila é [P1, P3].
*   **15-20 ms:** P1 executa por mais 5 ms (agora restam 10 ms). A fila é [P3, P1].
*   **20-23 ms:** P3 executa por 3 ms e termina. A fila é [P1].
*   **23-33 ms:** P1 executa em dois *quanta* seguidos (e mais um parcial) até terminar.

A grande vantagem do Round Robin é que ele oferece tempos de resposta razoáveis para processos interativos, pois nenhum processo precisa esperar mais do que `(n-1) * quantum` para receber sua próxima fatia de tempo.

#### 4)

Este algoritmo de escalonamento prioriza os processos limitados por E/S ou processos interativos.

A lógica é a seguinte:
*   Um processo limitado por E/S executa por um curto período de tempo antes de bloquear para realizar uma operação de E/S (como ler um dado do disco ou da rede). Portanto, ele utiliza apenas uma pequena fração `f` do seu *quantum*. Como `f` é pequeno, o valor de `1/f` será grande, resultando em uma alta prioridade.
*   Um processo limitado por CPU tende a usar todo o seu *quantum* de tempo em cálculos. Neste caso, `f` será próximo de 1, e o valor de `1/f` será baixo, resultando em uma baixa prioridade.

**Vantagem de Priorizar Processos Limitados por E/S:**
A principal vantagem de dar alta prioridade a esses processos é a melhora da responsividade e da utilização geral do sistema.

1.  **Melhor Responsividade:** Processos interativos (como editores de texto, navegadores web) são tipicamente limitados por E/S, pois passam a maior parte do tempo esperando por uma entrada do usuário (teclado, mouse). Ao dar-lhes alta prioridade, o sistema garante que eles sejam executados imediatamente assim que recebem uma entrada, proporcionando uma experiência de usuário fluida e sem atrasos perceptíveis.

2.  **Maior Utilização dos Recursos:** Um processo limitado por E/S precisa de um curto surto de CPU para processar dados e rapidamente iniciar a próxima operação de E/S. Ao escaloná-lo rapidamente, o sistema permite que ele mantenha os dispositivos de E/S (que são lentos) ocupados. Enquanto o dispositivo de E/S está trabalhando, a CPU pode ser alocada a outros processos (como os *CPU-bound*), maximizando o paralelismo entre a CPU e os dispositivos de E/S e, consequentemente, a vazão (*throughput*) do sistema.