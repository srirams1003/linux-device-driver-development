#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>  // For copy_from_user and copy_to_user
#include <linux/slab.h>     // For kmalloc and kfree
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR( "Sriram Suresh <srisuresh@tamu.edu>");
MODULE_DESCRIPTION( "Basic read loadable kernel module");

#define BUFFER_SIZE 256  // Define buffer size

static struct proc_dir_entry *custom_proc_node;
static char *kernel_buffer;  // Buffer to store written data
static size_t buffer_length = 0;  // Length of stored data

static ssize_t pyjama_write(struct file* file_pointer,
                    const char *user_space_buffer,
                    size_t count,
                    loff_t* offset) {

    if (!kernel_buffer) {
        printk(KERN_ERR "pyjama_write: kernel_buffer is NULL!\n");
        return -EFAULT;
    }

    if (count > BUFFER_SIZE - 1) // Limit input size
        count = BUFFER_SIZE - 1;

    memset(kernel_buffer, 0, BUFFER_SIZE); // Clear buffer
    if (copy_from_user(kernel_buffer, user_space_buffer, count))
    {
        printk("pyjama_write: Failed to copy from user\n");
        return -EFAULT;
    }

    kernel_buffer[count] = '\0'; // Null-terminate the string
    buffer_length = count;

    printk("pyjama_write: Stored %s\n", kernel_buffer);
    // printk("pyjama_write: Stored \"%s\"\n", kernel_buffer);
    return count;
}


static ssize_t pyjama_read(struct file* file_pointer,
                    char *user_space_buffer,
                    size_t count,
                    loff_t* offset) {
    size_t len = strlen(kernel_buffer);
    // printk(KERN_INFO "pyjama_read. strlen kernel_buffer: %zu\n", len);
    printk("pyjama_read\n");
    ssize_t ret;

    // printk("pyjama_read: outside count: %zu\n", count);
    // printk("pyjama_read: outside *offset: %llu\n", *offset);
    
    if (*offset >= len){
        return 0; // EOF
    }
    
    if (count > len - *offset){
        // printk("pyjama_read: len: %zu\n", len);
        // printk("pyjama_read: before count: %zu\n", count);
        // printk("pyjama_read: *offset: %llu\n", *offset);
        count = len - *offset;
        // printk("pyjama_read: after count: %zu\n", count);
    };

    ret = copy_to_user(user_space_buffer, kernel_buffer + *offset, count);
    
    if (ret)
        return -EFAULT;

    *offset += count;
    
    return count;
}

struct proc_ops driver_proc_ops = {
    .proc_read = pyjama_read,
    .proc_write = pyjama_write,
};

static int pyjama_module_init (void) {
    printk("pyjama_module_init: entry\n");

    kernel_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (!kernel_buffer) {
        printk(KERN_ERR "pyjama_module_init: Failed to allocate kernel_buffer\n");
        return -ENOMEM; //ENOMEM = no memory
    }

    // this line below creates /proc/pyjama_driver
    custom_proc_node = proc_create("pyjama_driver",
                                0666,
                                NULL,
                                &driver_proc_ops);
    
    if (custom_proc_node == NULL) {
        printk("pyjama_module_init: error\n");
        kfree(kernel_buffer); // Free the memory if proc_create fails
        return -1;
    }

    printk("pyjama_module_init: exit\n");
    return 0;
}

static void pyjama_module_exit (void) {
    printk("pyjama_module_exit: entry\n");

    proc_remove(custom_proc_node); // proc_remove(struct proc_dir_entry *);
    kfree(kernel_buffer); // Free the allocated memory

    printk("pyjama_module_exit: exit\n");
}

module_init (pyjama_module_init);
module_exit (pyjama_module_exit);
