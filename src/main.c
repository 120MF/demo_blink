#include <stdio.h>
#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/uart.h>


static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));
#define UART_DEVICE_NODE DT_NODELABEL(usart6)
static const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

#define MIN_PERIOD PWM_SEC(1U) / 128U
#define MAX_PERIOD PWM_SEC(1U)


int main(void)
{

	// Make sure that the GPIO was initialized
	if (!gpio_is_ready_dt(&led0)) {
		return 0;
	}


	// Set the GPIO as output
	int ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT);
	if (ret < 0) {
		return 0;
	}
	if (!device_is_ready(uart_dev)) {
		return 0;
	}
	const char *data = "Hello from UART6!\r\n";
	for (int i = 0; data[i]; i++) {
		uart_poll_out(uart_dev, data[i]);
	}

	if (!pwm_is_ready_dt(&pwm_led0)) {
		printk("Error: PWM device %s is not ready\n",
			   pwm_led0.dev->name);
		return 0;
	}

	printk("Calibrating for channel %d...\n", pwm_led0.channel);
	uint32_t max_period = MAX_PERIOD;
	while (pwm_set_dt(&pwm_led0, max_period, max_period / 2U)) {
		max_period /= 2U;
		if (max_period < (4U * MIN_PERIOD)) {
			printk("Error: PWM device "
				   "does not support a period at least %lu\n",
				   4U * MIN_PERIOD);
			return 0;
		}
	}

	ret = gpio_pin_set_dt(&led0, 1);

	uint32_t period = max_period;
	uint8_t dir = 0U;
	// Do forever
	while (1) {


		ret = pwm_set_dt(&pwm_led0, period, period / 2U);
		if (ret) {
			printk("Error %d: failed to set pulse width\n", ret);
			return 0;
		}
		printk("Using period %d\n", period);

		period = dir ? (period * 2U) : (period / 2U);
		if (period > max_period) {
			period = max_period / 2U;
			dir = 0U;
		} else if (period < MIN_PERIOD) {
			period = MIN_PERIOD * 2U;
			dir = 1U;
		}

		// Sleep
		k_sleep(K_SECONDS(4U));
	}

	return 0;
}
