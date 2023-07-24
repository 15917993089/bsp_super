#include <stdio.h>
#include "mpu6050.h"
static int read_data_from_mpu6050(int fd,unsigned char slave,unsigned char reg,unsigned char *pdata){
	struct i2c_rdwr_ioctl_data work = {NULL};
	int ret;

	struct i2c_msg msgs[2] = {{0}};
	unsigned char buf1[1] = {reg};
	unsigned char buf2[2] = {0};

	work.msgs = msgs;
	work.nmsgs = 2;
	
	msgs[0].addr = slave;
	msgs[0].flags = 0;
	msgs[0].buf = buf1;
	msgs[0].len = 1;

	msgs[1].addr = slave;
	msgs[1].flags = I2C_M_RD;
	msgs[1].buf = buf2;
	msgs[1].len = 1;
	
	ret = ioctl(fd,I2C_RDWR,&work);
	if(ret < 0){
		printf("ioctl I2C_RDWR failed,in read_data_from_mpu6050\n");
		return -1;
	}else{
		*pdata = buf2[0];
		return 0;
	}
	return 0;
}
static int write_data_to_mpu6050(int fd,unsigned char slave,unsigned char reg,unsigned char data){
	struct i2c_rdwr_ioctl_data work={NULL};
	int ret;

	struct i2c_msg msg = {0};
	unsigned char buf[2] = {reg,data};

	work.msgs = &msg;
	work.nmsgs = 1;
	
	msg.addr = slave;
	msg.flags = 0;
	msg.buf = buf;
	msg.len = 2;

	ret = ioctl(fd,I2C_RDWR,&work);
	if(ret < 0){
		printf("ioctl I2C_RDWR failed,in write_data_to_mpu6050\n");
		return 0;
	}else{
		return 0;
	}
}
int init_mpu6050(int fd){
	int ret = 0;
	ret = ioctl(fd,I2C_TENBIT,0);
	if(ret < 0){
		printf("ioctl I2C_TENBIT failed,in init_mpu6050\n");
		return -1;
	}

	ret = ioctl(fd,I2C_SLAVE,0x68);
	if(ret < 0){
		printf("ioctl I2C_SLAVE failed,in init_mpu6050\n");
		return -1;
	}
	ret = write_data_to_mpu6050(fd,0x68,PWR_MGMT_1,0x00);
	ret = write_data_to_mpu6050(fd,0x68,SMPLRT_DIV,0x07);
	ret = write_data_to_mpu6050(fd,0x68,ACCEL_CONFIG,0x19);
	ret = write_data_to_mpu6050(fd,0x68,GYRO_CONFIG,0xF8);
	if(ret < 0){
		printf("write inin data to mpu6050 failed,in inint_mpu6050\n");
		return -1;
	}
	return 0;
}

int read_accelx(int fd){
	unsigned short val =0;
	unsigned char d = 0;
	int ret = 0;

	ret = read_data_from_mpu6050(fd,0x68,ACCEL_XOUT_L,&d);
	val = d;
	ret = read_data_from_mpu6050(fd,0x68,ACCEL_XOUT_H,&d);
	val |= d << 8;
	if(ret < 0){
		printf("raed accel x value failed,in read_accelx\n");
		return -1;
	}else{
		return val;
	}
	return 0;
}

int read_accely(int fd){
	unsigned short val =0;
	unsigned char d = 0;
	int ret = 0;

	ret = read_data_from_mpu6050(fd,0x68,ACCEL_YOUT_L,&d);
	val = d;
	ret = read_data_from_mpu6050(fd,0x68,ACCEL_YOUT_H,&d);
	val |= d << 8;
	if(ret < 0){
		printf("raed accel y value failed,in read_accely\n");
		return -1;
	}else{
		return val;
	}
	return 0;
}

int read_accelz(int fd){
	unsigned short val =0;
	unsigned char d = 0;
	int ret = 0;

	ret = read_data_from_mpu6050(fd,0x68,ACCEL_ZOUT_L,&d);
	val = d;
	ret = read_data_from_mpu6050(fd,0x68,ACCEL_ZOUT_H,&d);
	val |= d << 8;
	if(ret < 0){
		printf("raed accel z value failed,in read_accelz\n");
		return -1;
	}else{
		return val;
	}
	return 0;
}

int read_temp(int fd){
	unsigned short val =0;
	unsigned char d = 0;
	int ret = 0;

	ret = read_data_from_mpu6050(fd,0x68,TEMP_OUT_L,&d);
	val = d;
	ret = read_data_from_mpu6050(fd,0x68,TEMP_OUT_H,&d);
	val |= d << 8;
	if(ret < 0){
		printf("raed accel temp value failed,in read_temp\n");
		return -1;
	}else{
		return val;
	}
	return 0;
}

int read_gyrox(int fd){
	unsigned short val =0;
	unsigned char d = 0;
	int ret = 0;

	ret = read_data_from_mpu6050(fd,0x68,GYRO_XOUT_L,&d);
	val = d;
	ret = read_data_from_mpu6050(fd,0x68,GYRO_XOUT_H,&d);
	val |= d << 8;
	if(ret < 0){
		printf("raed gyro x value failed,in read_gyrox\n");
		return -1;
	}else{
		return val;
	}
	return 0;
}
int read_gyroy(int fd){
	unsigned short val =0;
	unsigned char d = 0;
	int ret = 0;

	ret = read_data_from_mpu6050(fd,0x68,GYRO_YOUT_L,&d);
	val = d;
	ret = read_data_from_mpu6050(fd,0x68,GYRO_YOUT_H,&d);
	val |= d << 8;
	if(ret < 0){
		printf("raed gyro y value failed,in read_gyroy\n");
		return -1;
	}else{
		return val;
	}
	return 0;
}
int read_gyroz(int fd){
	unsigned short val =0;
	unsigned char d = 0;
	int ret = 0;

	ret = read_data_from_mpu6050(fd,0x68,GYRO_ZOUT_L,&d);
	val = d;
	ret = read_data_from_mpu6050(fd,0x68,GYRO_ZOUT_H,&d);
	val |= d << 8;
	if(ret < 0){
		printf("raed gyro z value failed,in read_gyroz\n");
		return -1;
	}else{
		return val;
	}
	return 0;
}
