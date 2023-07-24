#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>


#include <stdio.h>

#include "mpu6050.h"

int main(int argc,char **argv)
{
	int fd;
	union mpu6050_data data;
	if(argc <2 ){
		printf("The argument is too few\n");
		return 1;
	} 
	fd = open(argv[1],O_RDONLY);
	if(fd < 0){
		printf("open %s failed\n",arhv[1]);
		return 2;
	}

	while(1){
		sleep(2);

		ioctl(fd,GET_ACCEL,&data);
		printf("Accel-x=0x%x\n",data.accel.x);
		printf("Accel-y=0x%x\n",data.accel.y);
		printf("Accel-z=0x%x\n",data.accel.z);

		
		ioctl(fd,GET_GYRO,&data);
		printf("gyro-x=0x%x\n",data.gyro.x);
		printf("gyro-y=0x%x\n",data.gyro.y);
		printf("gyro-z=0x%x\n",data.gyro.z);

		ioctl(fd,GET_TEMP,&data);
		printf("Accel-x=0x%x\n",data.accel.x);
		printf("Temp-x=0x%x\n",data.temp);

		printf("\n");
		
	
	}
	close(fd);
	fd = -1;
	return 0;
}

