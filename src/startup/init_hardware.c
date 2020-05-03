#include "init_hardware.h"

void core_init() {
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));  /* set CP10 and CP11 Full Access */
#endif
}

void set_flash_latency() {
    // Flash latency
    FLASH->ACR =
            FLASH_ACR_LATENCY_2WS       /* 3 CPU cycles wait for 84 Mhz @ 3.3v */
            | FLASH_ACR_PRFTEN          /* enable prefetch */
            | FLASH_ACR_ICEN            /* instruction cache enable */
            | FLASH_ACR_DCEN;           /* data cache enable */
    // TODO: Check latency is set
}

void clock_reset() {
    /* Reset the RCC clock configuration to the default reset state ------------*/
    /* Set HSION bit */
    RCC->CR |= (uint32_t)0x00000001;

    /* Reset CFGR register */
    RCC->CFGR = 0x00000000;

    /* Reset HSEON, CSSON and PLLON bits */
    RCC->CR &= (uint32_t)0xFEF6FFFF;

    /* Reset PLLCFGR register */
    RCC->PLLCFGR = 0x24003010;

    /* Reset HSEBYP bit */
    RCC->CR &= (uint32_t)0xFFFBFFFF;

    /* Disable all interrupts */
    RCC->CIR = 0x00000000;
}

void clock_init() {
    /* Enable external oscillator */
    RCC->CR = RCC_CR_HSEON;
    /* Wait for locked external oscillator */
    while((RCC->CR & RCC_CR_HSERDY) != RCC_CR_HSERDY);

    /* PLL config */
    RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) - 1) << 16) | (PLL_Q << 24);

    /* HCLK = SYSCLK / 1 */
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
    /* PCLK2 = HCLK / 1 */
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
    /* PCLK1 = HCLK / 2 */
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

    /* PLL on */
    RCC->CR |= RCC_CR_PLLON;
    /* Wait for locked PLL */
    while((RCC->CR & RCC_CR_PLLRDY) != RCC_CR_PLLRDY);

    /* select system clock */
    RCC->CFGR &= ~RCC_CFGR_SW; /* clear */
    RCC->CFGR |= RCC_CFGR_SW_PLL;   /* SYSCLK is PLL */

    /* Wait for SYSCLK to be PPL */
    while((RCC->CFGR & RCC_CFGR_SW_PLL) != RCC_CFGR_SW_PLL);
}