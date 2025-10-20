## Respostas - Lista Avaliativa 5

### 1) Algoritmo de Substituição Ótimo - Viabilidade de Análise Prévia

**Resposta: NÃO, não é viável na prática.**

**Por quê:**

1. **Problema da indecidibilidade:**
   - Prever o comportamento futuro de um programa é equivalente ao problema da parada (Halting Problem)
   - É impossível determinar com certeza quando cada página será usada no futuro
   
2. **Dependência de entrada:**
   - Comportamento do programa pode depender de entradas do usuário
   - Dados lidos de arquivos ou rede
   - Eventos externos imprevisíveis

3. **Complexidade computacional:**
   - Análise estática completa de código é computacionalmente proibitiva
   - Programas com loops, condicionais e ponteiros tornam análise muito complexa

4. **Natureza dinâmica:**
   - Muitos programas têm comportamento que só pode ser determinado em tempo de execução
   - Recursão, alocação dinâmica, estruturas de dados complexas

**O algoritmo ótimo serve apenas como:**
- Referência teórica para comparar desempenho de outros algoritmos
- Benchmark em simulações onde se conhece toda a sequência de acessos

**Alternativas práticas:**
- LRU (Least Recently Used)
- Clock/Second Chance
- NRU (Not Recently Used)
- Working Set

### 2) Algoritmo NRU (Not Recently Used)

**Funcionamento:**

O algoritmo classifica páginas em 4 categorias baseadas em 2 bits:
- **R (Referenced):** Página foi acessada
- **M (Modified/Dirty):** Página foi modificada

**Categorias (ordem de preferência para remoção):**
1. **Classe 0:** R=0, M=0 (não referenciada, não modificada) - **Melhor candidata**
2. **Classe 1:** R=0, M=1 (não referenciada, modificada)
3. **Classe 2:** R=1, M=0 (referenciada, não modificada)
4. **Classe 3:** R=1, M=1 (referenciada, modificada) - **Pior candidata**

**Algoritmo:**
1. A cada clock interrupt (periodicamente), SO zera todos os bits R
2. Bits M nunca são zerados pelo relógio (só ao escrever no disco)
3. Quando ocorre page fault, SO examina todas as páginas
4. Remove página da **classe mais baixa não vazia**
5. Dentro da mesma classe, escolha é aleatória

**Exemplo:**

Estado das páginas na memória:
```
Página  |  R  |  M  | Classe
--------|-----|-----|--------
   0    |  1  |  0  |   2
   1    |  0  |  0  |   0    ← Candidata (classe mais baixa)
   2    |  1  |  1  |   3
   3    |  0  |  0  |   0    ← Candidata (classe mais baixa)
   4    |  0  |  1  |   1
   5    |  1  |  0  |   2
```

**Decisão:** Remove página 1 ou 3 (ambas classe 0), escolhida aleatoriamente.

**Vantagens:**
- Fácil de implementar
- Eficiente (baixo overhead)
- Considera tanto uso recente quanto modificação
- Evita escrever páginas não modificadas no disco

### 3) Algoritmo LRU com Matriz de n×n bits

**Matriz LRU:** n×n bits onde n=4

**Regra de atualização:**
- Quando página k é referenciada:
  1. Seta todos os bits da linha k para 1
  2. Zera todos os bits da coluna k

**Sequência de referência: 3, 3, 2, 1, 2, 3, 0**

**Estado inicial (todas não referenciadas):**
```
     0  1  2  3
   +-----------
 0 | 0  0  0  0  (valor binário: 0000 = 0)
 1 | 0  0  0  0  (valor binário: 0000 = 0)
 2 | 0  0  0  0  (valor binário: 0000 = 0)
 3 | 0  0  0  0  (valor binário: 0000 = 0)
```

**Após referência 3:**
```
     0  1  2  3
   +-----------
 0 | 0  0  0  0  (valor: 0)
 1 | 0  0  0  0  (valor: 0)
 2 | 0  0  0  0  (valor: 0)
 3 | 1  1  1  0  (valor: 7)
```

**Após segunda referência 3:**
```
     0  1  2  3
   +-----------
 0 | 0  0  0  0  (valor: 0)
 1 | 0  0  0  0  (valor: 0)
 2 | 0  0  0  0  (valor: 0)
 3 | 1  1  1  0  (valor: 7)
```

**Após referência 2:**
```
     0  1  2  3
   +-----------
 0 | 0  0  0  0  (valor: 0)
 1 | 0  0  0  0  (valor: 0)
 2 | 1  1  0  1  (valor: 11)
 3 | 1  1  0  0  (valor: 6)
```

**Após referência 1:**
```
     0  1  2  3
   +-----------
 0 | 0  0  0  0  (valor: 0)
 1 | 1  0  1  1  (valor: 13)
 2 | 1  0  0  1  (valor: 9)
 3 | 1  0  0  0  (valor: 4)
```

**Após segunda referência 2:**
```
     0  1  2  3
   +-----------
 0 | 0  0  0  0  (valor: 0)
 1 | 1  0  0  1  (valor: 9)
 2 | 1  1  0  1  (valor: 13)
 3 | 1  0  0  0  (valor: 4)
```

**Após segunda referência 3:**
```
     0  1  2  3
   +-----------
 0 | 0  0  0  0  (valor: 0)
 1 | 1  0  0  0  (valor: 4)
 2 | 1  1  0  0  (valor: 6)
 3 | 1  1  1  0  (valor: 14)
```

**Após referência 0:**
```
     0  1  2  3
   +-----------
 0 | 0  1  1  1  (valor: 7)
 1 | 0  0  0  0  (valor: 0)
 2 | 0  1  0  0  (valor: 2)
 3 | 0  1  1  0  (valor: 6)
```

**Página para substituição:** Página **1** (menor valor = 0 = menos recentemente usada)

### 4) Algoritmo PFF (Page Fault Frequency)

**Objetivo:**
Controlar quantos frames cada processo tem na memória, ajustando dinamicamente o conjunto de trabalho (working set).

**Funcionamento:**

**Parâmetros:**
- **Limiar superior (U):** Taxa máxima aceitável de page faults
- **Limiar inferior (L):** Taxa mínima de page faults
- **τ (tau):** Intervalo de tempo entre page faults

**Regras:**

1. **Quando ocorre page fault:**
   - Mede tempo desde último page fault (τ)
   
2. **Se τ < L (page faults muito frequentes):**
   - Processo tem **poucos frames**
   - **Ação:** Aloca mais frames para o processo
   - Se não há frames livres: pode suspender outro processo (swap out)

3. **Se τ > U (page faults muito raros):**
   - Processo tem **frames demais**
   - **Ação:** Remove frames do processo (libera memória)
   - Páginas removidas podem ser dadas a outros processos

**Exemplo:**

Processo A:
```
Tempo  | Evento      | τ desde último PF | Ação
-------|-------------|-------------------|------------------
0.0s   | Page fault  | -                 | Inicial
0.1s   | Page fault  | 0.1s              | τ < L → Aloca +2 frames
0.5s   | Page fault  | 0.4s              | τ normal → Mantém
2.0s   | Page fault  | 1.5s              | τ > U → Remove 1 frame
5.0s   | Page fault  | 3.0s              | τ >> U → Remove 2 frames
5.05s  | Page fault  | 0.05s             | τ < L → Aloca +3 frames
```

**Vantagens:**
- Adaptação dinâmica à necessidade de cada processo
- Previne thrashing (paginação excessiva)
- Uso eficiente da memória
- Simples de implementar

**Resultado:**
Sistema mantém cada processo com quantidade adequada de memória, balanceando desempenho e uso de recursos.

### 5) Tradução de Endereço MULTICS

**MULTICS usa segmentação com paginação.**

**Estrutura do endereço virtual:**
```
[Número do segmento] [Número da página dentro do segmento] [Offset dentro da página]
      (18 bits)                    (6 bits)                        (10 bits)
```

**Processo de tradução:**

1. **Número do segmento** indexa a **Tabela de Descritores de Segmento**

2. **Descritor de Segmento contém:**
   - Endereço base da **Tabela de Páginas** desse segmento
   - Tamanho do segmento (para validação)
   - Bits de proteção (read, write, execute)
   - Bit de presente (segmento está na memória?)
   - Nível de privilégio necessário

3. **Número da página** indexa a **Tabela de Páginas do Segmento**

4. **Entrada da Tabela de Páginas contém:**
   - Número do frame físico
   - Bit de presente
   - Bits de proteção
   - Bits de uso e modificação

5. **Offset** é concatenado ao número do frame para formar **endereço físico**

**Exemplo:**
```
Endereço virtual: Segmento 5, Página 3, Offset 0x124

1. Acessa Tabela de Descritores[5]
   → Encontra ponteiro para Tabela de Páginas do segmento 5
   → Verifica proteção e presença

2. Acessa Tabela de Páginas do Seg5[3]
   → Encontra frame físico: 0x8A
   → Verifica presença

3. Monta endereço físico:
   → Frame: 0x8A, Offset: 0x124
   → Endereço físico: 0x8A124
```

**Vantagens:**
- **Proteção:** Cada segmento pode ter permissões diferentes
- **Compartilhamento:** Segmentos podem ser compartilhados entre processos
- **Flexibilidade:** Segmentos de tamanhos variáveis
- **Sem fragmentação:** Paginação evita fragmentação externa