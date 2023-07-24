#include <stdio.h>
#include "mpu6050.h"
int main(int argc,char *argv[]){
	int fd=-1;
	if(argc<2){
		printf("Argument is too few\n");
		return 1;
	}
	/*open*/
	fd = open(argv[1],O_RDWR);
	if(fd<0){
		printf("open %s failed\n",argv[1]);
		return 2;
	}
	/*init mpu6050*/
	init_mpu6050(fd);

	while(1){
		sleep(2);
		/*read and print data from 6050*/
		printf("Accel-X:0x%x\n",read_accelx(fd));
		printf("Accel-Y:0x%x\n",read_accely(fd));
		printf("Accel-Z:0x%x\n",read_accelz(fd));
		printf("Temp:0x%x\n",read_temp(fd));
		printf("GYRO-X:0x%x\n",read_gyrox(fd));
		printf("GYRO-Y:0x%x\n",read_gyroy(fd));
		printf("GYRO-Z:0x%x\n",read_gyroz(fd));
	}
	/*close*/
	close(fd);
	fd=-1;
	return 0;
}
