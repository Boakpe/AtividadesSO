/**
 * @file    list_driver.c
 * @author  Seu Nome
 * @brief   Driver de dispositivo de caractere (char driver) que armazena dados em uma lista encadeada.
 * Escrita: Adiciona um nó na lista.
 * Leitura: Consome (lê e remove) um nó da lista. Se vazia, retorna "vazia".
 * @version 1.0
 */

#include <linux/module.h>    // Necessário para qualquer módulo do kernel
#include <linux/kernel.h>    // Contém macros úteis (como KERN_INFO) e funções do kernel
#include <linux/init.h>      // Macros para __init e __exit
#include <linux/fs.h>        // Estruturas de arquivo (file_operations, inode, etc.)
#include <linux/cdev.h>      // Estrutura e funções para char devices (cdev)
#include <linux/uaccess.h>   // Funções para troca de dados entre User Space e Kernel Space (copy_to/from_user)
#include <linux/slab.h>      // Gerenciamento de memória no kernel (kmalloc, kfree)
#include <linux/list.h>      // API de listas encadeadas circulares do Kernel Linux
#include <linux/mutex.h>     // Mecanismos de exclusão mútua (mutex) para evitar race conditions
#include <linux/device.h>    // Criação automática de arquivos de dispositivo em /dev (class, device)
#include <linux/version.h>   // Usado para verificar a versão do kernel (compatibilidade)

#define DEVICE_NAME "list_device" // O nome como aparecerá em /dev
#define CLASS_NAME "list_class"   // O nome da classe do dispositivo no sysfs

// Metadados do módulo (boas práticas)
MODULE_LICENSE("GPL");            // Licença (GPL evita "tainting" do kernel)
MODULE_AUTHOR("Seu Nome");
MODULE_DESCRIPTION("Driver de caractere com lista encadeada");
MODULE_VERSION("1.0");

/**
 * struct data_node - Estrutura para armazenar cada mensagem escrita no driver.
 * @message: Ponteiro para a string (buffer) armazenada.
 * @size:    Tamanho da mensagem armazenada.
 * @list:    Estrutura list_head do kernel que permite ligar este nó à lista encadeada.
 */
struct data_node {
    char *message;
    int size;
    struct list_head list; // O "gancho" que conecta este nó à lista principal
};

// Variáveis Globais
static int majorNumber;                  // Armazena o número 'Major' atribuído dinamicamente ao driver
static struct class* listClass  = NULL; // Estrutura da classe do dispositivo
static struct device* listDevice = NULL; // Estrutura do dispositivo criado
static struct cdev listCdev;             // Estrutura interna de representação do Char Device

// Inicialização da lista e do mutex
static LIST_HEAD(msg_list);       // Cria e inicializa a cabeça da lista chamada 'msg_list'
static DEFINE_MUTEX(list_mutex);  // Cria e inicializa um mutex chamado 'list_mutex' para proteger a lista

// Protótipos das funções (operações de arquivo)
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

/**
 * fops - Mapeia as chamadas de sistema (open, read, write...) para as funções deste driver.
 */
static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

/**
 * list_driver_init - Função de inicialização do módulo.
 * Executada quando o comando `insmod` é chamado.
 */
static int __init list_driver_init(void){
    dev_t dev_no; // Estrutura que segura o Major e Minor number
    int ret;

    // 1. Aloca dinamicamente um Major Number disponível
    // alloc_chrdev_region(saída, minor_inicial, contagem, nome)
    ret = alloc_chrdev_region(&dev_no, 0, 1, DEVICE_NAME);
    if (ret < 0){
        printk(KERN_ALERT "ListDriver: Falha ao alocar major number\n");
        return ret;
    }
    majorNumber = MAJOR(dev_no); // Extrai o número Major da estrutura dev_t
    
    // 2. Cria a classe do dispositivo (necessário para aparecer em /sys/class)
    // Verifica a versão do kernel pois a API mudou na versão 6.4
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    listClass = class_create(CLASS_NAME);
#else
    listClass = class_create(THIS_MODULE, CLASS_NAME);
#endif

    if (IS_ERR(listClass)){ // Verifica se houve erro na criação
        unregister_chrdev_region(dev_no, 1); // Desfaz a alocação do número
        return PTR_ERR(listClass);
    }

    // 3. Inicializa e registra o Char Device (cdev)
    cdev_init(&listCdev, &fops); // Liga a estrutura cdev às nossas funções fops
    ret = cdev_add(&listCdev, dev_no, 1); // Adiciona o dispositivo ao sistema
    if (ret < 0) {
        class_destroy(listClass);
        unregister_chrdev_region(dev_no, 1);
        return ret;
    }

    // 4. Cria o nó do dispositivo em /dev (o arquivo físico que o usuário interage)
    // device_create(classe, pai, dev_t, dados_privados, string de formatação do nome)
    listDevice = device_create(listClass, NULL, dev_no, NULL, DEVICE_NAME);
    if (IS_ERR(listDevice)){
        cdev_del(&listCdev); // Remove o cdev
        class_destroy(listClass); // Destrói a classe
        unregister_chrdev_region(dev_no, 1); // Libera o número
        return PTR_ERR(listDevice);
    }

    printk(KERN_INFO "ListDriver: Modulo carregado. Dispositivo criado em /dev/%s\n", DEVICE_NAME);
    return 0; // Sucesso
}

/**
 * list_driver_exit - Função de limpeza do módulo.
 * Executada quando o comando `rmmod` é chamado.
 */
static void __exit list_driver_exit(void){
    struct data_node *ptr, *next;

    // 1. Limpeza da memória: Percorre a lista e deleta todos os nós restantes
    mutex_lock(&list_mutex); // Bloqueia para garantir que ninguém esteja lendo/escrevendo agora
    
    // list_for_each_entry_safe é usado quando vamos deletar itens durante a iteração
    // 'ptr' é o item atual, 'next' é o armazenamento temporário do próximo
    list_for_each_entry_safe(ptr, next, &msg_list, list) {
        kfree(ptr->message);    // Libera a string alocada
        list_del(&ptr->list);   // Remove o nó da lista encadeada do kernel
        kfree(ptr);             // Libera a estrutura do nó
    }
    mutex_unlock(&list_mutex);

    // 2. Destruição dos componentes do driver na ordem inversa da criação
    device_destroy(listClass, MKDEV(majorNumber, 0)); // Remove /dev/list_device
    cdev_del(&listCdev);                              // Remove o registro do cdev
    class_unregister(listClass);                      // Remove o registro da classe
    class_destroy(listClass);                         // Destrói a struct classe
    unregister_chrdev_region(MKDEV(majorNumber, 0), 1); // Devolve o Major Number
    
    printk(KERN_INFO "ListDriver: Modulo descarregado.\n");
}

/**
 * dev_open - Chamado quando o arquivo é aberto (ex: `cat /dev/list_device`)
 */
static int dev_open(struct inode *inodep, struct file *filep){
    // Não precisamos fazer nada específico na abertura para este exercício simples
    return 0;
}

/**
 * dev_release - Chamado quando o arquivo é fechado
 */
static int dev_release(struct inode *inodep, struct file *filep){
    // Não precisamos fazer nada específico no fechamento
    return 0;
}

/**
 * dev_read - Lê dados do dispositivo para o usuário.
 * Comportamento: Retira o nó mais antigo (FIFO) e retorna o texto. Se vazia, retorna "vazia".
 */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
    struct data_node *entry;
    int bytes_read = 0;
    const char *msg_vazia = "vazia\n";
    int msg_len;

    // Tenta obter o mutex. Se for interrompido por um sinal, retorna erro.
    if (mutex_lock_interruptible(&list_mutex))
        return -ERESTARTSYS;

    // Verifica se a lista está vazia usando a macro da API de listas
    if (list_empty(&msg_list)) {
        msg_len = strlen(msg_vazia);
        
        // copy_to_user(destino_user, origem_kernel, tamanho)
        // Retorna 0 em sucesso (bytes NÃO copiados)
        if (copy_to_user(buffer, msg_vazia, msg_len) != 0) {
            mutex_unlock(&list_mutex); // Nunca esquecer de destravar em caso de erro
            return -EFAULT;
        }
        bytes_read = msg_len;
    } 
    else {
        // Pega o primeiro nó da lista.
        // list_first_entry(cabeça, tipo_da_struct, nome_do_campo_list_na_struct)
        entry = list_first_entry(&msg_list, struct data_node, list);
        msg_len = entry->size;
        
        // Garante que não vamos copiar mais do que o usuário pediu
        if (len < msg_len) msg_len = len;

        // Copia a mensagem armazenada para o buffer do usuário
        if (copy_to_user(buffer, entry->message, msg_len) != 0) {
            mutex_unlock(&list_mutex);
            return -EFAULT;
        }

        // Lógica de consumo: Remove da lista e libera memória
        list_del(&entry->list); // Remove os ponteiros da lista
        kfree(entry->message);  // Libera a string
        kfree(entry);           // Libera o nó
        
        bytes_read = msg_len;
    }

    mutex_unlock(&list_mutex); // Libera o acesso para outros processos
    return bytes_read; // Retorna quantos bytes foram lidos
}

/**
 * dev_write - Escreve dados do usuário para o dispositivo.
 * Comportamento: Cria um novo nó, copia o texto e adiciona ao final da lista.
 */
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
    struct data_node *new_node;

    // 1. Aloca memória para a estrutura do nó (GFP_KERNEL = alocação normal de kernel)
    new_node = kmalloc(sizeof(struct data_node), GFP_KERNEL);
    if (!new_node) return -ENOMEM; // Out of memory

    // 2. Aloca memória para a string (mensagem) + 1 para o caractere nulo
    new_node->message = kmalloc(len + 1, GFP_KERNEL);
    if (!new_node->message) {
        kfree(new_node); // Se falhar a string, libera o nó criado antes de sair
        return -ENOMEM;
    }

    // 3. Copia os dados do espaço do usuário (buffer) para o espaço do kernel (new_node->message)
    if (copy_from_user(new_node->message, buffer, len)) {
        kfree(new_node->message);
        kfree(new_node);
        return -EFAULT; // Bad address
    }
    
    new_node->message[len] = '\0'; // Garante terminação da string
    new_node->size = len;

    // 4. Seção Crítica: Adicionar à lista
    if (mutex_lock_interruptible(&list_mutex)) {
        // Se falhar o lock, limpa tudo pois não vamos conseguir inserir
        kfree(new_node->message);
        kfree(new_node);
        return -ERESTARTSYS;
    }
    
    // Adiciona o novo nó ao final da lista (Tail)
    list_add_tail(&new_node->list, &msg_list);
    
    mutex_unlock(&list_mutex);

    printk(KERN_INFO "ListDriver: Dado adicionado (%zu bytes)\n", len);
    return len; // Retorna o número de bytes escritos (sucesso)
}

module_init(list_driver_init);
module_exit(list_driver_exit);