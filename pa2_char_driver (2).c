#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h> 
#include <linux/slab.h> 

MODULE_AUTHOR("Angel");
MODULE_LICENSE("GPL");

#define DEVICE_NAME "simple_character_device"
#define BUFFER_SIZE 1024

static int Major = 240; 
static char *device_buffer; 

int open_func(struct inode *inode, struct file *filep); 
int close_func(struct inode *inode, struct file *filep); 
ssize_t read_func(struct file *filep, char __user *user_buff, size_t buff_size, loff_t *offset);
ssize_t write_func(struct file *filep, const char __user *user_buff, size_t buff_size, loff_t *offset); 
loff_t seek_func(struct file *filep, loff_t offset, int whence); 


static struct file_operations my_op = {
	.owner = THIS_MODULE, 
	.open = open_func, //int open_func(struct inode *, struct file *);
	.release = close_func, //int close_func(struct inode *, struct file *); 
	.read = read_func, //ssize_t read_func(struct file *, char __user *, size_t, loff_t *); 
	.write = write_func, //ssize_t write_func(struct file *, const char __user *, size_t, loff_t *); 
	.llseek = seek_func //loff_t seek_func(struct file *, loff_t, int); 

};

static int pa2_char_driver_init(void){
    int res = register_chrdev(Major, DEVICE_NAME, &my_op); 
    if(res < 0){
        printk(KERN_ALERT "Register character device failed with Major Number: %d\n", Major); 
        return res; 
    }
    device_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    printk(KERN_ALERT "Registering character device...\n");
    return 0;
}

static void pa2_char_driver_exit(void){

    printk(KERN_ALERT "Unregistering character device...\n");
    kfree(device_buffer); 
    unregister_chrdev(Major, DEVICE_NAME);
}

int open_func(struct inode *inode, struct file *filep){
    static int num_open = 0; 
    printk(KERN_ALERT "Device %d opened \n", Major); 
    printk(KERN_ALERT "Number of times opened: %d\n", ++num_open);
    return 0; 
}

int close_func(struct inode *inode, struct file *filep){
    static int num_close = 0; 
    printk(KERN_ALERT "Device %d closed \n", Major); 
    printk(KERN_ALERT "Number of times closed: %d\n", ++num_close);
    return 0; 
}

ssize_t read_func(struct file *filep, char __user *user_buff, size_t buff_size, loff_t *offset){
    if(buff_size + *offset > BUFFER_SIZE || buff_size + *offset < 0){
        printk(KERN_ALERT "ERROR: Read failed, out of bounds.\n"); 
        return -1; 
    }
    if(copy_to_user(user_buff, device_buffer + *offset, buff_size) != 0){
        printk(KERN_ALERT "ERROR: Read failed \n"); 
        return -1; 
    }
    else{
        printk(KERN_ALERT "Number of bytes read: %lu\n", buff_size); 
        *offset = buff_size + *offset; 
    }
    return buff_size; 
}

ssize_t write_func(struct file *filep, const char __user *user_buff, size_t buff_size, loff_t *offset){
     if(buff_size + *offset > BUFFER_SIZE || buff_size + *offset < 0){
        printk(KERN_ALERT "ERROR: Write failed, out of bounds.\n"); 
        return -1; 
    }
    if(copy_from_user(device_buffer + *offset, user_buff, buff_size) != 0){
        printk(KERN_ALERT "ERROR: Write failed\n"); 
        return -1; 
    }
    else{
         printk(KERN_ALERT "Number of bytes written: %lu\n", buff_size); 
        *offset = buff_size + *offset;    
    }
    return buff_size; 
}

loff_t seek_func(struct file *filep, loff_t offset, int whence){
    loff_t new_position; 
    switch(whence){
        case 0: /*seek_set*/
            if(offset < 0 || offset > BUFFER_SIZE){
                printk(KERN_ALERT "ERROR: SEEK_SET out of bounds\n"); 
                return -1; 
            }
            new_position = offset; 
            break;
        case 1: /*seek_cur*/
            if(filep -> f_pos + offset > BUFFER_SIZE || filep -> f_pos + offset < 0){
                printk(KERN_ALERT "ERROR: SEEK_CUR out of bounds.\n"); 
                return -1; 
            }
            new_position = filep -> f_pos + offset; 
            break; 
        case 2: /*seek_end*/
            if(offset > 0 || BUFFER_SIZE + offset < 0){
                printk(KERN_ALERT "ERROR: SEEK_END out of bounds.\n"); 
                return -1; 
            }
            new_position = BUFFER_SIZE + offset; 
            break;
        default: 
            printk(KERN_ALERT "Invalid whence, please enter only 0, 1, or 2"); 
            printk(KERN_ALERT "You entered: %d\n",whence);
            return -1; 
    }
    filep -> f_pos = new_position; 
    return 0; 
}

module_init(pa2_char_driver_init); 
module_exit(pa2_char_driver_exit); 


