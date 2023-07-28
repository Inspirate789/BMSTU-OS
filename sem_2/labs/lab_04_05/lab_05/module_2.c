#include <linux/init.h>
#include <linux/module.h>

#include "kernel_module.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Inspirate789");

static int __init module_2_init(void)
{
    printk("+ module module_2 started.\n");
    printk("+ data string exported from module_1: %s\n", module_1_data);
    printk("+ string returned module_1_proc(): %s\n", module_1_proc());

    //printk( "+ module_2 use local from module_1: %s\n", module_1_local());

    //printk( "+ module_2 use noexport from module_1: %s\n", module_1_noexport());

    return 0;
}

static void __exit module_2_exit(void)
{
    printk("+ module_2 unloaded.\n");
}

module_init(module_2_init);
module_exit(module_2_exit);
