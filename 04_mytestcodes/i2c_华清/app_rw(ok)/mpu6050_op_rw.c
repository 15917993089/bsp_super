#include <stdio.h>
#include "mpu6050.h"
static int read_data_from_mpu6050(int fd,unsigned char reg,unsigned char *pdata){
	int ret=0;
	unsigned char buf[1] = {reg};

	ret = write(fd,buf,1);
	if(ret !=1){
		printf("write reg failed,in read_data_from_mpu6050\n");
		return -1;
	}

	buf[0]=0;
	ret = read(fd,buf,1);
	if(ret != 1){
		printf("read data failed,in read_data_from_mpu6050\n");
		return -1;
	}
	*pdata = buf[0];
	return 0;
}
static int write_data_to_mpu6050(int fd,unsigned char reg,unsigned char data){
	unsigned char buf[2] = {reg,data};
	int ret = 0;

	ret = write(fd,buf,2);
	if(ret != 2){
		printf("write data failed,in write_data_to_mpu6050\n");
		return -1;
	}
	return 0;
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
	ret = write_data_to_mpu6050(fd,PWR_MGMT_1,0x00);
	ret = write_data_to_mpu6050(fd,SMPLRT_DIV,0x07);
	ret = write_data_to_mpu6050(fd,ACCEL_CONFIG,0x19);
	ret = write_data_to_mpu6050(fd,GYRO_CONFIG,0xF8);
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

	ret = read_data_from_mpu6050(fd,ACCEL_XOUT_L,&d);
	val = d;
	ret = read_data_from_mpu6050(fd,ACCEL_XOUT_H,&d);
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

	ret = read_data_from_mpu6050(fd,ACCEL_YOUT_L,&d);
	val = d;
	ret = read_data_from_mpu6050(fd,ACCEL_YOUT_H,&d);
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

	ret = read_data_from_mpu6050(fd,ACCEL_ZOUT_L,&d);
	val = d;
	ret = read_data_from_mpu6050(fd,ACCEL_ZOUT_H,&d);
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

	ret = read_data_from_mpu6050(fd,TEMP_OUT_L,&d);
	val = d;
	ret = read_data_from_mpu6050(fd,TEMP_OUT_H,&d);
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

	ret = read_data_from_mpu6050(fd,GYRO_XOUT_L,&d);
	val = d;
	ret = read_data_from_mpu6050(fd,GYRO_XOUT_H,&d);
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

	ret = read_data_from_mpu6050(fd,GYRO_YOUT_L,&d);
	val = d;
	ret = read_data_from_mpu6050(fd,GYRO_YOUT_H,&d);
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

	ret = read_data_from_mpu6050(fd,GYRO_ZOUT_L,&d);
	val = d;
	ret = read_data_from_mpu6050(fd,GYRO_ZOUT_H,&d);
	val |= d << 8;
	if(ret < 0){
		printf("raed gyro z value failed,in read_gyroz\n");
		return -1;
	}else{
		return val;
	}
	return 0;
}
