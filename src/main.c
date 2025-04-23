#include <stdio.h>
#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/led.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, CONFIG_LOG_DEFAULT_LEVEL);

int main(void) {
	int ret = 0;

	const struct device *led_dev = DEVICE_DT_GET(DT_NODELABEL(leds));
	if (!device_is_ready(led_dev)) {
		LOG_ERR("LED设备未就绪\n");
		return 0;
	}
	ret = led_set_brightness(led_dev, 0, 30) ||
	      led_set_brightness(led_dev, 1, 20) ||
	      led_set_brightness(led_dev, 2, 50);
	if (ret) {
		LOG_ERR("设置亮度失败: %d\n", ret);
	}

	while (1) {
		LOG_INF("程序运行中... \n");
		k_sleep(K_SECONDS(1U));

		printk("测试Console\n");
		k_sleep(K_SECONDS(1U));
	}

	return 0;
}
