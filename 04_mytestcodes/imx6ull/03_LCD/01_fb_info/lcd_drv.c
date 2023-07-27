/* frambuffer驱动程序框架 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/cpufreq.h>
#include <linux/io.h>

#include <asm/div64.h>

#include <asm/mach/map.h>
#include <mach/regs-lcd.h>
#include <mach/regs-gpio.h>
#include <mach/fb.h>

static struct fb_info *myfb_info;
static unsigned int pseudo_palette[16];

/* from pxafb.c */
static inline unsigned int chan_to_field(unsigned int chan,
					 struct fb_bitfield *bf)
{
	chan &= 0xffff;
	chan >>= 16 - bf->length;
	return chan << bf->offset;
}
static int mylcd_setcolreg(unsigned regno,
			       unsigned red, unsigned green, unsigned blue,
			       unsigned transp, struct fb_info *info)
{
	unsigned int val;

	/* dprintk("setcol: regno=%d, rgb=%d,%d,%d\n",
		   regno, red, green, blue); */

	switch (info->fix.visual) {
	case FB_VISUAL_TRUECOLOR:
		/* true-colour, use pseudo-palette */

		if (regno < 16) {
			u32 *pal = info->pseudo_palette;

			val  = chan_to_field(red,   &info->var.red);
			val |= chan_to_field(green, &info->var.green);
			val |= chan_to_field(blue,  &info->var.blue);

			pal[regno] = val;
		}
		break;

	default:
		return 1;	/* unknown type */
	}

	return 0;
}

static struct fb_ops myfb_ops = {
	.owner 			=	THIS_MODULE,
	.fb_setcolreg	= mylcd_setcolreg,//假调色板
	.fb_fillrect 	=	cfb_fillrect,
	.fb_copyarea 	=	cfb_copyarea,
	.fb_imageblit 	=	cfb_imageblit,
};


int __init lcd_drv_init(void)
{
	dma_addr_t phy_addr;//物理地址保存在这个变量里,用来保存物理地址

	/* 1.分配 fb_info */ 
	myfb_info = framebuffer_alloc(0,NULL);
	/* 2.设置 fb_info */
	/* a.var:LCD分辨率、颜色格式 */
	myfb_info->var.xres = 1024;
	myfb_info->var.yres = 600;

	myfb_info->var.bits_per_pixel = 16;

	myfb_info->var.red.offset = 11;
	myfb_info->var.red.length = 5;

 	myfb_info->var.green.offset = 5;
	myfb_info->var.green.length = 6;

	myfb_info->var.blue.offset = 0;
	myfb_info->var.blue.length = 5;
	
	/* b.fix */
	myfb_info->fix.smem_len = myfb_info->var.xres * myfb_info->var.yres * myfb_info->var.bits_per_pixel/8;
	if(myfb_info->var.bits_per_pixel == 24)//显存的长度
		myfb_info->fix.smem_len = myfb_info->var.xres * myfb_info->var.yres * 4;
	
	
	//fb的虚拟地址
	myfb_info->screen_base = dma_alloc_wc(NULL, myfb_info->fix.smem_len, &phy_addr,
					 GFP_KERNEL);//这个函数分配显存之后会把这个显存的物理地址保存在phy_addr里面
	myfb_info->fix.smem_start = phy_addr;//fb的物理地址


	myfb_info->fix.type = FB_TYPE_PACKED_PIXELS;
	myfb_info->fix.visual = FB_VISUAL_TRUECOLOR;//全彩
	//每行长度
	myfb_info->fix.line_length = myfb_info->var.xres * myfb_info->var.bits_per_pixel / 8;
	if (myfb_info->var.bits_per_pixel == 24)
		myfb_info->fix.line_length = myfb_info->var.xres * 4;
	
	/* c.fbops */
	myfb_info->fbops = &myfb_ops;
	myfb_info->pseudo_palette = pseudo_palette;//假调色板


	/* 3.注册 fb_info */
	register_framebuffer(struct fb_info * fb_info)

	/* 4.硬件操作 */
	

	return 0;
}

static void __exit lcd_drv_eixt(void)
{
	/* 1.反注册 fb_info */
	unregister_framebuffer(myfb_info);
	/* 2.释放 fb_info */
	framebuffer_release(myfb_info);
	
}

module_init(lcd_drv_init);
module_exit(lcd_drv_eixt);

MODULE_AUTHOR("曾东城");
MODULE_DESCRIPTION("Framebuffer driver for the myboard");
MODULE_LICENSE("GPL");








