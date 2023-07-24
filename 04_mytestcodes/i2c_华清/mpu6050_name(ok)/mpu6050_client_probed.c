/* 这个是不知道从机地址的*/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>

static unsigned short mpu6050_addr_list[] =//内核匹配的地址列表
{
	0x68,
	0x69,
	I2C_CLIENT_END,//必须要有这一项代表已经枚举完了
};
static struct i2c_client *gpmpu6050_client = NULL;

static int __init mpu6050_client_init(void)
{
	struct i2c_adapter *padp = NULL;
	struct i2c_board_info mpu6050_info = {""};//内核找完自己填进去
	//board里面有名字地址列表里面有地址设备信息就完整了
	
	strcpy(mpu6050_info.type,"mpu6050");
	//strscpy(i2c_info.type,"ap3216c",sizeog(i2c_info.type));
	padp = i2c_get_adapter(5);
	gpmpu6050_client = i2c_new_probed_device(padp,&mpu6050_info,mpu6050_addr_list,NULL);//让内核自己帮我们去匹配寻找
																						//probed去探测哪个能走通，通过i2c通讯看看通道5所对应的哪个地址能走通
	i2c_put_adapter(padp);
	if(gpmpu6050_client != NULL){
		return 0;
	}else{
		return -ENODEV;
	}
	
}
static void mpu6050_client_exit(void)
{
	i2c_unregister_device(gpmpu6050_client);
}

module_init(mpu6050_client_init);
module_exit(mpu6050_client_exit);
MODULE_LICENSE("GPL");









