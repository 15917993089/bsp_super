#ifndef _LED_OPR_H_
#define _LED_OPR_H_
/*自定义板子的操作函数集*/
struct led_operations {
	int (*init)(int which);//初始化LED，which-哪个LED
	int (*ctl)(int which, char status);//控制LED，which-哪个LED，status：1-亮，0-灭
};
//为了避免直接使用
struct led_operations *get_board_led_opr(void);

#endif

