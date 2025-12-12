#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h> // Define task_struct e init_task
#include <linux/init.h>
#include <linux/list.h> // Define list_head, list_for_each, list_entry

MODULE_LICENSE("GPL");

static int __init listar_processos_init(void)
{
    struct task_struct *task;
    struct list_head *list;

    printk(KERN_INFO "--- Inicio da Lista de Processos ---\n");

    list_for_each(list, &init_task.tasks)
    {
        task = list_entry(list, struct task_struct, tasks);

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