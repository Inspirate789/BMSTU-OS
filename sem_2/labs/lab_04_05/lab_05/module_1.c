#include <linux/init.h>
#include <linux/module.h>

#include "kernel_module.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Inspirate789");

char *module_1_data = "ABCDE";

extern char *module_1_proc(void) { return module_1_data; }
static char *module_1_local(void) { return module_1_data; }
extern char *module_1_noexport(void) { return module_1_data; }

EXPORT_SYMBOL(module_1_data);
EXPORT_SYMBOL(module_1_proc);

static int __init module_1_init(void)
{
    printk("+ module_1 started.\n");
    printk("+ module_1 use local from module_1: %s\n", module_1_local());
    printk("+ module_1 use noexport from module_1: %s\n", module_1_noexport());
    return 0;
}

static void __exit module_1_exit(void) { printk("+ module module_1 unloaded.\n"); }

module_init(module_1_init);
module_exit(module_1_exit);
