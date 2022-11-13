#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
#include<linux/uaccess.h> 
#include "kernel_module.h"
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/sched.h>
#include <linux/export.h>
#include <linux/pid.h>


#define FIRST_MINOR 0
#define MINOR_CNT 1

static dev_t dev;
static struct cdev c_dev;
static struct class *cl;

static int pid_in;
module_param(pid_in, int, S_IRUGO);

static unsigned long get_pfn(int process_ID_In, unsigned long vpn_in){
        struct pid *pid;
        struct task_struct *pid_struct;
        struct mm_struct *pid_mm_struct;
  
        unsigned long vaddr;
       

        pgd_t *pgd;
        pud_t *pud;
        pmd_t *pmd;
        pte_t *ptep;
        unsigned long pfn;


        pid = find_get_pid(process_ID_In);
        if(pid == NULL){
                printk("pid does not exist.\n");
                return -1;
        }
        pid_struct = pid_task(pid, PIDTYPE_PID);
        pid_mm_struct = pid_struct->mm;

        vaddr = vpn_in;
        pgd = pgd_offset(pid_mm_struct, vaddr);
        if(pgd_none(*pgd)){
                printk("not mapped in pgd\n");
                return 0;
        }
        pud = pud_offset(pgd, vaddr);
        if(pud_none(*pud)){
                printk("not mapped in pud\n");
                return 0;
        }
        pmd = pmd_offset(pud, vaddr);
        if(pmd_none(*pmd)){
                printk("not mapped in pmd\n");
                return 0;
        }
        ptep = pte_offset_map(pmd, vaddr);
        if(pte_none(*ptep)){
                printk("not mapped in pte\n");
                return 0;
        }
        if(pte_present(*ptep)){
                 
                 

                pfn =  pte_pfn(*ptep);
                
                return pfn;
        }
        return 0;  
}
static int my_open(struct inode *i, struct file *f)
{
        return 0;
}
static int my_close(struct inode *i, struct file *f)
{
        return 0;
}
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
static int my_ioctl(struct inode *i, struct file *f, unsigned int cmd, unsigned long arg)
#else
static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
#endif
{
        query_arg_t q;

        switch (cmd)
        {
                case IOCTL_GET_PFN:
                        if (copy_from_user(&q, (query_arg_t *)arg, sizeof(query_arg_t)))
                        {
                                return -EACCES;
                        }
                        printk("In the my_ioctl.\n");
                        q.pfn = get_pfn(pid_in, q.vpn);
                        if (copy_to_user((query_arg_t *)arg, &q, sizeof(query_arg_t)))
                        {
                                return -EACCES;
                        }
                        break;
                default:
                        return -EINVAL;
        }

        return 0;
}


static struct file_operations query_fops =
{
        .owner = THIS_MODULE,
        .open = my_open,
        .release = my_close,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
        .ioctl = my_ioctl
#else
                .unlocked_ioctl = my_ioctl
#endif
};

static int __init query_ioctl_init(void)
{
        int ret;
        struct device *dev_ret;


        if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, "query_ioctl")) < 0)
        {
                return ret;
        }

        cdev_init(&c_dev, &query_fops);

        if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0)
        {
                return ret;
        }

        if (IS_ERR(cl = class_create(THIS_MODULE, "char")))
        {
                cdev_del(&c_dev);
                unregister_chrdev_region(dev, MINOR_CNT);
                return PTR_ERR(cl);
        }
        if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "query")))
        {
                class_destroy(cl);
                cdev_del(&c_dev);
                unregister_chrdev_region(dev, MINOR_CNT);
                return PTR_ERR(dev_ret);
        }

        return 0;
}

static void __exit query_ioctl_exit(void)
{
        device_destroy(cl, dev);
        class_destroy(cl);
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
}

module_init(query_ioctl_init);
module_exit(query_ioctl_exit);





MODULE_DESCRIPTION("Virtual to Physical Address Mapping");
MODULE_AUTHOR("Roja Eswaran <reswara1@binghamton.edu>");
MODULE_LICENSE("GPL");
