# Code for 6Sens Project : button application test for a customed MDBT50Q (based on nRF52840) board

## Overview
A simple button demo showcasing the use of GPIO input with interrupts.
This example purpose a GPIO interrupt handling with buttons and LEDs defined in Devicetree.
The version of Zephyr RTOS used is the version v4.0.0.

## Board used
Original MDBT50Q board, powered by battery/solar panel. (see 6sens_prj repository/hardware part, for more information.)

## Building and Running
The following commands clean build folder, build and flash the sample:

**Command to use**
````
west build -t pristine

west build -p always -b mdbt50q_lora_dev applications/6sens_rtos_button

west flash --runner jlink