#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/slab.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/gpio_keys.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_gpio.h>
#include <linux/spinlock.h>

#define TOUCHSCREEN_POLL_TIME_MS 10

struct qemu_ts_con {
	volatile unsigned int pressure;
	volatile unsigned int x;
	volatile unsigned int y;
	volatile unsigned int clean;
};

static struct input_dev *g_input_dev;
static int g_irq;
static struct qemu_ts_con *ts_con;
struct timer_list ts_timer;

static void ts_irq_timer(unsigned long _data)
{
	if (ts_con->pressure) // pressed
	{
		input_event(g_input_dev, EV_ABS, ABS_X, ts_con->x);
		input_event(g_input_dev, EV_ABS, ABS_Y, ts_con->y);
		input_sync(g_input_dev);

		mod_timer(&ts_timer,
			jiffies + msecs_to_jiffies(TOUCHSCREEN_POLL_TIME_MS));
	}

}

static irqreturn_t input_dev_demo_isr(int irq, void *dev_id)
{
	/* read data */

	/* report data */
	//input_event(g_input_dev, EV_KEY, XX, 0);
	//input_sync(g_input_dev);
	//printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	if (ts_con->pressure)
	{
		input_event(g_input_dev, EV_ABS, ABS_X, ts_con->x);//按下上报
		input_event(g_input_dev, EV_ABS, ABS_Y, ts_con->y);
		input_event(g_input_dev, EV_KEY, BTN_TOUCH, 1);
		input_sync(g_input_dev);

		/* start timer */
		mod_timer(&ts_timer,//滑动上报
			jiffies + msecs_to_jiffies(TOUCHSCREEN_POLL_TIME_MS));
		//定时器都是一次性的如果你不去修改他的下一次时间他就是不起作用的
	}
	else
	{
		input_event(g_input_dev, EV_KEY, BTN_TOUCH, 0);
		input_sync(g_input_dev);

		/* cancel timer *///所以不需要取消定时器
	}
	
	return IRQ_HANDLED;
}


/* alloc/set/register platform_driver */
static int input_dev_demo_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	int error;
	//struct resource *irq;
	struct resource *io;
	int gpio;
	
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

	gpio = of_get_gpio(pdev->dev.of_node, 0);

	/* get hardware info from device tree */

	/* alloc/set/register input_dev *///根据设备树信息
	g_input_dev = devm_input_allocate_device(dev);

	g_input_dev->name = "input_dev_demo";
	g_input_dev->phys = "input_dev_demo";
	g_input_dev->dev.parent = dev;

	g_input_dev->id.bustype = BUS_HOST;
	g_input_dev->id.vendor = 0x0001;
	g_input_dev->id.product = 0x0001;
	g_input_dev->id.version = 0x0100;

	/* set 1: which type event ? */	
	__set_bit(EV_KEY, g_input_dev->evbit);
	__set_bit(EV_ABS, g_input_dev->evbit);
	__set_bit(INPUT_PROP_DIRECT, g_input_dev->propbit);

	/* set 2: which event ? */	
	__set_bit(BTN_TOUCH, g_input_dev->keybit);
	__set_bit(ABS_X, g_input_dev->absbit);
	__set_bit(ABS_Y, g_input_dev->absbit);

	/* set 3: event params ? */	
	input_set_abs_params(g_input_dev, ABS_X, 0, 0xffff, 0, 0);
	input_set_abs_params(g_input_dev, ABS_Y, 0, 0xffff, 0, 0);
	//注册
	error = input_register_device(g_input_dev);

	/* hardware opration */
	//获取寄存器资源
	io = platform_get_resource(pdev, IORESOURCE_MEM, 0);//寄存器是io资源
	ts_con = ioremap(io->start, io->end - io->start + 1);//映射寄存器地址


	//irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	//g_irq = irq->start;
	//按下上传
	g_irq = gpio_to_irq(gpio);//生成中断号
	//上面获得gpio的属性之后，我们按下按键会产生一个中断，所以要注册中断
	error = request_irq(g_irq, input_dev_demo_isr, 
					IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, 
					"input_dev_demo_irq", NULL);
	//滑动上传，设置定时器
	setup_timer(&ts_timer, 
			ts_irq_timer, (unsigned long)NULL);

	return 0;
}

static int input_dev_demo_remove(struct platform_device *pdev)
{
	del_timer_sync(&ts_timer);//删除定时器
	iounmap(ts_con);
	free_irq(g_irq, NULL);
	input_unregister_device(g_input_dev);
	return 0;
}

static const struct of_device_id input_dev_demo_of_match[] = {
	{ .compatible = "100ask,input_dev_demo", },
	{ },
};

static struct platform_driver input_dev_demo_driver = {
	.probe		= input_dev_demo_probe,
	.remove	    = input_dev_demo_remove,
	.driver		= {
		.name	= "input_dev_demo",
		.of_match_table = input_dev_demo_of_match,
	}
};


static int __init input_dev_demo_init(void)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	return platform_driver_register(&input_dev_demo_driver);
}

static void __exit input_dev_demo_exit(void)
{
	platform_driver_unregister(&input_dev_demo_driver);
}

module_init(input_dev_demo_init);
module_exit(input_dev_demo_exit);

MODULE_LICENSE("GPL");


