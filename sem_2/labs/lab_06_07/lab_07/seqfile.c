#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/init.h>  
#include <linux/vmalloc.h>
#include <linux/proc_fs.h> 
#include <linux/seq_file.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Inspirate789");
MODULE_DESCRIPTION("Fortune Cookie Kernel Module");

#define FORTUNE_DIRNAME "fortune_dir"
#define FORTUNE_FILENAME "fortune_file"
#define FORTUNE_SYMLINK "fortune_symlink"
#define FORTUNE_PATH FORTUNE_DIRNAME "/" FORTUNE_FILENAME

#define MAX_BUF_SIZE PAGE_SIZE

#define KERN_LOG_MSG() { printk(KERN_INFO "FORTUNE_MODULE: %s called.\n", __func__); }
#define KERN_ERR_MSG(err) { printk(KERN_ERR "FORTUNE_MODULE: %s.\n", err); }
#define KERN_INFO_MSG(msg) { printk(KERN_INFO "FORTUNE_MODULE: %s.\n", msg); }

static struct proc_dir_entry *fortune_dir, *fortune_file, *fortune_symlink = NULL;
static char *buffer = NULL;

static void *ct_seq_start(struct seq_file *m, loff_t *pos)
{
    KERN_LOG_MSG();

    if (*pos == 0)
        return buffer;

    *pos = 0;
    return NULL;
}

static void *ct_seq_next(struct seq_file *m, void *v, loff_t *pos)
{
    KERN_LOG_MSG();

    if (buffer == NULL)
        return NULL;

    (*pos)++;
    printk(KERN_INFO "FORTUNE_MODULE: v = %p, pos = %Ld.\n", v, *pos);
    
    return NULL;
}

static void ct_seq_stop(struct seq_file *m, void *v) 
{
    KERN_LOG_MSG();

    if (v)
        printk(KERN_INFO "FORTUNE_MODULE: v is %p.\n", v);
    else
        printk(KERN_INFO "FORTUNE_MODULE: v is NULL.\n");
}

static int ct_seq_show(struct seq_file *sfile, void *v)
{
    KERN_LOG_MSG();
    seq_printf(sfile, (char *)v);
    
    return 0;
}

const struct seq_operations seq_ops = {
    start: ct_seq_start,
    next: ct_seq_next,
    stop: ct_seq_stop,
    show: ct_seq_show,
};

static int fortune_open(struct inode *sp_inode, struct file *sp_file)
{
    KERN_LOG_MSG();
    return seq_open(sp_file, &seq_ops);
}

static int fortune_release(struct inode *sp_node, struct file *sp_file) 
{
    KERN_LOG_MSG();
    return 0;
}

static ssize_t fortune_read(struct file *file, char __user *buf, size_t len, loff_t *f_pos)
{
    KERN_LOG_MSG();
    return seq_read(file, buf, len, f_pos);
}

static ssize_t fortune_write(struct file *file, const char __user *buf, size_t len, loff_t *ppos) 
{
    KERN_LOG_MSG();

    if (len > MAX_BUF_SIZE)
    {
        KERN_ERR_MSG("Buffer overflow");
        return -ENOSPC;
    }

    if (copy_from_user(buffer, buf, len) != 0)
    {
        KERN_ERR_MSG("copy_from_user function get a error");
        return -EFAULT;
    }

    buffer[len - 1] = '\0';

    return len;
}

static const struct proc_ops fops =
{
    proc_write: fortune_write,
    proc_read: fortune_read,
    proc_open: fortune_open,
    proc_release: fortune_release,
};

static void cleanup_fortune(void)
{
    KERN_LOG_MSG();

    if (fortune_symlink != NULL) 
        remove_proc_entry(FORTUNE_SYMLINK, NULL);

    if (fortune_file != NULL)
        remove_proc_entry(FORTUNE_FILENAME, fortune_dir);

    if (fortune_dir != NULL) 
        remove_proc_entry(FORTUNE_DIRNAME, NULL);

    vfree(buffer);
}

static int __init fortune_init(void) 
{
    KERN_LOG_MSG();

    if ((buffer = vzalloc(MAX_BUF_SIZE)) == NULL)
    {
        KERN_ERR_MSG("Allocate memory error.");
        return -ENOMEM;
    }

    if ((fortune_dir = proc_mkdir(FORTUNE_DIRNAME, NULL)) == NULL)
    {
        KERN_ERR_MSG("Error during create directory in proc");
        cleanup_fortune();
        return -ENOMEM;
    }

    if ((fortune_file = proc_create(FORTUNE_FILENAME, S_IRUGO | S_IWUGO, fortune_dir, &fops)) == NULL) 
    {
        KERN_ERR_MSG("Error during create file in proc");
        cleanup_fortune();
        return -ENOMEM;
    }

    if ((fortune_symlink = proc_symlink(FORTUNE_SYMLINK, NULL, FORTUNE_PATH)) == NULL)
    {
        KERN_ERR_MSG("Error during create symlink in proc");
        cleanup_fortune();
        return -ENOMEM;
    }

    KERN_INFO_MSG("Module has been successfully loaded.\n");
    return 0;
}

static void __exit fortune_exit(void) 
{
    KERN_LOG_MSG();
    cleanup_fortune();
    KERN_INFO_MSG("Module has been successfully removed");
}

module_init(fortune_init);
module_exit(fortune_exit);
