/* 参考: tools\spi\spidev_fdx.c */
/* 关键在于如何构造数据和得到数据之后如何解析 */
/* 自己编写驱动程序之后不需要我在用于程序里面对数据做任何处理写应用的人
*	不需要去操心硬件的底层实现
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <linux/types.h>

#define SPI_IOC_WR 456

/* dac_test /dev/100ask_dac <val> */

int main(int argc, char **argv)
{
	int fd;
	unsigned int val;
	//struct spi_ioc_transfer	xfer[1];//要设置这个值传输
	int	status;

	unsigned char tx_buf[2];
	unsigned char rx_buf[2];
	
	if(argc != 3){
		printf("Usage: %s /dev/spidevB.D <val>\n", argv[0]);
		return 0;
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		//perror("open");
		printf("Can not open %s\n", argv[1]);
		return 1;	
	}
	val = strtoul(argv[2], NULL, 0);
#if 0
	//以前我们要看芯片手册，对这些数据进行处理
	//现在交给驱动去处理就可以了
	//transfer也不需要构造了
	val <<= 2;	/* bit0,bit1 = 0b00 */
	val &=0xFFC;	/* 只保留10bit */

	tx_buf[1] = val & 0xff;	//低位
	tx_buf[1] = (val >> 8) & 0xff;	//高位。到这里就构造好了数据
	memset(xfer, 0, sizeof xfer);
	//struct spi_ioc_transfer xfer[1];//要设置这个值传输
	xfer[0].tx_buf = tx_buf;	//发送数据
	xfer[0].rx_buf = rx_buf;	//读到数据
	xfer[0].len = 2;	//发送和接受的长度都是2
#else
#endif
	status = ioctl(fd, SPI_IOC_WR, &val);
	if (status < 0) {
		//perror("SPI_IOC_MESSAGE");
		printf("SPI_IOC_WR FAILED\n");
		return -1;
	}
#if 0

	/* 打印 */
	val = (rx_buf[0] << 8) | (rx_buf[1]);
	val >>= 2;
#else
#endif

	printf("Pre val = %d\n", val);
	

	return 0;
}


