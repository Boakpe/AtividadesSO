#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/device.h>

#define DEVICE_NAME "list_device"
#define CLASS_NAME "list_class"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Seu Nome");
MODULE_DESCRIPTION("Driver de caractere com lista encadeada");
MODULE_VERSION("1.0");

// Estrutura para armazenar os dados na lista
struct data_node {
    char *message;
    int size;
    struct list_head list;
};

// Variáveis globais
static int majorNumber;
static struct class*  listClass  = NULL;
static struct device* listDevice = NULL;
static struct cdev listCdev;

// Inicializa a cabeça da lista e o mutex
static LIST_HEAD(msg_list);
static DEFINE_MUTEX(list_mutex);

// Protótipos das funções
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

// Estrutura de operações de arquivo
static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

// Função chamada ao carregar o módulo
static int __init list_driver_init(void){
    dev_t dev_no;
    int ret;

    // 1. Aloca dinamicamente um Major Number
    ret = alloc_chrdev_region(&dev_no, 0, 1, DEVICE_NAME);
    if (ret < 0){
        printk(KERN_ALERT "ListDriver: Falha ao alocar major number\n");
        return ret;
    }
    majorNumber = MAJOR(dev_no);
    
    // 2. Cria a classe do dispositivo (para aparecer em /sys/class)
    listClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(listClass)){
        unregister_chrdev_region(dev_no, 1);
        return PTR_ERR(listClass);
    }

    // 3. Registra o Driver de Dispositivo
    cdev_init(&listCdev, &fops);
    ret = cdev_add(&listCdev, dev_no, 1);
    if (ret < 0) {
        class_destroy(listClass);
        unregister_chrdev_region(dev_no, 1);
        return ret;
    }

    // 4. Cria o nó do dispositivo em /dev (udev fará isso automaticamente)
    listDevice = device_create(listClass, NULL, dev_no, NULL, DEVICE_NAME);
    if (IS_ERR(listDevice)){
        cdev_del(&listCdev);
        class_destroy(listClass);
        unregister_chrdev_region(dev_no, 1);
        return PTR_ERR(listDevice);
    }

    printk(KERN_INFO "ListDriver: Modulo carregado. Dispositivo criado em /dev/%s\n", DEVICE_NAME);
    return 0;
}

// Função chamada ao descarregar o módulo
static void __exit list_driver_exit(void){
    struct data_node *ptr, *next;

    // Limpa a lista para evitar vazamento de memória
    mutex_lock(&list_mutex);
    list_for_each_entry_safe(ptr, next, &msg_list, list) {
        kfree(ptr->message);
        list_del(&ptr->list);
        kfree(ptr);
    }
    mutex_unlock(&list_mutex);

    // Remove o dispositivo
    device_destroy(listClass, MKDEV(majorNumber, 0));
    cdev_del(&listCdev);
    class_unregister(listClass);
    class_destroy(listClass);
    unregister_chrdev_region(MKDEV(majorNumber, 0), 1);
    
    printk(KERN_INFO "ListDriver: Modulo descarregado.\n");
}

static int dev_open(struct inode *inodep, struct file *filep){
    // Pode ser usado para inicializações por sessão
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep){
    return 0;
}

// Função de LEITURA: Retira um nó da lista
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
    struct data_node *entry;
    int bytes_read = 0;
    const char *msg_vazia = "vazia\n";
    int msg_len;

    // Bloqueia mutex para acesso seguro
    if (mutex_lock_interruptible(&list_mutex))
        return -ERESTARTSYS;

    // Se a lista estiver vazia
    if (list_empty(&msg_list)) {
        msg_len = strlen(msg_vazia);
        
        // Verifica se o usuário já leu "vazia" nesta chamada (simples lógica de EOF pode ser aplicada aqui, 
        // mas o requisito pede para retornar "vazia", o que fará 'cat' repetir infinitamente se não controlarmos.
        // Assumiremos que cada read retorna o dado solicitado).
        
        if (copy_to_user(buffer, msg_vazia, msg_len) != 0) {
            mutex_unlock(&list_mutex);
            return -EFAULT;
        }
        bytes_read = msg_len;
    } 
    else {
        // Pega o primeiro elemento (FIFO)
        entry = list_first_entry(&msg_list, struct data_node, list);

        msg_len = entry->size;
        
        // Copia dados para o usuário (trunca se o buffer do usuário for pequeno)
        if (len < msg_len) msg_len = len;

        if (copy_to_user(buffer, entry->message, msg_len) != 0) {
            mutex_unlock(&list_mutex);
            return -EFAULT;
        }

        // Remove da lista e libera memória
        list_del(&entry->list);
        kfree(entry->message);
        kfree(entry);
        
        bytes_read = msg_len;
    }

    mutex_unlock(&list_mutex);
    return bytes_read;
}

// Função de ESCRITA: Adiciona um nó na lista
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
    struct data_node *new_node;

    // Aloca estrutura do nó
    new_node = kmalloc(sizeof(struct data_node), GFP_KERNEL);
    if (!new_node) return -ENOMEM;

    // Aloca buffer para a mensagem
    new_node->message = kmalloc(len + 1, GFP_KERNEL); // +1 para null terminator se quiser printar no debug
    if (!new_node->message) {
        kfree(new_node);
        return -ENOMEM;
    }

    // Copia do usuário para o kernel
    if (copy_from_user(new_node->message, buffer, len)) {
        kfree(new_node->message);
        kfree(new_node);
        return -EFAULT;
    }
    
    new_node->message[len] = '\0'; // Garante terminação nula para segurança
    new_node->size = len;

    // Adiciona na lista de forma segura
    if (mutex_lock_interruptible(&list_mutex)) {
        kfree(new_node->message);
        kfree(new_node);
        return -ERESTARTSYS;
    }
    
    // Adiciona no final da lista (FIFO)
    list_add_tail(&new_node->list, &msg_list);
    
    mutex_unlock(&list_mutex);

    printk(KERN_INFO "ListDriver: Dado adicionado a lista (%zu bytes)\n", len);
    return len;
}

module_init(list_driver_init);
module_exit(list_driver_exit);