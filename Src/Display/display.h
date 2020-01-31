#include "stm32f4xx_hal.h"
#include <stdlib.h>
#include "main.h"

void Display_Menu(void);
void Display_Text(void);
void Display_Picture(void);
void Display_Color_Picture(void);

/********************************************
 ******** My display functions **************
 ********     Pero, 1/2020     **************
 ********************************************/
void Display_ADCChannelIcon_60x40(char* text, unsigned int x, unsigned int y);
