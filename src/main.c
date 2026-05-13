#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

// ===== Devicetree =====
#define LED0_NODE DT_ALIAS(led0)
#define SW1_NODE  DT_ALIAS(sw1)

// ===== GPIO specs =====
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec btn1 = GPIO_DT_SPEC_GET(SW1_NODE, gpios);

// ===== Callback =====
static struct gpio_callback btn_cb_data;

// ===== ISR =====
void button_pressed(const struct device *dev,
                    struct gpio_callback *cb,
                    gpio_port_pins_t pins)
{
    printk("Button pressed!\n");

    gpio_pin_toggle_dt(&led0);
}

// ===== Main =====
int main(void)
{
    int ret;

    // --- LED ---
    if (!gpio_is_ready_dt(&led0)) {
        printk("LED not ready\n");
        return -1;
    }

    ret = gpio_pin_configure_dt(&led0,
                                GPIO_OUTPUT_INACTIVE | led0.dt_flags);
    if (ret < 0) {
        printk("LED config failed\n");
        return -1;
    }

    // --- Button ---
    if (!gpio_is_ready_dt(&btn1)) {
        printk("Button not ready\n");
        return -1;
    }

    ret = gpio_pin_configure_dt(&btn1,
                                GPIO_INPUT | btn1.dt_flags);
    if (ret < 0) {
        printk("Button config failed\n");
        return -1;
    }

    ret = gpio_pin_interrupt_configure_dt(&btn1,
                                          GPIO_INT_EDGE_TO_ACTIVE);
    if (ret < 0) {
        printk("Interrupt config failed\n");
        return -1;
    }

    // --- Callback ---
    gpio_init_callback(&btn_cb_data,
                       button_pressed,
                       BIT(btn1.pin));

    gpio_add_callback(btn1.port, &btn_cb_data);

    printk("Ready: press button sw1\n");

    while (1) {
        k_sleep(K_FOREVER);
    }
}