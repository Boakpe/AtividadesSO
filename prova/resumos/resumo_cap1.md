### **Resumo Detalhado: Introdução aos Sistemas Operacionais (Capítulo 1)**

#### **1. O Que é um Sistema Operacional?**

Um sistema operacional (SO) é um software fundamental que atua como uma camada intermediária entre o hardware do computador e os programas do usuário. Ele possui duas funções essenciais e complementares:

##### **1.1. O Sistema Operacional como Máquina Estendida (Visão *Top-Down*)**

Nesta visão, a principal função do SO é fornecer **abstrações** aos programas do usuário. O hardware de um computador é complexo, primitivo e difícil de programar (ex: controlar os detalhes de um disco SATA, gerenciar interrupções, etc.). O SO esconde essa complexidade e apresenta ao programador um modelo de computador mais simples, limpo e fácil de usar.

*   **Abstração:** É a chave para gerenciar a complexidade. O SO transforma recursos de hardware complexos em conceitos simples e poderosos.
    *   **Exemplo:** Em vez de lidar com trilhas, setores e cabeças de um disco rígido, o programador trabalha com a abstração de **arquivos**, que podem ser criados, lidos e escritos de forma simples.
*   **Interface:** O SO fornece uma interface de programação (API, por meio de chamadas de sistema) que permite que os aplicativos usem essas abstrações sem conhecer os detalhes do hardware.
*   **Metáfora:** "Sistemas operacionais transformam o hardware feio em abstrações belas."

##### **1.2. O Sistema Operacional como Gerenciador de Recursos (Visão *Bottom-Up*)**

Nesta visão, a função do SO é gerenciar de forma ordenada e controlada todos os recursos do hardware, como processadores, memória, discos, teclado, monitor, etc.

*   **Gerenciamento:** O SO aloca recursos para os diversos programas que competem por eles, resolve conflitos e otimiza o uso do sistema.
*   **Multiplexação de Recursos:** O compartilhamento de recursos é feito de duas formas:
    *   **Multiplexação no Tempo:** Diferentes programas se revezam para usar um mesmo recurso. Ex: A CPU é compartilhada entre vários processos, cada um executando por uma pequena fração de tempo (*timesharing*).
    *   **Multiplexação no Espaço:** Cada programa recebe uma parte de um recurso para usar simultaneamente com outros. Ex: A memória principal é dividida entre vários programas residentes ao mesmo tempo.

---

#### **2. História dos Sistemas Operacionais**

A evolução dos SOs está intimamente ligada à evolução da arquitetura dos computadores.

*   **Primeira Geração (1945-1955): Válvulas e Painéis de Conexão**
    *   **Hardware:** Válvulas, relés. Computadores enormes e pouco confiáveis.
    *   **Operação:** Sem SO. Programação feita diretamente em linguagem de máquina absoluta, conectando fios em painéis. Apenas um programador usava a máquina por vez.
*   **Segunda Geração (1955-1965): Transistores e Sistemas em Lote (Batch)**
    *   **Hardware:** Transistores, computadores de grande porte (*mainframes*).
    *   **Operação:** Surgem os **sistemas em lote (*batch*)** para otimizar o uso da CPU cara. Tarefas (jobs) eram reunidas em fitas magnéticas e executadas em sequência pelo computador principal (ex: IBM 7094), que lia a fita de entrada e escrevia em uma fita de saída. Um computador menor e mais barato (ex: IBM 1401) cuidava da leitura de cartões para a fita e da impressão da fita de saída (*off-line*).
    *   **Primeiros SOs:** Eram basicamente monitores que controlavam a sequência de tarefas (ex: FMS - Fortran Monitor System).
*   **Terceira Geração (1965-1980): Circuitos Integrados e Multiprogramação**
    *   **Hardware:** Circuitos Integrados (CIs), "família" de computadores compatíveis (ex: IBM System/360).
    *   **Conceitos Chave:**
        *   **Multiprogramação:** A memória principal é dividida em partições, cada uma contendo uma tarefa. Quando uma tarefa precisa esperar por uma operação de E/S (Entrada/Saída), a CPU passa a executar outra tarefa que está pronta, mantendo a CPU ocupada e aumentando a eficiência.
        *   **Spooling (Simultaneous Peripheral Operation On-Line):** Uso do disco como um grande buffer para ler tarefas e armazenar saídas de impressão, permitindo que a E/S ocorra em paralelo com a computação.
        *   **Timesharing (Compartilhamento de Tempo):** Variante da multiprogramação que permite que múltiplos usuários interajam com o sistema simultaneamente através de terminais. A CPU alterna rapidamente entre os usuários, dando a cada um a impressão de que tem o computador só para si. (Ex: CTSS, MULTICS).
*   **Quarta Geração (1980-Presente): Computadores Pessoais**
    *   **Hardware:** Circuitos Integrados em Larga Escala (LSI), microprocessadores.
    *   **SOs Dominantes:** CP/M, MS-DOS, Windows, e o surgimento das Interfaces Gráficas de Usuário (GUI), popularizadas pelo Apple Macintosh e depois pelo Microsoft Windows. UNIX e seus derivados (Linux, BSD) ganham força.
*   **Quinta Geração (1990-Presente): Computadores Móveis**
    *   **Hardware:** Smartphones, tablets.
    *   **SOs:** Domínio inicial do Symbian OS, seguido pelo crescimento do iOS e a dominância atual do Android.

---

#### **3. Conceitos de Hardware para Sistemas Operacionais**

*   **Processadores (CPU)**
    *   **Ciclo Básico:** Buscar instrução -> Decodificar -> Executar.
    *   **Registradores:** Memória interna de alta velocidade na CPU. Incluem:
        *   **Contador de Programa (PC):** Endereço da próxima instrução.
        *   **Ponteiro de Pilha (Stack Pointer):** Aponta para o topo da pilha de memória.
        *   **PSW (Program Status Word):** Contém bits de controle, incluindo o bit que define o modo de operação.
    *   **Modo Núcleo (Supervisor) vs. Modo Usuário:**
        *   **Modo Núcleo:** O SO roda neste modo. Tem acesso total a todo o hardware e a todas as instruções da máquina.
        *   **Modo Usuário:** Programas de aplicação rodam neste modo. O acesso a instruções privilegiadas (como E/S direta ou manipulação do mapa de memória) é proibido.
        *   **Troca de Modo:** Um programa em modo usuário que precisa de um serviço do SO (ex: ler um arquivo) executa uma instrução especial de **trap**, que muda a CPU para o modo núcleo e transfere o controle para o SO. Essa trap é o mecanismo por trás das **chamadas de sistema**.
    *   **Multithreading e Multinúcleo:** Chips modernos possuem múltiplos núcleos (CPUs completas no mesmo chip) e/ou *multithreading* (permite que um único núcleo mantenha o estado de múltiplos threads e alterne entre eles em nanossegundos) para aumentar o paralelismo.

*   **Memória**
    *   **Hierarquia de Memória:** Uma estrutura em camadas para equilibrar velocidade, custo e tamanho.
        1.  **Registradores** (dentro da CPU, mais rápidos)
        2.  **Cache L1, L2, L3** (memória estática rápida, próxima à CPU)
        3.  **Memória Principal (RAM)** (volátil, tamanho médio)
        4.  **Disco Magnético / SSD** (não volátil, grande, mais lento)
    *   **MMU (Memory Management Unit):** Componente de hardware que traduz endereços virtuais (gerados pelos programas) em endereços físicos (na RAM).

*   **Dispositivos de E/S (I/O)**
    *   **Estrutura:** Consistem em um **controlador** (chip que comanda o dispositivo) e o **dispositivo** físico.
    *   **Driver de Dispositivo:** Software, geralmente parte do SO, que entende o controlador e fornece uma interface simplificada para o resto do sistema.
    *   **Mecanismos de E/S:**
        1.  **Espera Ocupada (Polling):** A CPU fica em um loop, perguntando repetidamente ao dispositivo se ele já terminou a operação. Desperdiça tempo de CPU.
        2.  **Interrupção:** O driver inicia a E/S e bloqueia. Quando o dispositivo termina, ele envia um sinal de **interrupção** para a CPU. A CPU para o que está fazendo, salva seu estado e executa uma rotina do SO para tratar o evento. É o método mais comum e eficiente.
        3.  **DMA (Direct Memory Access):** Para transferências de grandes blocos de dados, a CPU instrui um controlador DMA com a origem, destino e tamanho dos dados. O DMA transfere os dados diretamente entre o dispositivo e a memória, sem envolver a CPU, que fica livre para outras tarefas. O DMA gera uma interrupção apenas no final da transferência.

*   **Inicialização (Booting)**
    *   O processo começa com o **BIOS** (Basic Input Output System) ou **UEFI**, um firmware na placa-mãe.
    *   Ele realiza um autoteste (POST), detecta dispositivos e, em seguida, carrega um programa inicializador (*bootloader*) de um dispositivo de inicialização (HD, SSD, etc.).
    *   O *bootloader* finalmente carrega o núcleo do sistema operacional na memória e transfere o controle para ele.

---

#### **4. Conceitos Fundamentais de Sistemas Operacionais (Abstrações)**

##### **4.1. Processos**
*   **Definição:** Um programa em execução. É um contêiner para todas as informações necessárias para executar um programa.
*   **Componentes:** Inclui o código do programa, os dados, a pilha, o contador de programa, registradores e outros recursos.
*   **Tabela de Processos:** Uma estrutura de dados mantida pelo SO que armazena informações sobre cada processo ativo no sistema.
*   **Criação/Término:** O SO fornece chamadas de sistema para criar novos processos (ex: `fork` em UNIX) e para terminá-los (ex: `exit`).
*   **UID/GID:** Todo processo tem um Identificador de Usuário (UID) e um Identificador de Grupo (GID) para fins de permissão e segurança.

##### **4.2. Espaços de Endereçamento e Memória Virtual**
*   **Espaço de Endereçamento:** É o conjunto de endereços de memória que um processo pode usar.
*   **Memória Virtual:** É uma abstração fundamental onde o SO faz com que cada processo tenha seu próprio espaço de endereçamento privado, geralmente começando em 0. O SO, com a ajuda da MMU, mapeia esses endereços virtuais para endereços físicos na RAM. Isso permite que um programa seja maior que a memória física disponível, mantendo apenas as partes necessárias na RAM e o restante no disco.

##### **4.3. Arquivos**
*   **Definição:** Uma abstração que permite armazenar informações em disco de forma nomeada e lógica.
*   **Diretórios (Pastas):** Uma forma de organizar arquivos hierarquicamente.
*   **Caminho (Path):** Uma string que especifica a localização de um arquivo na hierarquia de diretórios (ex: `/home/user/documento.txt`).
*   **Montagem (Mounting):** Processo de integrar o sistema de arquivos de um dispositivo (ex: pendrive) em um diretório da hierarquia principal.
*   **Arquivos Especiais:** Em sistemas como o UNIX, dispositivos de E/S são representados como arquivos no diretório `/dev`, permitindo que sejam lidos e escritos com as mesmas chamadas de sistema de arquivos comuns.

##### **4.4. Proteção**
*   O SO deve proteger os usuários e seus processos uns dos outros.
*   **Exemplo (UNIX):** A proteção de arquivos é baseada em um código de 9 bits associado a cada arquivo. Esses bits definem permissões de **leitura (r)**, **escrita (w)** e **execução (x)** para três categorias de usuários: o **dono** do arquivo, os membros do **grupo** do arquivo, e **outros** (todos os demais).

##### **4.5. O Interpretador de Comandos (Shell)**
*   O shell é a interface do usuário com o SO. Ele **não é parte do núcleo**; é um programa de usuário comum.
*   Ele lê comandos do usuário e os executa criando novos processos e usando chamadas de sistema.
*   Funções comuns: redirecionamento de E/S (`> file`), pipes (`|`), execução em segundo plano (`&`).

---

#### **5. Chamadas de Sistema (System Calls)**

As chamadas de sistema são a interface que os programas de usuário utilizam para solicitar serviços do núcleo do SO.

*   **Mecanismo:**
    1.  O programa de usuário empilha os parâmetros da chamada.
    2.  Chama uma rotina de biblioteca (ex: `read`).
    3.  A rotina de biblioteca coloca o número da chamada de sistema em um registrador.
    4.  A rotina executa uma instrução `TRAP`, que passa a CPU para o modo núcleo.
    5.  O núcleo usa o número no registrador para encontrar e executar a rotina correspondente na sua tabela de chamadas.
    6.  Após a conclusão, o controle é devolvido à rotina de biblioteca.
    7.  A rotina de biblioteca retorna ao programa de usuário.
*   **Exemplos POSIX (UNIX-like):**
    *   **Processos:** `fork()`, `waitpid()`, `execve()`, `exit()`
    *   **Arquivos:** `open()`, `close()`, `read()`, `write()`, `lseek()`
    *   **Diretórios:** `mkdir()`, `rmdir()`, `link()`, `unlink()`, `mount()`

---

#### **6. Estrutura de Sistemas Operacionais**

Existem diferentes filosofias de design para estruturar o SO internamente.

*   **Sistemas Monolíticos:**
    *   **Estrutura:** Não há uma estrutura real. O SO é uma coleção de procedimentos, cada um podendo chamar qualquer outro. Todo o SO é compilado em um único programa binário que roda em modo núcleo.
    *   **Vantagem:** Alto desempenho devido à comunicação direta entre componentes.
    *   **Desvantagem:** Difícil de manter e depurar ("bola de lama"). Uma falha em um driver pode derrubar todo o sistema. (Ex: MS-DOS, versões iniciais do UNIX).

*   **Sistemas em Camadas:**
    *   **Estrutura:** O SO é organizado em uma hierarquia de camadas. Cada camada usa apenas as funções da camada imediatamente inferior.
    *   **Vantagem:** Design mais limpo e modular, facilitando a depuração.
    *   **Desvantagem:** Potencialmente menos eficiente, pois uma requisição pode precisar atravessar várias camadas. (Ex: Sistema THE).

*   **Micronúcleos (Microkernels):**
    *   **Estrutura:** A filosofia é mover o máximo possível de funcionalidades para fora do núcleo, deixando-o o menor possível. Funções como drivers de dispositivo e sistemas de arquivos rodam como processos de usuário (servidores).
    *   **Núcleo Mínimo:** O micronúcleo lida apenas com comunicação entre processos (IPC), gerenciamento básico de memória e escalonamento.
    *   **Vantagem:** Mais robusto e seguro. Uma falha em um driver (que está em modo usuário) não derruba o sistema, apenas o serviço correspondente.
    *   **Desvantagem:** Desempenho inferior devido à sobrecarga da comunicação entre processos cliente e servidor. (Ex: MINIX 3, QNX).

*   **Máquinas Virtuais:**
    *   **Estrutura:** Um software chamado **hipervisor** (ou Monitor de Máquina Virtual - VMM) cria abstrações da máquina física, permitindo que múltiplos sistemas operacionais (convidados) rodem em um único hardware físico.
    *   **Tipo 1 (Nativo):** O hipervisor roda diretamente no hardware.
    *   **Tipo 2 (Hospedado):** O hipervisor roda como um programa sobre um SO hospedeiro.

*   **Modelo Cliente-Servidor:**
    *   Uma variação do modelo de micronúcleo, onde os serviços são encapsulados em processos servidores e os programas de usuário são os clientes. A comunicação é feita por troca de mensagens. Este modelo se estende naturalmente para sistemas distribuídos em uma rede.