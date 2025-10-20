## Respostas - Lista Avaliativa 2

### 1) Transição de Bloqueado para Executando

Um processo **não pode** passar diretamente do estado bloqueado para o estado executando. A transição correta é:

**Bloqueado → Pronto → Executando**

**Condições necessárias:**
1. O evento pelo qual o processo estava esperando deve ocorrer (E/S completada, recurso disponibilizado, mensagem recebida, etc.)
2. O processo passa para o estado **Pronto** (ready)
3. O escalonador deve selecioná-lo para execução entre os processos prontos
4. Então o processo passa para **Executando** (running)

### 2) Informações na Tabela de Processos (PCB)

**Principais informações armazenadas:**

- **Estado dos registradores:** Necessário para restaurar o contexto do processo quando ele voltar a executar
- **Contador de programa (PC):** Indica qual instrução será executada a seguir
- **Ponteiro da pilha:** Localização do topo da pilha do processo
- **Estado do processo:** Executando, Pronto, Bloqueado, etc.
- **PID:** Identificador único do processo
- **PPID:** PID do processo pai
- **Prioridade:** Usado pelo escalonador
- **Informações de memória:** Tabela de páginas, segmentos, limites de memória
- **Recursos alocados:** Arquivos abertos, dispositivos, semáforos
- **Tempo de CPU:** Estatísticas de uso
- **Informações de escalonamento:** Quantum restante, fila a que pertence

### 3) Diferença entre Processos e Threads

**Processo:**
- Unidade de alocação de recursos
- Possui espaço de endereçamento próprio e isolado
- Criação e troca de contexto são operações pesadas

**Thread:**
- Unidade de execução dentro de um processo
- Compartilha recursos do processo
- Criação e troca de contexto são mais leves

**Únicos por processo:**
- Espaço de endereçamento (memória)
- Variáveis globais
- Arquivos abertos
- Processos filhos
- Sinais e tratadores
- Informações de contabilidade

**Únicos por thread:**
- Contador de programa (PC)
- Registradores
- Pilha (stack)
- Estado da thread
- Variáveis locais

### 4) Implementação de Threads: Kernel vs Espaço de Usuário

**Threads em Espaço de Usuário:**

*Características:*
- Implementadas por biblioteca (sem suporte do kernel)
- Kernel enxerga apenas um processo

*Vantagens:*
- Troca de contexto muito rápida (sem chamada de sistema)
- Podem ser implementadas em qualquer SO
- Cada processo pode ter seu próprio algoritmo de escalonamento

*Desvantagens:*
- Se uma thread bloqueia em E/S, todo o processo bloqueia
- Não aproveita múltiplos cores
- Necessita polling para evitar bloqueio completo

**Threads no Kernel:**

*Características:*
- Kernel gerencia e escalona as threads
- Cada thread tem seu próprio TCB no kernel

*Vantagens:*
- Quando uma thread bloqueia, outras podem continuar executando
- Aproveita múltiplos cores (paralelismo real)
- Melhor integração com o SO

**Desvantagens:**
- Troca de contexto mais lenta (envolve o kernel)
- Criação e destruição mais custosas
- Overhead no kernel