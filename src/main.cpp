extern "C" {
    #include "aux.h"
    #include "xprintf/xprintf.h"
}

void GPIO_en() {
    RCC->AHB1ENR   |= RCC_AHB1ENR_GPIOCEN;
    GPIOC->MODER   |= GPIO_MODER_MODER13_0;      /* Output */
    //GPIOC->OTYPER  &= ~GPIO_OTYPER_OT_0;         /* push-pull */
    //GPIOC->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR0; /* For 2 MHz */
    //GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0_0;  /* 10 MHz speed */
    //GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0;  /* For 50 MHz (full-speed) */
}

uint32_t get_sysclock()
{
    static const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
    uint32_t tmp = 0, pllvco = 0, pllp = 2, pllsource = 0, pllm = 2;

    /* Get SYSCLK source -------------------------------------------------------*/
    tmp = RCC->CFGR & RCC_CFGR_SWS;
    uint32_t sysclock;
    switch (tmp)
    {
        case 0x00:  /* HSI used as system clock source */
            sysclock = HSI_VALUE;
            break;
        case 0x04:  /* HSE used as system clock source */
            sysclock = HSE_VALUE;
            break;
        case 0x08:  /* PLL used as system clock source */

            /* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N
               SYSCLK = PLL_VCO / PLL_P
               */
            pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) >> 22;
            pllm = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;

            if (pllsource != 0)
            {
                /* HSE used as PLL clock source */
                pllvco = (HSE_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
            }
            else
            {
                /* HSI used as PLL clock source */
                pllvco = (HSI_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
            }

            pllp = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >>16) + 1 ) *2;
            sysclock = pllvco / pllp;
            break;
        default:
            sysclock = HSI_VALUE;
            break;
    }
    /* Compute HCLK frequency --------------------------------------------------*/
    /* Get HCLK prescaler */
    tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
    /* HCLK frequency */
    sysclock >>= tmp;
    return sysclock;
}

void systick_init() {
    MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk,get_sysclock() / 1000 - 1);
    CLEAR_BIT(SysTick->VAL, SysTick_VAL_CURRENT_Msk);
    SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);
}

static __IO uint32_t millis = 0;

extern "C" void SysTick_Handler(void) {
    millis += 1;
}

void delay(uint32_t ms) {
    const uint32_t next = millis + ms;
    do {
        __WFI();
    } while (millis < next);
}

int main() {
    GPIO_en();
    systick_init();
    xdev_out(ITM_SendChar);
    while (1) {
        // Set the state of pin 13 to output high
        GPIOC->BSRR = GPIO_BSRR_BS_13;
        delay(500);
        // Reset the state of pin 13 to output low
        GPIOC->BSRR = GPIO_BSRR_BR_13;
        delay(500);
        xprintf("Frequency: %u\n", get_sysclock());
        xprintf("Time passed: %u\n", millis);
    }

    return 0;
}
