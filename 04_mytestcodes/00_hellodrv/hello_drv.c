#include <linux/module.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>

/*1.确定主设备号*/
static int major = 0;

/*3.1.*/
static char kernel[1024];
#define MIN(a,b) (a<b?a:b)//总是填入最小值
/*5.1.*/
static struct class *hello_class;


/*3.对操作函数集进行实现*/
static ssize_t hello_drv_open(struct inode *node,struct file *file)
{
	printk("%s %s line %d\n",__FILE__,__FUNCTION__,__LINE__);
	return 0;
}

static ssize_t hello_drv_read(struct file *file,char __user *buf,size_t size,loff_t *offset)
{
	int err;
	printk("%s %s line %d\n",__FILE__,__FUNCTION__,__LINE__);
	err = copy_to_user(buf,kernel_buf,MIN(1024,size));
	return MIN(1024,size);
}

static ssize_t hello_drv_write(struct file *file,const char __user *buf,size_t size,loff_t *offset)
{
	int err;
	printk("%s %s line %d\n",__FILE__,__FUNCTION__,__LINE__);
	err = copy_from_user(kernel_buf, buf, MIN(1024,size));
	return MIN(1024,size);
}

static ssize_t hello_drv_close(struct inode *node,struct file *file)
{
	printk("%s %s line %d\n",__FILE__,__FUNCTION__,__LINE__);
	return 0;
}




/*2.定义自己的file_operations结构体*/
static struct file_operations hello_drv = {
	.owner    = THIS_MODULE,
	.open     = hello_drv_open,
	.read     = hello_drv_read,
	.write    = hello_drv_write,
	.release  = hello_drv_close,
};

/*4.把file_operations结构体告诉内核：注册驱动程序*/
/*5.谁来注册驱动程序呢？要有一个入口函数：安装驱动程序的时候就会调用这个入口函数*/
static int __init hello_init(void)//通常这里的取名与设备结点有关
{
	int err;
	printk("%s %s line %d\n",__FILE__,__FUNCTION__,__LINE__);
	major = register_chrdev(0,"hello",&hello_drv);/* 注册/dev/hello */

	hello_class = class_create(THIS_MODULE,"hello_class");
	err = PTR_ERR(hello_class);
	if(IS_ERR(hello_class)){//如果出错，打印错误信息，注销设备，归还设备结点
		printk("%s %s line %d\n",__FILE__,__FUNCTION__,__LINE__);
		unregister_chrdev(major,"hello");
		return -1;
	}
	device_create(hello_class,NULL,MKDEV(major,0),NULL,"hello");/* 生成/dev/hello  */
	return 0;
	
}
/*6.有入口函数就应该有出口函数：卸载驱动程序的时候就会去调用这个出口函数*/
static void __exit hello_exit(void)//通常这里的取名与设备结点有关
{
	printk("%s %s line %d\n",__FILE__,__FUNCTION__,__LINE__);
	device_destroy(hello_class,MKDEV(major,0));
	class_destroy(hello_class);
	unregister_chrdev(major,"hello");
}

/*7.其他完善：提供设备信息，自动创建设备结点*/
module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");


