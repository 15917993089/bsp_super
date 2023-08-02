/* 为uart_driver下面的每一个port添加一个端口 */
#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/sysrq.h>
#include <linux/platform_device.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/serial_core.h>
#include <linux/serial.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/rational.h>
#include <linux/reset.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/io.h>
#include <linux/dma-mapping.h>
#include <linux/proc_fs.h>

#include <asm/irq.h>


#define BUF_LEN  1024
#define NEXT_PLACE(i) ((i+1)&0x3FF)

static struct uart_port	*virt_port;
static unsigned char txbuf[BUF_LEN];
static int tx_buf_r = 0;
static int tx_buf_w = 0;

static unsigned char rxbuf[BUF_LEN];
static int rx_buf_w = 0;

static struct proc_dir_entry *uart_proc_file;



static struct uart_driver virt_uart_drv = {
	.owner          = THIS_MODULE,
	.driver_name    = "VIRT_UART",
	.dev_name       = "ttyVIRT",
	.major          = 0,
	.minor          = 0,
	.nr             = 1,
};

/* circle buffer */


static int is_txbuf_empty(void)
{
	return tx_buf_r == tx_buf_w;
}

static int is_txbuf_full(void)
{
	return NEXT_PLACE(tx_buf_w) == tx_buf_r;
}

static int txbuf_put(unsigned char val)
{
	if (is_txbuf_full())
		return -1;
	txbuf[tx_buf_w] = val;
	tx_buf_w = NEXT_PLACE(tx_buf_w);
	return 0;
}

static int txbuf_get(unsigned char *pval)
{
	if (is_txbuf_empty())
		return -1;
	*pval = txbuf[tx_buf_r];
	tx_buf_r = NEXT_PLACE(tx_buf_r);
	return 0;
}

static int txbuf_count(void)
{
	if (tx_buf_w >= tx_buf_r)
		return tx_buf_w - tx_buf_r;
	else
		return BUF_LEN + tx_buf_w - tx_buf_r;
}

ssize_t virt_uart_buf_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	/* 把txbuf中的数据copy_to_user */
	int cnt = txbuf_count();//要读多长
	int i;
	unsigned char val;
	int ret;

	cnt = (cnt > size)? size: cnt;

	for (i = 0; i < cnt; i++)
	{
		txbuf_get(&val);//从环形缓冲区中把数据一个一个取出来
		ret = copy_to_user(buf+i, &val, 1);
	}//这不是高效的方法
	//高效的方法是一次性拷贝到用户空间
	return cnt;
}

static ssize_t virt_uart_buf_write (struct file *file, const char __user *buf, size_t size, loff_t *ppos)
{
	int ret;
	/* get data */
	ret = copy_from_user(rxbuf, buf, size);
	rx_buf_w = size;

	/* 模拟产生RX中断 */
	irq_set_irqchip_state(virt_port->irq, IRQCHIP_STATE_PENDING, 1);//表示有中断在等待，
																    //下面要注册一个中断处理函数
	
	return size;
	//return 0;
}

static const struct file_operations virt_uart_buf_fops = {//虽然有这样的一个结构体但是我们要把他用起来
															//创建一个虚拟文件
	.read		= virt_uart_buf_read,
	.write		= virt_uart_buf_write,
};

static unsigned int virt_tx_empty(struct uart_port *port)
{
	/* 因为要发送的数据瞬间存入buffer */
	return 1;
}


/*
 * interrupts disabled on entry
 */
static void virt_start_tx(struct uart_port *port)
{
	struct circ_buf *xmit = &port->state->xmit;

	while (!uart_circ_empty(xmit) &&
	       !uart_tx_stopped(port)) {//环形缓冲区里面有数据并且还没有停止这个串口
		/* send xmit->buf[xmit->tail]
		 * out the port here */

		/* 把circ buffer中的数据全部存入txbuf */

		//txbuf[tx_buf_w++] =  xmit->buf[xmit->tail];
		txbuf_put(xmit->buf[xmit->tail]);
		xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
		port->icount.tx++;//更新统计信息
	}

   if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)
	   uart_write_wakeup(port);//如果有其他程序在等待的话我们可以去唤醒他们

}

static void virt_set_termios(struct uart_port *port, struct ktermios *termios,
		  struct ktermios *old)
{
   return;
}

static int virt_startup(struct uart_port *port)
{
   return 0;
}

static void virt_set_mctrl(struct uart_port *port, unsigned int mctrl)
{
}

static unsigned int virt_get_mctrl(struct uart_port *port)
{
   return 0;
}

static void virt_stop_tx(struct uart_port *port)
{
}

static void virt_stop_rx(struct uart_port *port)
{
}

static void virt_shutdown(struct uart_port *port)
{
}

static const char *virt_type(struct uart_port *port)
{
   return "100ASK_VIRT_UART";
}   



static const struct uart_ops virt_pops = {
	.tx_empty   = virt_tx_empty,//瞬间发送
   .set_mctrl  = virt_set_mctrl,
   .get_mctrl  = virt_get_mctrl,
   .stop_tx    = virt_stop_tx,
   .start_tx   = virt_start_tx,//有数据要发送时调用
   .stop_rx    = virt_stop_rx,
   //.enable_ms    = imx_enable_ms,
   //.break_ctl    = imx_break_ctl,
   .startup    = virt_startup,
   .shutdown   = virt_shutdown,
   //.flush_buffer = imx_flush_buffer,
   .set_termios    = virt_set_termios,
   .type	   = virt_type,
   //.config_port  = imx_config_port,
   //.verify_port  = imx_verify_port,
};

static irqreturn_t virt_uart_rxint(int irq, void *dev_id)
{
	int i;
	struct uart_port *port = dev_id;
	struct tty_port *tport = &port->state->port;
	unsigned long flags;
	spin_lock_irqsave(&port->lock, flags);//先关中断
	for (i = 0; i < rx_buf_w; i++) {
		port->icount.rx++;//行规程获取数据的同时，会更新统计信息
	
		/* get data from hardware/rxbuf */

		/* put data to ldisc */
		tty_insert_flip_char(tport, rxbuf[i], TTY_NORMAL);//放到行规程
	}
	rx_buf_w = 0;//全部写完之后，这个写位置给复位为0
	spin_unlock_irqrestore(&port->lock, flags);//开启中断
	tty_flip_buffer_push(tport);//告诉上一层，即行规程来处理
	
	return IRQ_HANDLED;
}

//在probe函数里面我们要去设备树获得硬件信息（去构造和添加uart_port）
//然后去设置uart_port,需要用到uart_ops（波特率、数据位、停止位还有流量控制等等还有读写）
//注册
static int virtual_uart_probe(struct platform_device *pdev)
{	int rxirq;
	int ret;

	/* create proc file */	//把virt_uart_buf_fops这个用起来，创建一个虚拟文件
	uart_proc_file = proc_create("virt_uart_buf", 0, NULL, &virt_uart_buf_fops);
	
	//这个port属于哪一个uart_driver所以在入口函数那里还要注册
	//uart_add_one_port(struct uart_driver * drv, struct uart_port * uport);

	/* 从设备树获得硬件信息 */
	rxirq = platform_get_irq(pdev, 0);
	
	/* 分配设置注册uart_port */
	virt_port = devm_kzalloc(&pdev->dev, sizeof(*virt_port), GFP_KERNEL);

	virt_port->dev = &pdev->dev;
	virt_port->iotype = UPIO_MEM;
	virt_port->irq = rxirq;
	virt_port->fifosize = 32;
	virt_port->ops = &virt_pops;
	virt_port->flags = UPF_BOOT_AUTOCONF;
	virt_port->type = PORT_8250;

	/* 注册一个中断 */
	ret = devm_request_irq(&pdev->dev, rxirq, virt_uart_rxint, 0,
				       dev_name(&pdev->dev), virt_port);
	
	return uart_add_one_port(&virt_uart_drv, virt_port);
}
static int virtual_uart_remove(struct platform_device *pdev)
{
	uart_remove_one_port(&virt_uart_drv, virt_port);
	proc_remove(uart_proc_file);
	return 0;
}



static const struct of_device_id virtual_uart_of_match[] = {
	{ .compatible = "100ask,virtual_uart", },
	{ },
};


static struct platform_driver virtual_uart_driver = {
	.probe		= virtual_uart_probe,
	.remove		= virtual_uart_remove,
	.driver		= {
		.name	= "100ask_virtual_uart",
		.of_match_table = of_match_ptr(virtual_uart_of_match),
	}
};


/* 1. 入口函数 */
static int __init virtual_uart_init(void)
{	
	int ret;
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	
	ret = uart_register_driver(&virt_uart_drv);//和platform_driver一样
													//我们要去构造一个uart_driver

	if (ret)
		return ret;
	
	/* 1.1 注册一个platform_driver */
	return platform_driver_register(&virtual_uart_driver);
}


/* 2. 出口函数 */
static void __exit virtual_uart_exit(void)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	/* 2.1 反注册platform_driver */
	platform_driver_unregister(&virtual_uart_driver);

	uart_unregister_driver(&virt_uart_drv);
}

module_init(virtual_uart_init);
module_exit(virtual_uart_exit);

MODULE_LICENSE("GPL");


