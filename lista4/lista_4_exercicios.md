## Respostas - Lista Avaliativa 4

### 1) Memória Virtual e Paginação

**Memória Virtual:**
Técnica que cria uma abstração da memória física, dando a cada processo a ilusão de ter um espaço de endereçamento grande e contínuo. Permite:
- Execução de programas maiores que a RAM física
- Isolamento entre processos
- Compartilhamento controlado de memória
- Proteção de memória

**Paginação:**
Implementação da memória virtual onde:
- Espaço de endereçamento virtual é dividido em **páginas** (blocos de tamanho fixo, tipicamente 4KB)
- Memória física é dividida em **frames** (mesmo tamanho das páginas)
- Sistema operacional mantém **tabelas de páginas** mapeando páginas virtuais para frames físicos
- Páginas podem estar na RAM ou no disco (swap)

**Funcionamento:**
1. Processo acessa endereço virtual
2. MMU consulta tabela de páginas
3. Se página está na RAM: traduz para endereço físico
4. Se página está no disco: ocorre **page fault**, SO carrega a página do disco para RAM

### 2) Tradução de Endereço Virtual para Físico

**Exemplo:**
- Endereço virtual: 32 bits (4 GB de espaço virtual)
- Endereço físico: 24 bits (16 MB de RAM física)
- Tamanho da página: 4 KB (12 bits para offset)

**Estrutura do endereço:**
- **Endereço virtual (32 bits):** `[20 bits: número da página] [12 bits: offset]`
- **Endereço físico (24 bits):** `[12 bits: número do frame] [12 bits: offset]`

**Sim, é possível ter endereço virtual maior que físico!**
- Nem todas as páginas virtuais precisam estar na RAM simultaneamente
- Muitas podem estar em disco (swap)
- Isso é a essência da memória virtual

**Processo de tradução:**
1. CPU gera endereço virtual: `0x00403ABC`
   - Número da página: `0x00403` (20 bits superiores)
   - Offset: `0xABC` (12 bits inferiores = 2748)

2. MMU consulta entrada da tabela de páginas para página `0x00403`
   - Se bit de **presente** = 1: página está na RAM
   - Encontra número do frame, ex: `0x5A2`
   
3. MMU monta endereço físico:
   - Frame: `0x5A2`
   - Offset: `0xABC` (mantido)
   - Endereço físico: `0x5A2ABC`

4. Se bit presente = 0: **page fault** → SO carrega página do disco

**Função da MMU (Memory Management Unit):**
- Hardware dedicado para tradução de endereços
- Consulta tabelas de páginas automaticamente
- Usa TLB para acelerar traduções
- Gera interrupção (page fault) quando página não está presente
- Opera transparentemente para o programa

### 3) Estrutura da Tabela de Páginas

**Entrada da Tabela de Páginas (PTE - Page Table Entry):**

Cada entrada contém informações sobre uma página virtual:

**Campos principais:**
- **Número do frame físico:** Localização da página na RAM (bits mais significativos)
- **Bit de presente/válido:** Indica se página está na RAM (1) ou disco (0)
- **Bits de proteção:** Permissões (read, write, execute)
- **Bit de modificação (dirty bit):** Indica se página foi modificada (importante para swap)
- **Bit de referência (referenced bit):** Indica se página foi acessada (usado para algoritmos de substituição)
- **Bit de cache:** Controla se página pode ser cacheada
- **Bit de usuário/supervisor:** Indica se página pode ser acessada em modo usuário

**Organização:**
```
| Frame number | Present | R | W | X | Dirty | Referenced | User | ...
```

**Tipos de organização:**
1. **Tabela de um nível:** Array simples, mas muito grande para espaços de 32 ou 64 bits
2. **Tabela multinível:** Hierarquia de tabelas (economiza espaço)
3. **Tabela invertida:** Uma entrada por frame físico (não por página virtual)

### 4) TLB (Translation Lookaside Buffer)

**O que é:**
Cache de hardware dentro da MMU que armazena traduções recentes de endereços virtuais para físicos.

**Para que serve:**
Acelerar a tradução de endereços, evitando consultas à tabela de páginas na memória RAM (que é lenta).

**Como funciona:**

1. **Acesso ao endereço virtual:**
   - MMU primeiro verifica se tradução está na TLB (TLB lookup)
   
2. **TLB Hit (acerto):**
   - Tradução encontrada na TLB
   - Endereço físico obtido imediatamente (muito rápido, ~1 ciclo)
   - Acesso à memória prossegue

3. **TLB Miss (falha):**
   - Tradução não está na TLB
   - MMU consulta tabela de páginas na RAM (lento, várias dezenas de ciclos)
   - Tradução é adicionada à TLB
   - Entrada antiga pode ser removida (política de substituição)

**Características:**
- **Pequena:** Tipicamente 64-256 entradas
- **Associativa:** Busca rápida em paralelo
- **Alta taxa de acerto:** Princípio da localidade (90-98%)
- **Gerenciada por hardware** (mais comum) ou software
- **Limpa em troca de contexto:** Cada processo tem seu próprio espaço de endereçamento

**Importância:**
Sem TLB, cada acesso à memória exigiria múltiplos acessos (para consultar tabelas de páginas), tornando o sistema muito lento.