#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/seq_file.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include <linux/time.h>

#define IRQ_LINE 1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Inspirate789");

char *tasklet_data = "tasklet_data";
struct tasklet_struct *tasklet;
int code;

void tasklet_function(unsigned long data)
{
    char *ascii[84] = 
    {" ", "Esc", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "+", "Backspace", 
     "Tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Ctrl",
     "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "\"", "'", "Shift (left)", "|", 
     "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "Shift (right)", 
     "*", "Alt", "Space", "CapsLock", 
     "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10",
     "NumLock", "ScrollLock", "Home", "Up", "Page-Up", "-", "Left",
     " ", "Right", "+", "End", "Down", "Page-Down", "Insert", "Delete"};
    if (code < 84)
    {
        printk(" tasklet_function code: %s, time: %llu, data: %s", ascii[code], ktime_get(), (char *)data);
    }
}

irqreturn_t my_handler(int irq, void *dev)
{
    if (irq == IRQ_LINE)
    {
        code = inb(0x60);
        printk(" my_handler before sheduled time: %llu", ktime_get());
        tasklet_schedule(tasklet);
        printk(" my_handler after  sheduled time: %llu", ktime_get());
        return IRQ_HANDLED;
    }
    printk(" irq != IRQ_LINE");
    return IRQ_NONE;
}

static int __init my_tasklet_init(void)
{
    tasklet = kmalloc(sizeof(struct tasklet_struct), GFP_KERNEL); 
    if (!tasklet)
    {
        printk(" kmalloc failed");
        return 1;
    }

    tasklet_init(tasklet, tasklet_function, (unsigned long)tasklet_data);

    if (request_irq(IRQ_LINE, my_handler, IRQF_SHARED, "my_irq_handler", &my_handler))
    {
        printk(" request_irq failed");
        return 1;
    }
    printk(" module loaded");
    return 0;
}

static void __exit my_tasklet_exit(void)
{
    tasklet_kill(tasklet);
    kfree(tasklet);
    free_irq(IRQ_LINE, &my_handler);
    printk(" module unloaded");
}

module_init(my_tasklet_init) 
module_exit(my_tasklet_exit)

