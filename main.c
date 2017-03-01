/*
	created by Ahmed Moussa -- 2/25/17
	attempting to blink LED connected to pin 2 on galileo board
 */


#include <zephyr.h>
#include <misc/printk.h>
#include <gpio.h>
#include <device.h>

//#include "galileo2GPIO.h"

#define pin  2

#if defined(CONFIG_GPIO_DW_0)
#define GPIO_DRV_NAME CONFIG_GPIO_DW_0_NAME


void main(void)
{
	printk("maybe idk! %s\n", CONFIG_ARCH);
	k_sleep(2000);
	printk("BUT MAYBE I DON't CAAAAARE! \n");

	struct device *gpioDevice;
	struct device *pinmux;
	int ret = 0;

	gpioDevice = device_get_binding(CONFIG_GPIO_DW_0_NAME);
	if (!gpioDevice) {
		printk("Cannot find %s!\n", CONFIG_GPIO_DW_0_NAME);
		return;
	}
	pinmux = device_get_binding(CONFIG_PINMUX_NAME);

	//gpio_pin_configure(gpioDevice, pin, GPIO_DIR_OUT);
	pinmux_pin_set(pinmux, pin,/*GPIO_DIR_OUT??*/ );

	while(1){
		ret = gpio_pin_write(gpioDevice, pin, 1);
		if (ret) {
			printk("Someone messed up");
		}
		k_sleep(2000);
		printk("maybe idk! \n");
		ret = gpio_pin_write(gpioDevice, pin, 0);
		if (ret) {
			printk("Someone messed up");
		}
		k_sleep(2000);
		printk("BUT MAYBE I DON't CAAAAARE! \n");
	}
}
