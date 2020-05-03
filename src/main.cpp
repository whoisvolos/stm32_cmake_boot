#include "aux.h"

// Quick and dirty delay
static void delay (unsigned int time) {
    // 84 000 000
    // 84 000 * 500
    for (unsigned int i = 0; i < time; i++)
        for (volatile unsigned int j = 0; j < 2000; j++);
}

void GPIO_en() {
    RCC->AHB1ENR   |= RCC_AHB1ENR_GPIOCEN;
    GPIOC->MODER   |= GPIO_MODER_MODER13_0;      /* Output */
    //GPIOC->OTYPER  &= ~GPIO_OTYPER_OT_0;         /* push-pull */
    //GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0_0;  /* 10 MHz speed */
    //GPIOC->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR0; /* For 2 MHz */
    //GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0;  /* For 50 MHz (full-speed) */
}

int main() {
    GPIO_en();
    while (1) {
        //xputs("Speed of MCU: %i\n");
        // Set the state of pin 13 to output high
        GPIOC->BSRR = GPIO_BSRR_BS_13;
        delay(500);
        // Reset the state of pin 13 to output low
        GPIOC->BSRR = GPIO_BSRR_BR_13;
        delay(500);
    }
    return 0;
}
