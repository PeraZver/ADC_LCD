# ADC_LCD

Utilizing a TFT-LCD on STM32F4 to display ADC measurement values. The idea is to achieve kinda DMM behavior, where the multiple channels' measured values are displayed in the real time. Touchscreen would make it possible to modify and configure measurement settings. This is a first step of the bigger project for "on-the-run" configurable MCU, where you'll be able to configure MCU peripherals without flashing.

This is an educational project, so the purpose is only fun and learning.

## Hardware 
 * A NUCLEO-F411RE board with STM32F411RET6 (100 MHz SYSCLK)
 * A TFT-LCD with ILI9341 driver from [AliExpress](https://www.aliexpress.com/item/32815224002.html?spm=a2g0s.9042311.0.0.6f9d4c4dv0T4OV), with SPI (100 MHz SCK)

### Wiring

|LCD Board|Nucleo |STM32F411RE|  
| ------- |:-----:| ---------:|
|VCC      |CN6-4  | -         | 3.3V
|GND      |CN6-6  | -         | GND
|CS       |CN5-3  | PB6       | D10, Chip Select
|RST      |CN9-5  | PB5       | D4,  LCD Reset
|DC       |CN7-21 | PB7       | Data control
|MOSI     |CN5-4  | PA7       | D11, MOSI
|SCK      |CN5-6  | PA5       | D13, SCK
|LED      |CN7-5  | VDD       | 5V
|MISO     |CN5-5  | PA6       | D12, MISO
|T_CLK    |CN8-4  | PB0       | A3, Touch CLK
|T_CS     |CN10-6 | PC5       | 
|T_DIN    |CN10-34| PC4       |
|T_DO     |CN10-22| PB2       |
|T_IRQ    |CN10-24| PB1       |


## Software
 * Standard STM32 HAL Libraries, on the fresh STM32CubeIDE
 * ILI9341 Driver - I'm using an existing library from [this guy](https://github.com/martnak/STM32-ILI9341)
   * Also [this one](https://github.com/afiskon/stm32-ili9341) has nice GUI.

## To-Do

 - [x] Display single ADC measurement 
 - [x] Display multichannel ADC measurement values
 - [x] Make some cool professional font
 - [x] Multiple measurements using DMA
 - [ ] Add touchscreen functionality
 - [ ] Calculate some statistics on touchscreen touch
 - [ ] Plot some values maybe
 - [ ] FreeRTOS
 - [ ] Some kickas graphics, like TouchGFX or uGFX2

 