## Respostas - Sistemas Operacionais Modernos

### 1) Sistema Operacional - Duas Visões

**Máquina Estendida:**
O sistema operacional atua como uma camada de abstração sobre o hardware, ocultando sua complexidade e fornecendo uma interface mais simples e amigável. Ele transforma o hardware "bruto" em uma máquina virtual mais fácil de programar, oferecendo chamadas de sistema padronizadas para operações como leitura/escrita de arquivos, criação de processos e comunicação de rede, sem que o programador precise conhecer detalhes de baixo nível do hardware.

**Gerenciador de Recursos:**
O SO gerencia e coordena o uso dos recursos de hardware (CPU, memória, dispositivos de E/S, arquivos) entre múltiplos programas e usuários. Ele decide quem usa qual recurso, quando, por quanto tempo e resolve conflitos quando há competição por recursos. O objetivo é garantir uso eficiente, justo e seguro dos recursos do sistema.

### 2) Modo Supervisor e Modo Usuário

**Modo Usuário:**
- Modo restrito onde processos de aplicação executam
- Acesso limitado às instruções e recursos do sistema
- Não pode executar instruções privilegiadas nem acessar diretamente hardware

**Modo Supervisor (Kernel):**
- Modo privilegiado com acesso total ao hardware
- Pode executar qualquer instrução e acessar qualquer endereço de memória
- Sistema operacional executa neste modo

**Momentos de Troca:**
- **Usuário → Supervisor:** Chamadas de sistema, interrupções de hardware, exceções (erros)
- **Supervisor → Usuário:** Retorno de chamadas de sistema, retorno de tratamento de interrupções

### 3) Sistema de Proteção de Arquivos (9 bits)

Sistema usado em Unix/Linux onde cada arquivo possui 9 bits de permissão organizados em três grupos:

**Estrutura (rwx rwx rwx):**
- **Primeiros 3 bits:** Permissões do proprietário (owner)
- **Segundos 3 bits:** Permissões do grupo (group)
- **Últimos 3 bits:** Permissões para outros usuários (others)

**Cada conjunto de 3 bits representa:**
- **r (read):** Permissão de leitura
- **w (write):** Permissão de escrita
- **x (execute):** Permissão de execução

Exemplo: `rwxr-xr--` = proprietário tem acesso total, grupo pode ler e executar, outros só podem ler.

### 4) Processo

**Definição:**
Um processo é um programa em execução, incluindo o código do programa, seus dados, pilha, registradores, contador de programa e todas as informações necessárias para executar o programa.

**Armazenamento quando não está em execução:**
As informações são armazenadas no **Bloco de Controle do Processo (PCB - Process Control Block)**, que contém:
- Estado dos registradores
- Contador de programa
- Ponteiros de pilha
- Estado do processo
- Informações de gerenciamento de memória
- Informações de E/S
- Prioridade e estatísticas

O PCB fica na memória do kernel, permitindo que o SO restaure completamente o contexto do processo quando ele voltar a executar.

### 5) Espaço de Endereçamento e Memória Virtual

**Espaço de Endereçamento:**
Conjunto de endereços de memória que um processo pode usar. É uma abstração que dá a cada processo a ilusão de ter toda a memória disponível só para si, tipicamente indo de 0 até um valor máximo determinado pela arquitetura.

**Memória Virtual:**
Técnica que permite:
- Usar mais memória do que a RAM física disponível
- Cada processo ter seu próprio espaço de endereçamento isolado
- Páginas de memória podem estar na RAM ou em disco (swap)
- Sistema usa tabelas de páginas para mapear endereços virtuais em endereços físicos
- Implementa proteção de memória entre processos
- Permite compartilhamento controlado de memória

### 6) Micronúcleo (Microkernel)

**Estrutura:**
Arquitetura onde o kernel contém apenas funcionalidades mínimas essenciais:
- Gerenciamento básico de processos e threads
- Comunicação entre processos (IPC)
- Gerenciamento básico de memória
- Escalonamento de baixo nível

**Características:**
- Serviços tradicionais do kernel (drivers, sistemas de arquivos, rede) executam como **processos em modo usuário**
- Comunicação entre componentes via **passagem de mensagens**
- Kernel menor e mais simples
- **Vantagens:** Maior modularidade, confiabilidade, segurança, facilidade de manutenção
- **Desvantagens:** Overhead de comunicação pode impactar desempenho
- **Exemplos:** Minix, QNX, L4