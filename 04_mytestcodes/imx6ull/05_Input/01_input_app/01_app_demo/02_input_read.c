
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


/* ./01_get_input_info /dev/input/event0 noblock */
int main(int argc, char **argv)
{
	int fd;
	int err;
	int len;
	int i;
	unsigned char byte;
	int bit;
	struct input_id id;
	unsigned int evbit[2];
	struct input_event event;
	
	char *ev_names[] = {
		"EV_SYN ",
		"EV_KEY ",
		"EV_REL ",
		"EV_ABS ",
		"EV_MSC ",
		"EV_SW	",
		"NULL ",
		"NULL ",
		"NULL ",
		"NULL ",
		"NULL ",
		"NULL ",
		"NULL ",
		"NULL ",
		"NULL ",
		"NULL ",
		"NULL ",
		"EV_LED ",
		"EV_SND ",
		"NULL ",
		"EV_REP ",
		"EV_FF	",
		"EV_PWR ",
		};
	
	if (argc < 2)
	{
		printf("Usage: %s <dev> [noblock]\n", argv[0]);
		return -1;
	}

	if (argc == 3 && !strcmp(argv[2], "noblock"))
	{
		fd = open(argv[1], O_RDWR | O_NONBLOCK);//改变默认的特性
	}
	else
	{
		fd = open(argv[1], O_RDWR);//默认是阻塞！
	}
	if (fd < 0)
	{
		printf("open %s err\n", argv[1]);
		return -1;
	}

	err = ioctl(fd, EVIOCGID, &id);//把地址传给这里，才可以去设置地址里面的内容！
	if (err == 0)
	{
		printf("bustype = 0x%x\n", id.bustype );//打印地址里面的内容
		printf("vendor	= 0x%x\n", id.vendor  );
		printf("product = 0x%x\n", id.product );
		printf("version = 0x%x\n", id.version );
	}

	len = ioctl(fd, EVIOCGBIT(0, sizeof(evbit)), &evbit);//把地址传给这里，才可以去设置地址里面的内容！
	if (len > 0 && len <= sizeof(evbit))//表明读到了数据
	{
		printf("support ev type: ");
		for (i = 0; i < len; i++)
		{
			byte = ((unsigned char *)evbit)[i];//设置地址里面的内容
			for (bit = 0; bit < 8; bit++)
			{
				if (byte & (1<<bit)) {//哪一位为1表示哪一位去除哪一位，
										
					printf("%s ", ev_names[i*8 + bit]);//这种做法和lcd的显存转成oled的显存很相似
														//类似于翻页
				}
			}
		}
		printf("\n");
	}

	while (1)
	{
		len = read(fd, &event, sizeof(event));//把地址传给这里，才可以去设置地址里面的内容！
		if (len == sizeof(event))//表示读到数据
		{
			printf("get event: type = 0x%x, code = 0x%x, value = 0x%x\n",
				event.type, event.code, event.value);
		}
		else
		{
			printf("read err %d\n", len);
		}
	}

	return 0;
}

