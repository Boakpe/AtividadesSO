## Respostas - Lista Avaliativa 3

### 1) Exclusão Mútua com TSL e Desabilitação de Interrupções

**TSL (Test and Set Lock):**

*Funcionamento:*
- Instrução atômica de hardware que lê um valor de memória e escreve 1 nela em uma única operação indivisível
- Processo tenta pegar o lock em um loop:
  ```
  TSL REGISTER, LOCK  ; copia LOCK para REGISTER e seta LOCK=1
  CMP REGISTER, #0     ; era zero?
  JNE loop            ; se não era zero, espera
  ```
- Para liberar: escreve 0 no LOCK

*Problema:*
- **Espera ocupada (busy waiting):** Desperdiça tempo de CPU
- Não é uma boa solução para sistemas com poucos processadores

**Desabilitação de Interrupções:**

*Funcionamento:*
- Antes de entrar na região crítica: desabilita interrupções
- Após sair: habilita interrupções
- Garante que o processo não será interrompido

*Problemas:*
- **Perigoso:** Processo pode travar e nunca reabilitar interrupções
- **Não funciona em multiprocessadores:** Outros cores podem acessar a região crítica
- **Privilégio:** Processos de usuário não deveriam ter esse poder
- **Pode ser útil apenas no kernel** para seções críticas muito curtas

**Conclusão:** Ambas têm limitações significativas para uso geral em sistemas modernos.

### 2) Mutexes e Variáveis de Condição (Pthreads)

**Mutexes:**

*Definição:*
- Mecanismo de exclusão mútua para proteger regiões críticas
- Pode estar **locked** ou **unlocked**

*Implementação em Pthreads:*
```c
pthread_mutex_t mutex;
pthread_mutex_init(&mutex, NULL);

pthread_mutex_lock(&mutex);    // Tenta adquirir o lock
// Região crítica
pthread_mutex_unlock(&mutex);  // Libera o lock

pthread_mutex_destroy(&mutex);
```

*Vantagem:*
- Se o mutex está ocupado, a thread é **bloqueada** (não desperdiça CPU)

**Variáveis de Condição:**

*Definição:*
- Usadas para sincronização baseada em condições
- Permitem que threads esperem até que uma condição seja satisfeita

*Funcionamento:*
```c
pthread_cond_t cond;
pthread_mutex_t mutex;

// Thread que espera
pthread_mutex_lock(&mutex);
while (!condicao)
    pthread_cond_wait(&cond, &mutex);  // Libera mutex e espera
// Condição satisfeita
pthread_mutex_unlock(&mutex);

// Thread que sinaliza
pthread_mutex_lock(&mutex);
condicao = true;
pthread_cond_signal(&cond);  // ou pthread_cond_broadcast
pthread_mutex_unlock(&mutex);
```

*Importante:*
- `pthread_cond_wait` atomicamente libera o mutex e bloqueia a thread
- Ao acordar, reaquire o mutex automaticamente

### 3) Escalonamento Round Robin

**Funcionamento:**
- Cada processo recebe um intervalo de tempo chamado **quantum** (time slice)
- Processos são organizados em uma fila circular
- Quando um processo usa todo seu quantum, é colocado no final da fila
- Próximo processo da fila é selecionado

**Exemplo:**

Processos: A (10ms), B (5ms), C (8ms), D (3ms)  
Quantum: 4ms

```
Tempo 0-4:   A executa (4ms) → volta para fila com 6ms restantes
Tempo 4-8:   B executa (4ms) → volta para fila com 1ms restante
Tempo 8-12:  C executa (4ms) → volta para fila com 4ms restantes
Tempo 12-15: D executa (3ms) → termina
Tempo 15-19: A executa (4ms) → volta para fila com 2ms restantes
Tempo 19-20: B executa (1ms) → termina
Tempo 20-24: C executa (4ms) → termina
Tempo 24-26: A executa (2ms) → termina
```

**Características:**
- **Justo:** Todos os processos recebem tempo de CPU
- **Preemptivo:** Processos são interrompidos após o quantum
- **Bom para sistemas interativos**
- Quantum pequeno: muitas trocas de contexto (overhead)
- Quantum grande: se aproxima de FIFO

### 4) Prioridades Dinâmicas com 1/f

**Tipo de processo priorizado:**
Processos **limitados por E/S (I/O-bound)** são priorizados.

**Explicação:**
- **f** = fração do quantum utilizada
- Processos I/O-bound usam pouco CPU (pequeno f) → prioridade alta (1/f grande)
- Processos CPU-bound usam todo quantum (f próximo de 1) → prioridade baixa

**Vantagem de priorizar processos I/O-bound:**
1. **Uso eficiente de recursos:** Enquanto um processo espera E/S, outro usa a CPU
2. **Melhor responsividade:** Processos interativos (editores, navegadores) respondem rapidamente
3. **Maior throughput do sistema:** Mantém dispositivos de E/S ocupados
4. **Melhor experiência do usuário:** Interface gráfica permanece fluida

**Exemplo:**
- Editor de texto (I/O-bound): usa 5% do quantum → prioridade = 1/0.05 = 20
- Compilação (CPU-bound): usa 100% do quantum → prioridade = 1/1.0 = 1