# ADC_LCD

Utilizing a TFT-LCD on STM32F4 to display ADC measurement values. The idea is to achieve kinda DMM behavior, where the multiple channels' measured values are displayed in the real time. Touchscreen would make it possible to modify and configure measurement settings. This is a first step of the bigger project for "on-the-run" configurable MCU, where you'll be able to configure MCU peripherals without flashing.

This is an educational project, so the purpose is only fun and learning.

## Hardware 
 * A NUCLEO-F411RE board with STM32F411RET6 (100 MHz SYSCLK)
 * A TFT-LCD with ILI9341 driver, with SPI (100 MHz SCK)

## Software
 * Standard STM32 HAL Libraries, on the fresh STM32CubeIDE
 * ILI9341 Driver - I'm using an existing library from [this guy](https://github.com/martnak/STM32-ILI9341)

## To-Do

 - [x] Display single ADC measurement 
 - [ ] Display multichannel ADC measurement values
 - [ ] Make some cool professional font
 - [ ] Multiple measurements using DMA
 - [ ] Add touchscreen functionality
 - [ ] Calculate some statistics on touchscreen touch
 - [ ] FreeRTOS?

 