#include <linux/init.h>
#include <linux/module.h>

#include "kernel_module.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Inspirate789");

static int __init module_2_init(void)
{
    printk("+ module_3 started.\n");
    printk("+ data string exported from module_1: %s\n", module_1_data);
    printk("+ string returned module_1_proc() is: %s\n", module_1_proc());

    return -1;
}
module_init(module_2_init);
