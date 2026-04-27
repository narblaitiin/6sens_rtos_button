/*
 * Copyright (c) 2026
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

//  ========== includes ====================================================================
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

//  ========== defines =====================================================================
// time in milliseconds to wait between LED toggles (2000 ms = 2 seconds)
#define SLEEP_TIME_MS   2000

// device tree node identifiers for the LEDs
#define LED_0 DT_ALIAS(led0)
#define LED_1 DT_ALIAS(led1)

// device tree node identifiers for the buttons
#define SW_0 DT_ALIAS(sw0)
#define SW_1 DT_ALIAS(sw1)

//  ========== globals =====================================================================
// define GPIO configurations for the LEDs
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED_0, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED_1, gpios);

// define GPIO configurations for the buttons
static const struct gpio_dt_spec btn0 = GPIO_DT_SPEC_GET(SW_0, gpios);
static const struct gpio_dt_spec btn1 = GPIO_DT_SPEC_GET(SW_1, gpios);

// define callbacks data strutures
static struct gpio_callback btn0_cb_data;
static struct gpio_callback btn1_cb_data;

//  ========== ISR callbacks ===============================================================
void btn0_pressed(const struct device *dev, struct gpio_callback *cb,
                  gpio_port_pins_t pins) {
    printk("button 1 pressed — toggling LED 1\n");
    gpio_pin_toggle_dt(&led0);
}

void btn1_pressed(const struct device *dev, struct gpio_callback *cb,
                  gpio_port_pins_t pins) {
    printk("button 2 pressed — toggling LED 2\n");
    gpio_pin_toggle_dt(&led1);
}

//  ========== configure one button with interrupt + callback ==============================
static int setup_button(const struct gpio_dt_spec *btn, struct gpio_callback *cb_data,gpio_callback_handler_t handler) {
	int8_t ret;

    // test if GPIO device is ready
    if (!gpio_is_ready_dt(btn)) {
        printk("GPIO device %s not ready\n", btn->port->name);
        return -ENODEV;
    }

    // configure the buttons as active input pins
    ret = gpio_pin_configure_dt(btn, GPIO_INPUT);
    if (ret < 0) {
        printk("GPIO pin configuration failed. error: %d\n", ret);
        return ret;
    }

    // enable interrupt on falling edge (button press)
    ret = gpio_pin_interrupt_configure_dt(btn, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret < 0) {
        printk("GPIO interrupt configuration failed. error: %d\n", ret);
        return ret;
    }

    // init and add the callback
    gpio_init_callback(cb_data, handler, BIT(btn->pin));
    ret = gpio_add_callback(btn->port, cb_data);
    if (ret < 0) {
        printk("GPIO callback. error: %d\n", ret);
        return ret;
    }

    return 0;
}

//  ========== configure one LED as output, initially OFF ==================================
static int setup_led(const struct gpio_dt_spec *led)
{
    if (!gpio_is_ready_dt(led)) {
        printk("GPIO device %s not ready\n", led->port->name);
        return -ENODEV;
    }

    return gpio_pin_configure_dt(led, GPIO_OUTPUT_INACTIVE);
}

//  ========== main ========================================================================
int8_t main(void)
{
	// configure LEDs
    if (setup_led(&led0) < 0 || setup_led(&led1) < 0) {
        printk("LED setup failed — aborting\n");
        return -1;
    }

    // configure buttons */
    if (setup_button(&btn0, &btn0_cb_data, btn0_pressed) < 0 ||
        setup_button(&btn1, &btn1_cb_data, btn1_pressed) < 0) {
        printk("button setup failed — aborting\n");
        return -1;
    }

    printk("Ready. Press a button!\n");

    // main loop — everything is interrupt-driven, nothing to poll
    while (1) {
        k_sleep(K_FOREVER);
    }

    return 0;
}