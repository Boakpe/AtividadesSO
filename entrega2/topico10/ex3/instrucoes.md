### 3. Como compilar e testar

1.  **Compile o módulo:**
    Abra o terminal na pasta onde salvou os arquivos e execute:
    ```bash
    make
    ```

2.  **Carregue o módulo:**
    ```bash
    sudo insmod list_driver.ko
    ```
    Verifique se carregou corretamente olhando os logs:
    ```bash
    sudo dmesg | tail
    ```

3.  **Permissões (Opcional):**
    Por padrão, apenas root pode escrever. Para facilitar o teste:
    ```bash
    sudo chmod 666 /dev/list_device
    ```

4.  **Teste de Escrita (Adicionar à lista):**
    Escreva algumas strings no dispositivo.
    ```bash
    echo "Primeira Mensagem" > /dev/list_device
    echo "Segunda Mensagem" > /dev/list_device
    ```

5.  **Teste de Leitura (Retirar da lista):**
    Como o comando `cat` tenta ler continuamente até receber um EOF (0), e o nosso driver retorna "vazia" quando acaba a lista, o `cat` entraria num loop infinito de "vazia". Para testar item por item, use o comando `head` ou leia manualmente.
    
    Ler o primeiro item:
    ```bash
    head -n 1 /dev/list_device
    # Saída esperada: Primeira Mensagem
    ```

    Ler o segundo item:
    ```bash
    head -n 1 /dev/list_device
    # Saída esperada: Segunda Mensagem
    ```

    Ler quando a lista está vazia:
    ```bash
    head -n 1 /dev/list_device
    # Saída esperada: vazia
    ```

6.  **Descarregar o módulo:**
    ```bash
    sudo rmmod list_driver
    ```

### Explicação do Funcionamento

1.  **Estrutura de Dados:** Utilizamos `struct data_node` que contém um ponteiro char (os dados) e uma `struct list_head`. Essa é a maneira padrão de criar listas encadeadas no Kernel Linux.
2.  **Escrita (`dev_write`):**
    *   Aloca memória para o nó e para a string usando `kmalloc`.
    *   Copia os dados do espaço do usuário (`copy_from_user`).
    *   Usa `mutex_lock` para garantir exclusividade.
    *   Usa `list_add_tail` para inserir o dado no fim da fila (comportamento FIFO).
3.  **Leitura (`dev_read`):**
    *   Bloqueia com `mutex`.
    *   Verifica `list_empty`. Se vazia, copia a string "vazia\n" para o usuário.
    *   Se não vazia, pega o primeiro item com `list_first_entry`.
    *   Copia o conteúdo para o usuário.
    *   Remove o nó da lista com `list_del`.
    *   Libera a memória (`kfree`) do buffer e do nó.
4.  **Limpeza (`list_driver_exit`):** É crucial iterar sobre a lista e liberar qualquer memória restante ao remover o módulo, caso contrário, ocorrerá vazamento de memória no Kernel (memory leak).