## Lista Avaliativa 2 - Respostas

#### 1) 

Um processo não pode passar diretamente do estado bloqueado para o estado de execução. A transição ocorre em duas etapas. Primeiro, o processo precisa sair do estado bloqueado e ir para o estado pronto. Isso acontece quando o evento externo pelo qual ele estava esperando finalmente ocorre. Por exemplo, a conclusão de uma operação de entrada/saída, a chegada de dados de um dispositivo ou a liberação de um recurso que estava sendo aguardado.

Uma vez no estado pronto, o processo está apto a executar, mas ainda aguarda a sua vez de usar a CPU. A segunda etapa ocorre quando o escalonador do sistema operacional seleciona este processo, dentre todos os que estão no estado pronto, para ser o próximo a receber o tempo de CPU. Somente após ser escolhido pelo escalonador é que o processo finalmente passa para o estado de execução.

#### 2) 

A entrada de um processo na tabela de processos armazena todo o estado e contexto necessários para que o sistema operacional possa gerenciar o processo e restaurar sua execução após uma interrupção ou suspensão. As principais informações, e seus motivos, são:

*   **Gerenciamento do Processo:**
    *   **Registradores e Contador de Programa:** Armazenam o conteúdo dos registradores da CPU no momento da interrupção. O motivo é permitir que o processo continue sua execução exatamente do ponto onde parou, como se nada tivesse acontecido.
    *   **Estado do Processo:** Indica se o processo está em execução, pronto ou bloqueado. Essa informação é crucial para que o escalonador saiba quais processos estão aptos a receber tempo de CPU.
    *   **Prioridade e Parâmetros de Escalonamento:** Utilizados pelo algoritmo de escalonamento para decidir qual processo deve ser executado em seguida.

*   **Gerenciamento de Memória:**
    *   **Ponteiros para os Segmentos de Memória:** Apontam para as áreas de memória que contêm o código, os dados e a pilha do processo. O motivo é permitir que o sistema localize e gerencie o espaço de endereçamento do processo.

*   **Gerenciamento de Arquivos:**
    *   **Descritores de Arquivo e Diretórios:** Mantêm informações sobre os arquivos que o processo abriu e seu diretório de trabalho atual. Isso é necessário para gerenciar os recursos de E/S que o processo está utilizando e controlar o acesso a eles.

#### 3) 

A diferença fundamental entre processos e threads está na forma como agrupam recursos e executam tarefas. Um processo é uma unidade de alocação de recursos, funcionando como um contêiner que agrupa um conjunto de recursos relacionados. Por outro lado, uma thread é a unidade de execução, representando um fluxo de controle que executa o código dentro do ambiente de um processo.

Os itens que são únicos por processo e, portanto, compartilhados por todas as threads dentro dele, são:
*   **Espaço de endereçamento:** Todas as threads compartilham a mesma visão da memória, incluindo o código e as variáveis globais.
*   **Arquivos abertos:** Se uma thread abre um arquivo, as outras threads do mesmo processo podem acessá-lo.
*   **Processos filhos e alarmes:** Recursos como estes pertencem ao processo como um todo.

Os itens que são únicos por thread, representando seu contexto de execução individual, são:
*   **Contador de Programa (PC):** Indica qual instrução a thread está executando.
*   **Registradores:** Armazenam as variáveis de trabalho atuais da thread.
*   **Pilha de Execução (Stack):** Contém o histórico de chamadas de função e as variáveis locais de cada thread.
*   **Estado:** Cada thread possui seu próprio estado (execução, pronto ou bloqueado).

#### 4) 

**Threads no Espaço do Usuário:**
Neste modelo, as threads são implementadas por uma biblioteca de tempo de execução no espaço de usuário, e o kernel do sistema operacional não tem conhecimento de sua existência. Para o kernel, o processo inteiro é uma única thread de execução.

*   **Vantagens:**
    *   **Rapidez:** A criação, destruição e o chaveamento de contexto entre threads são extremamente rápidos, pois não exigem uma chamada de sistema para o núcleo.
    *   **Portabilidade:** Um pacote de threads de usuário pode ser implementado em qualquer sistema operacional, mesmo naqueles que não oferecem suporte nativo a threads.
    *   **Customização:** Cada processo pode ter seu próprio algoritmo de escalonamento de threads, customizado para sua aplicação.

*   **Desvantagens:**
    *   **Bloqueio do Processo:** Se uma thread realiza uma chamada de sistema bloqueante (ex: leitura de disco), o processo inteiro é bloqueado pelo kernel, impedindo que outras threads do mesmo processo executem.
    *   **Falta de Paralelismo Real:** Como o kernel enxerga apenas uma thread por processo, ele não pode escalonar as threads do mesmo processo para executar em múltiplos núcleos de CPU simultaneamente.

**Threads no Kernel:**
Neste modelo, o kernel do sistema operacional é responsável por gerenciar todas as threads. Ele mantém uma tabela de threads para todo o sistema e realiza o escalonamento no nível do kernel.

*   **Vantagens:**
    *   **Não Bloqueio do Processo:** Se uma thread realiza uma chamada de sistema bloqueante, o kernel pode escalonar outra thread do mesmo processo para executar.
    *   **Paralelismo Real:** O kernel pode escalonar diferentes threads de um mesmo processo para executar simultaneamente em diferentes núcleos de CPU, permitindo paralelismo verdadeiro.

*   **Desvantagens:**
    *   **Lentidão:** A criação, destruição e o chaveamento de contexto são consideravelmente mais lentos, pois cada operação exige uma chamada de sistema e uma mudança de modo (usuário para núcleo).
    *   **Maior Sobrecarga:** As estruturas de dados para gerenciar cada thread são mantidas no núcleo, o que consome mais recursos do sistema.