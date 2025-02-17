#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR( "Sriram Suresh <srisuresh@tamu.edu>");
MODULE_DESCRIPTION( "Basic read loadable kernel module");

static struct proc_dir_entry *custom_proc_node;

// ssize_t (*proc_read)(struct file *, char __user *, size_t, loff_t *);
static ssize_t pyjama_read(struct file* file_pointer,
                    char *user_space_buffer,
                    size_t count,
                    loff_t* offset) {
    char msg[] = "Ack!\n";
    size_t len = strlen(msg);
    int result;
    printk("pyjama_read\n");
    
    if (*offset >= len){
        return 0;
    }

    result = copy_to_user(user_space_buffer, msg, len);
    *offset += len;
    
    return len;
}

struct proc_ops driver_proc_ops = {
    .proc_read = pyjama_read
};

static int pyjama_module_init (void) {
    printk("pyjama_module_init: entry\n");

    // this line below creates /proc/pyjama_driver
    custom_proc_node = proc_create("pyjama_driver",
                                0,
                                NULL,
                                &driver_proc_ops);
    
    if (custom_proc_node == NULL) {
        printk("pyjama_module_init: error\n");
        return -1;
    }

    printk("pyjama_module_init: exit\n");
    return 0;
}

static void pyjama_module_exit (void) {
    printk("pyjama_module_exit: entry\n");

    proc_remove(custom_proc_node); // proc_remove(struct proc_dir_entry *);

    printk("pyjama_module_exit: exit\n");
}

// struct proc_dir_entry *proc_create(const char *name,
//                                     umode_t mode,
//                                     struct proc_dir_entry *parent,
//                                     const struct proc_ops *proc_ops);
// struct proc_dir_entry *proc_create("pyjama_driver",
//                                     0,
//                                     NULL,
//                                     NULL);

module_init (pyjama_module_init);
module_exit (pyjama_module_exit);
