#include <stdio.h>
#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/led.h>


#define UART_DEVICE_NODE DT_NODELABEL(usart1)

int main(void) {
	int ret = 0;

	const struct device *led_dev = DEVICE_DT_GET(DT_NODELABEL(pwmleds));
	if (!device_is_ready(led_dev)) {
		printk("LED设备未就绪\n");
		return 0;
	}
	ret = led_set_brightness(led_dev, 0, 30) ||
	      led_set_brightness(led_dev, 1, 20) ||
	      led_set_brightness(led_dev, 2, 50);
	if (ret) {
		printk("设置亮度失败: %d\n", ret);
	}

	while (1) {
		printk("Program running... \n");

		// Sleep
		k_sleep(K_SECONDS(4U));
	}

	return 0;
}
