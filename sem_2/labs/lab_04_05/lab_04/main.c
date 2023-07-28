#include <linux/init_task.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/fs_struct.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Inspirate789");

static int __init mod_init(void)
{
    printk(KERN_INFO " + module is loaded.\n");
    struct task_struct *task = &init_task;
    do
    {
        printk(KERN_INFO " + %s (%d) (%d - state, %d - prio, flags - %d, policy - %d), parent %s (%d), d_name %s",
            task->comm, task->pid, task->__state, task->prio, task->flags, task->policy, task->parent->comm, task->parent->pid, task->fs->root.dentry->d_name.name);
    } while ((task = next_task(task)) != &init_task);

   // task = current;
    printk(KERN_INFO " + %s (%d) (%d - state, %d - prio, flags - %d, policy - %d), parent %s (%d), d_name %s",
        current->comm, current->pid, current->__state, current->prio, current->flags, current->policy, current->parent->comm, current->parent->pid, current->fs->root.dentry->d_name.name);
    return 0;
}

static void __exit mod_exit(void)
{
    printk(KERN_INFO " + %s - %d, parent %s - %d\n", current->comm,
           current->pid, current->parent->comm, current->parent->pid);
    printk(KERN_INFO " + module is unloaded.\n");
}

module_init(mod_init);
module_exit(mod_exit);
