README !

This project is used for initial developement firmware for ARM cortex-M4 based on i.MX6SX UDOO NEO
development kit. Please flash Linux image into SD card, boot up Linux before starting debugger unless
we can't connect JTAG with M4.

This source code is based on FreeRTOS_BSP_1.0.0_iMX6SX from NXP, only platform specific code is kept,
rtos source code is removed out of the source tree.

GDB Server : JLink V5.12d, JTAG adapter mode.

Linux Image : udoobuntu-udoo-neo-minimal_2.0rc2.zip
