## Respostas - Lista Avaliativa 6

### 1) Manipulação de Nomes Longos em Diretórios

**Problema:**
Entradas de diretório tradicionalmente têm tamanho fixo, mas nomes de arquivos podem ser longos e de tamanho variável.

**Soluções:**

**1) Método in-line (tamanho variável):**
- Cada entrada tem tamanho variável para acomodar o nome completo
- **Vantagem:** Simples, todo nome está na entrada
- **Desvantagem:** Fragmentação, dificulta gerenciamento

**2) Heap (usado por muitos sistemas Unix/Linux):**
- Entrada do diretório tem tamanho fixo com ponteiro para o nome
- Nomes armazenados em heap no final do bloco do diretório
- **Estrutura:**
  ```
  [Inode | Tamanho entrada | Ponteiro nome] → "arquivo_longo.txt"
  ```
- **Vantagens:** 
  - Entradas principais mantêm tamanho fixo
  - Fácil de gerenciar
  - Usado no ext2/ext3/ext4

**3) Método de continuação:**
- Se nome não cabe, usa-se entradas adicionais marcadas como continuação
- Primeira entrada tem metadados, seguintes apenas mais caracteres do nome
- **Desvantagem:** Múltiplas entradas por arquivo

**4) Nome em bloco separado (Windows NTFS):**
- Se nome é curto: armazenado direto no MFT (Master File Table)
- Se nome é longo: armazenado em bloco separado apontado pelo MFT

**5) Múltiplas entradas com índice:**
- Uma entrada índice aponta para várias entradas de componentes do nome
- Usado em sistemas que precisam suportar nomes muito longos

**Abordagem moderna (ext4, NTFS, etc.):**
Combinação de métodos com alocação dinâmica e ponteiros, permitindo nomes de centenas de caracteres sem desperdiçar espaço.

### 2) Journaling em Sistema de Arquivos

**O que é:**
Técnica que registra mudanças a serem feitas no sistema de arquivos em um log (journal) antes de efetivamente realizá-las, garantindo consistência após falhas.

**Funcionamento:**

**1) Fase de escrita no Journal:**
- Operação (criar arquivo, escrever dados, deletar, etc.) é escrita no journal
- Journal é uma área circular no disco
- Entrada contém: operação, blocos envolvidos, dados

**2) Fase de commit:**
- Após todas as informações estarem no journal, marca-se a transação como "committed"
- Essa marcação indica que a operação pode ser aplicada

**3) Fase de checkpoint:**
- Operação é efetivamente aplicada no sistema de arquivos
- Estruturas de dados (inodes, bitmaps, blocos) são atualizadas no disco

**4) Fase de limpeza:**
- Após sucesso, entrada do journal é marcada como livre
- Journal pode ser reutilizado circularmente

**Em caso de falha:**

**Antes do commit:**
- Operação incompleta no journal é descartada
- Sistema de arquivos permanece consistente

**Após commit mas antes do checkpoint:**
- Na recuperação, sistema lê o journal
- Operações commitadas são reaplicadas (replay do journal)
- Sistema volta ao estado consistente

**Tipos de Journaling:**

**1) Journal completo (data + metadata):**
- Tudo é registrado no journal
- Mais seguro mas mais lento
- Exemplo: ext4 com modo `data=journal`

**2) Journal de metadata apenas:**
- Apenas metadados (inodes, diretórios) no journal
- Dados vão direto para o disco
- Mais rápido, padrão no ext4
- Exemplo: ext4 com modo `data=ordered`

**3) Writeback:**
- Metadata no journal, dados escritos sem ordem
- Mais rápido, menos garantias

**Exemplo prático:**

Criar arquivo "teste.txt" com 4KB:
```
1. Journal recebe:
   - Alocar inode 1234
   - Alocar bloco 5678
   - Adicionar entrada "teste.txt" no diretório
   - Escrever dados no bloco 5678
   - Marcar COMMIT

2. Sistema aplica mudanças no disco

3. Journal marca operação como concluída
```

Se falha entre passo 1 e 2: journal é replicado na recuperação.

**Vantagens:**
- Recuperação rápida após crash (segundos vs minutos/horas)
- Garante consistência do sistema de arquivos
- Previne corrupção de estruturas críticas

### 3) Alocação Baseada em Inodes

**Estrutura:**

**Inode (Index Node):**
Estrutura de dados que contém metadados e localização dos blocos de dados de um arquivo.

**Conteúdo do Inode:**
- Tipo de arquivo (regular, diretório, link, device)
- Permissões (rwx para owner/group/others)
- Proprietário (UID) e grupo (GID)
- Tamanho do arquivo
- Timestamps (criação, modificação, acesso)
- Número de hard links
- **Ponteiros para blocos de dados**

**Sistema de Ponteiros (Unix/Linux tradicional):**

Inode contém 15 ponteiros:

```
Inode
├─ 12 ponteiros diretos     → Apontam diretamente para blocos de dados
├─ 1 ponteiro indireto      → Aponta para bloco com ponteiros
├─ 1 ponteiro duplo indireto → Aponta para bloco de ponteiros de ponteiros
└─ 1 ponteiro triplo indireto → Três níveis de indireção
```

**Exemplo (ext2/ext3, blocos de 4KB):**

**Arquivo pequeno (< 48 KB):**
- Usa apenas ponteiros diretos
- Acesso rápido, sem indireção

**Arquivo médio (48 KB - 4 MB):**
- Usa ponteiros diretos + indireto simples
- Indireto simples: 1024 ponteiros × 4KB = 4MB

**Arquivo grande (> 4 MB):**
- Usa indireto duplo
- 1024 × 1024 ponteiros × 4KB = 4GB

**Arquivo muito grande:**
- Usa indireto triplo
- Pode endereçar até 4TB

**Funcionamento:**

```
Para ler byte no offset 50000 de um arquivo:

1. Determina qual bloco: 50000 / 4096 = bloco 12
2. Bloco 12 está no 1º ponteiro indireto (após 12 diretos)
3. Lê bloco apontado pelo ponteiro indireto
4. Dentro desse bloco, pega o ponteiro 0 (12-12=0)
5. Esse ponteiro leva ao bloco de dados desejado
6. Offset dentro do bloco: 50000 % 4096 = 1808
7. Lê dados a partir do byte 1808 desse bloco
```

**Vantagens:**
- Eficiente para arquivos pequenos (acesso direto)
- Suporta arquivos grandes (através de indireção)
- Não desperdiça espaço com arquivos pequenos
- Acesso aleatório eficiente

**Desvantagem:**
- Arquivos muito grandes requerem múltiplas leituras de disco para acessar blocos

### 4) Tabela FAT (File Allocation Table)

**Estrutura:**

FAT é uma tabela com uma entrada para cada bloco (cluster) do disco.

**Conteúdo de cada entrada:**
- **Número do próximo bloco** da cadeia (se arquivo continua)
- **EOF (End of File):** Marca fim do arquivo (ex: 0xFFFF)
- **0:** Bloco livre
- **BAD:** Bloco defeituoso
- **RESERVED:** Reservado

**Funcionamento:**

**Entrada do diretório contém:**
- Nome do arquivo
- Atributos
- Tamanho
- Data/hora
- **Número do primeiro bloco**

**Para ler arquivo:**
1. Diretório indica primeiro bloco (ex: bloco 5)
2. Lê dados do bloco 5
3. Consulta FAT[5] → obtém próximo bloco (ex: 7)
4. Lê dados do bloco 7
5. Consulta FAT[7] → obtém próximo bloco (ex: 15)
6. Continua até encontrar EOF

**Exemplo:**

```
Arquivo "documento.txt" começa no bloco 4:

FAT:
Índice | Conteúdo
-------|----------
  0    |   0        (livre)
  1    |   0        (livre)
  2    | BAD        (defeituoso)
  3    |   0        (livre)
  4    |   8        (próximo: bloco 8)
  5    |   EOF      (arquivo terminado)
  6    |   9        (próximo: bloco 9)
  7    |   0        (livre)
  8    |   12       (próximo: bloco 12)
  9    |   EOF
 10    |   0
 11    |   0
 12    |   EOF      (fim de documento.txt)
```

Cadeia do arquivo: 4 → 8 → 12 → EOF

**Versões:**

**FAT12:** 12 bits por entrada (disquetes)
**FAT16:** 16 bits por entrada (até 2GB)
**FAT32:** 32 bits por entrada (até 2TB)
**exFAT:** Versão moderna para pendrives/cartões

**Vantagens:**
- Simples de implementar
- Fácil de recuperar arquivos
- Suportado universalmente
- Boa para dispositivos removíveis

**Desvantagens:**
- FAT fica grande para discos grandes
- Acesso sequencial (lento para arquivos fragmentados)
- Sem journaling (FAT32)
- Sem permissões de arquivos sofisticadas
- Fragmentação ao longo do tempo

### 5) Access Control Lists (ACL)

**Conceito:**

ACLs estendem o modelo tradicional de permissões Unix (owner/group/others) permitindo especificar permissões para múltiplos usuários e grupos específicos.

**Estrutura:**

Cada arquivo pode ter uma lista de entradas ACL, cada uma especificando:
- **Tipo:** USER, GROUP, MASK, OTHER
- **Identificador:** UID ou GID específico
- **Permissões:** read, write, execute

**Formato típico:**
```
user::rwx           # Proprietário
user:joao:r-x       # Usuário específico (joão)
user:maria:rw-      # Usuária específica (maria)
group::r-x          # Grupo do arquivo
group:dev:rwx       # Grupo específico (dev)
mask::rwx           # Máscara de permissões efetivas
other::r--          # Outros usuários
```

**Como funciona:**

**1) Verificação de acesso:**
Quando usuário tenta acessar arquivo:
```
1. É o proprietário? → Usa permissões user::
2. Tem entrada ACL específica? → Usa permissões user:uid:
3. Pertence ao grupo? → Usa permissões group::
4. Pertence a grupo com ACL? → Usa permissões group:gid:
5. Nenhuma das anteriores? → Usa permissões other::
```

**2) Máscara:**
- Limita permissões máximas efetivas para usuários/grupos nomeados
- Exemplo: Se máscara é `r-x`, mesmo que ACL diga `rwx`, efetivo é `r-x`

**Exemplo prático:**

```bash
# Arquivo projeto.txt
# Tradicional: rwxr-x---

# Com ACL:
$ setfacl -m u:joao:rw projeto.txt   # João pode ler e escrever
$ setfacl -m u:maria:r projeto.txt   # Maria só pode ler
$ setfacl -m g:admin:rwx projeto.txt # Grupo admin tem acesso total

$ getfacl projeto.txt
# file: projeto.txt
# owner: breno
# group: alunos
user::rwx
user:joao:rw-
user:maria:r--
group::r-x
group:admin:rwx
mask::rwx
other::---
```

**Casos de uso:**

1. **Projeto colaborativo:**
   - Líder: rwx
   - Desenvolvedores: rw-
   - Revisores: r--
   - Outros: ---

2. **Servidor web:**
   - Apache (usuário www-data): r--
   - Desenvolvedor: rwx
   - Backup (usuário backup): r--

3. **Diretório compartilhado:**
   - Múltiplos usuários com diferentes níveis de acesso
   - Alguns podem adicionar, outros só ler

**Comandos Linux:**
```bash
setfacl -m u:usuario:rwx arquivo  # Modifica ACL
getfacl arquivo                   # Visualiza ACL
setfacl -x u:usuario arquivo      # Remove entrada ACL
setfacl -b arquivo                # Remove todas as ACLs
```

**Vantagens:**
- Controle granular de permissões
- Suporta múltiplos usuários/grupos
- Flexível para cenários complexos
- Retrocompatível com permissões Unix tradicionais

**Suporte:**
- ext3/ext4, XFS, Btrfs (Linux)
- NTFS (Windows)
- ZFS (BSD/Solaris)