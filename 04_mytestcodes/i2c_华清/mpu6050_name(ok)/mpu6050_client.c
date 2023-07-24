/* 这个是直接知道从机地址的，不要寻找匹配 */
/* driver 是“怎么”跟实实在在的硬件打交道，就是驱动硬件，怎么驱动呢？寄存器等设置 */
/* client 是存储设备信息，有哪些信息呢？就是一些外在的身份标识，例如一个人的学号名字，其实本质就是一个设备树 */
/* 而driver就是一个人的内在设置      */
/* 跟硬件“直接”打交道的是adapter*/
/* 所以client和driver合称为设备-驱动模型 */
/* 而i2c驱动是i2c_adapter */
#include <linux/kernel.h> 
//#include <linux/module.h>
#include <linux/i2c.h>
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
static struct i2c_board_info mpu6050_info = //我们匹配的设备信息
{
	I2C_BOARD_INFO("mpu6050",0x68)//一定要与driver中的ids中的名字一样！
};

static struct i2c_client *gpmpu6050_client = NULL;

static int __init mpu6050_client_init(void)
{
	struct i2c_adapter *padp = NULL;

	padp = i2c_get_adapter(5);//获取client对应的二级外设挂载在哪个通道上？5-》的得到通道锁对应的adapter对象
							//这里边有个引用计数会+1
	//想在第5个i2c adapter下面创建一个设备，这个设备的信息就在mpu6050_info里面
	gpmpu6050_client = i2c_new_device(padp,&mpu6050_info);//指定从机属性，知道从机地址
	i2c_put_adapter(padp);//相应的这里就要-1，用完之后把他释放掉
	return 0;
}
static void mpu6050_client_exit(void)
{
	i2c_unregister_device(gpmpu6050_client);
}

module_init(mpu6050_client_init);
module_exit(mpu6050_client_exit);
MODULE_LICENSE("GPL");
