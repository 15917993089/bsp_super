//使用全局指针避免使用全局变量
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/cdev.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/io.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>

#include "mpu6050.h"

#define SMPLRT_DIV 0x19
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C

#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H 0x41
#define TEMP_OUT_L 0x42
#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48

#define PWR_MGMT_1  0x6B

int major = 11;
int minor = 0;
int mpu6050_num = 1;//mpu6050的个数


struct mpu6050_dev
{
	struct cdev mydev;
	struct i2c_client *pctl;//定义全局变量记录client！！下面的函数形参传递之后记录下来
};

struct mpu6050_dev *pgmydev = NULL;

int mpu6050_read_byte(struct i2c_client *pctl,unsigned char reg)//读数据
{
	int ret;
	char txbuf[1] = {reg};
	char rxbuf[1] = {0};

	struct i2c_msg msg[2] = 
	{
		{pctl->addr,0,1,txbuf},//写寄存器地址
		{pctl->addr,I2C_M_RD,1,rxbuf},//从寄存器中读出数据
	};
	ret = i2c_transfer(pctl->adapter,msg,ARRAY_SIZE(msg));//将要发送出去的信息结构体发送出去
															//每个adapter代表一个i2c通道
	if(ret < 0){
		printk("ret = %d,in mpu6050_read_byte\n",ret);
		return ret;
	}
	return rxbuf[0];
}

int mpu6050_write_byte(struct i2c_client *pctl,unsigned char reg,unsigned char val)//写配置
{
	int ret;
	char txbuf[2] = {reg,val};

	struct i2c_msg msg[1] = 
	{
		{pctl->addr,0,2,txbuf},
	};

	ret = i2c_transfer(pctl->adapter,msg,ARRAY_SIZE(msg));
	if(ret < 0){
		printk("ret = %d,in mpu6050_write_byte\n",ret);
		return ret;
	}

	return 0;
	
}

int mpu6050_open(struct inode *pnode,struct file *pfile)
{
	pfile->private_data = (void *)(container_of(pnode->i_cdev,struct mpu6050_dev,mydev));//避免使用全局变量!
	return 0;
}
int mpu6050_close(struct inode *pnode,struct file *pfile)
{
	return 0;	
}

long mpu6050_ioctl(struct file *pfile,unsigned int cmd,unsigned long arg)
{
	struct mpu6050_dev *pmydev = (struct mpu6050_dev *)pfile->private_data;
	union mpu6050_data data;

	switch(cmd)
	{
		case GET_ACCEL:
			data.accel.x = mpu6050_read_byte(pmydev->pctl,ACCEL_XOUT_L);
			data.accel.x = mpu6050_read_byte(pmydev->pctl,ACCEL_XOUT_H) << 8;

			data.accel.y = mpu6050_read_byte(pmydev->pctl,ACCEL_YOUT_L);
			data.accel.y = mpu6050_read_byte(pmydev->pctl,ACCEL_YOUT_H) << 8;

			data.accel.z = mpu6050_read_byte(pmydev->pctl,ACCEL_ZOUT_L);
			data.accel.z = mpu6050_read_byte(pmydev->pctl,ACCEL_ZOUT_H) << 8;
			break;
		case GET_GYRO:
			data.gyro.x = mpu6050_read_byte(pmydev->pctl,GYRO_XOUT_L);
			data.gyro.x = mpu6050_read_byte(pmydev->pctl,GYRO_XOUT_H) << 8;

			data.gyro.y = mpu6050_read_byte(pmydev->pctl,GYRO_YOUT_L);
			data.gyro.y = mpu6050_read_byte(pmydev->pctl,GYRO_YOUT_H) << 8;

			data.gyro.z = mpu6050_read_byte(pmydev->pctl,GYRO_ZOUT_L);
			data.gyro.z = mpu6050_read_byte(pmydev->pctl,GYRO_ZOUT_H) << 8;
			break;
		case GET_TEMP:
			data.temp = mpu6050_read_byte(pmydev->pctl,TEMP_OUT_L);
			data.temp = mpu6050_read_byte(pmydev->pctl,TEMP_OUT_H) << 8;
			break;
		default:
			return -EINVAL;
	}
	if(copy_to_user((void *)arg,&data,sizeof(data))){
		return -EFAULT;
	}
	return sizeof(data);
}
void init_mpu6050(struct i2c_client *pctl)
{
	mpu6050_write_byte(pctl,PWR_MGMT_1,0x00);
	mpu6050_write_byte(pctl,SMPLRT_DIV,0x07);
	mpu6050_write_byte(pctl,CONFIG,0x06);
	mpu6050_write_byte(pctl,GYRO_CONFIG,0xF8);
	mpu6050_write_byte(pctl,ACCEL_CONFIG,0x19);
}

struct file_operations myops = {
	.owner = THIS_MODULE,
	.open = mpu6050_open,
	.release = mpu6050_close,
	.unlocked_ioctl = mpu6050_ioctl,
		
};

//初始化i2c的二级外设
static int mpu6050_probe(struct i2c_client *pclt,const struct i2c_device_id *pid)
{
	int ret = 0;
	dev_t devno = MKDEV(major,minor);

	/* 申请设备号 */
	ret = register_chrdev_region(devno,mpu6050_num,"mpu6050");
	if(ret)//返回为1就是出错！
	{
		ret = alloc_chrdev_region(&devno,minor,mpu6050_num,"mpu6050");//出错让系统自动帮我们申请一个

		if(ret)//再次返回1就是又出错了
		{
			printk("get devno failed\n");
			return -1;
		}
		major = MAJOR(devno);//申请出错要把设备号归还
	}

	/* 给struct cdev 对象指定操作函数集 */
	pgmydev = (struct mpu6050_dev *)kmalloc(sizeof(struct mpu6050_dev),GFP_KERNEL);//因为是指针形式所以要申请空间
																					//如果是变量形式就不用！
	if(NULL == pgmydev)
	{
		unregister_chrdev_region(devno,mpu6050_num);
		printk("kmalloc failed\n");
		return -1;
	}
	memset(pgmydev,0,sizeof(struct mpu6050_dev));
	pgmydev->pctl = pclt;//①//定义全局变量记录client！！下面的函数形参传递之后记录下来，就是在这里
	cdev_init(&pgmydev->mydev,&myops);//给struct cdev 对象指定操作函数集
	
	/* 将 struct cdev 对象添加到内核对应的数据结构里 */
	pgmydev->mydev.owner = THIS_MODULE;
	cdev_add(&pgmydev->mydev,devno,mpu6050_num);
	//硬件相关操作
	init_mpu6050(pgmydev->pctl);//②把二级外设也加载一下
	return 0;
}
static int mpu6050_remove(struct i2c_client *pclt)
{
	dev_t devno = MKDEV(major, minor);
	cdev_del(&pgmydev->mydev);
	unregister_chrdev_region(devno,mpu6050_num);//归还设备号，再此之前还要从内核中注销设备

	kfree(pgmydev);
	pgmydev = NULL;

	return 0;
}

struct i2c_device_id mpu6050_ids[] =
{
	{"mpu6050",0},
	{}
};

struct i2c_driver mpu6050_driver = //使用全局变量的的形式来定义一个i2c_driver的对象
{
	.driver = {
		.owner = THIS_MODULE,
		.name = "mpu6050",//方便cilent根据优先级匹配寻找，优先级第二
	},
	.probe = mpu6050_probe,
	.remove = mpu6050_remove,
	.id_table = mpu6050_ids,//方便cilent根据优先级匹配寻找，优先级第一
};

#if 0
int __init mpu6050_driver_init(void)
{
	i2c_add_driver(&mpu6050_driver);
}

void __exit mpu6050_driver_exit(void) 
{
	i2c_del_driver(&mpu6050_driver);
}
module_init(mpu6050_driver_init);
module_exit(mpu6050_driver_exit);
#else
module_i2c_driver(mpu6050_driver);
#endif

MODULE_LICENSE("GPL");











