#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

/*
 *./hello_drv_test -w abc
 *./hello_drv_test -r
 */

int main(int argc,char *argv[])
{
	int fd;
	char buf[1024];
	int len;
	/*1.参数判断*/
	if(argc < 2){
		printf("Usage:%s -w <string>\n",argv[0]);
		printf("	  %s -r",argv[0]);
		return -1;
	}
	/*2.打开文件*/
	fd = open("/dev/hello",O_RDWR);
	if(fd == -1){
		printf("can not open file /dev/hello\n");
		return-1;
	}
	/*3.写文件或者读文件*/
	if((0 == strcmp(argv[1],"-w")) && (argc == 3)){
		len = strlen(arhv[2]) + 1;
		len = len < 1024 ? len : 1024;
		write(fd, argv[2], len);
	}else{
		len = read(fd,buf,1024);
		buf[1023] = "\0";
		printf("APP read : %s\n",buf);
	}
	close(fd);
	return 0;
}


