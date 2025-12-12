#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h> // Define task_struct e init_task
#include <linux/init.h>
#include <linux/list.h> // Define list_head, list_for_each, list_entry

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Student");
MODULE_DESCRIPTION("Modulo para listar processos usando listas encadeadas do Cap 11");

static int __init listar_processos_init(void)
{
    struct task_struct *task;
    struct list_head *list;

    printk(KERN_INFO "--- Inicio da Lista de Processos ---\n");

    /*
     * CONCEITOS DO CAPÍTULO 11:
     *
     * 1. init_task é a "cabeça" (head) da lista de processos.
     * 2. &init_task.tasks é o ponto de partida do tipo struct list_head.
     * 3. list_for_each: Macro explicada na pág. 299 para iterar sobre a lista.
     * 4. list_entry: Macro explicada na pág. 297/298 para recuperar a estrutura
     *    pai (task_struct) a partir do ponteiro da lista.
     */

    list_for_each(list, &init_task.tasks)
    {
        /*
         * list_entry mapeia o ponteiro 'list' de volta para a estrutura que o contém.
         * Parâmetros:
         * - list: o ponteiro atual da iteração.
         * - struct task_struct: o tipo da estrutura que contém a lista.
         * - tasks: o nome do campo list_head dentro de task_struct.
         */
        task = list_entry(list, struct task_struct, tasks);

        // Imprime o ID do processo (pid) e o nome do comando (comm)
        printk(KERN_INFO "PID: %d | Processo: %s\n", task->pid, task->comm);
    }

    printk(KERN_INFO "--- Fim da Lista de Processos ---\n");

    return 0;
}

static void __exit listar_processos_exit(void)
{
    printk(KERN_INFO "Modulo de lista de processos removido.\n");
}

module_init(listar_processos_init);
module_exit(listar_processos_exit);