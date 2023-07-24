#ifndef __MPU_6050_H__
#define __MPU_6050_H__

struct accel_data
{
	unsigned short x;
	unsigned short y;
	unsigned short z;
};
struct gyro_data
{
	unsigned short x;
	unsigned short y;
	unsigned short z;
};
union mpu6050_data
{
	struct accel_data accel;
	struct gyro_data gyro;
	unsigned short temp;
};

#define MPU6050_MAGIC 'K'

#define GET_ACCEL _IOR(MPU6050_MAGIC,0,union mpu6050_data)
#define GET_GYRO _IOR(MPU6050_MAGIC,1,union mpu6050_data)
#define GET_TEMP _IOR(MPU6050_MAGIC,2,union mpu6050_data)



#endif
