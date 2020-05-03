#pragma once

#if defined(STM32F401xC)

/* 25MHz crystal osc / 25 = 1 MHz */
#define PLL_M      25U
/* 1MHz * 336 = 336 MHz */
#define PLL_N      336U
/* 336 MHz / 4 = 84 MHz (SYSCLK) */
#define PLL_P      4U
/* 336 MHz / 7 = 48 MHz (USB) */
#define PLL_Q      7U

#endif

#ifndef CLOCK_SOURCE
#define CLOCK_SOURCE=HSI
#endif

#if CLOCK_SOURCE == HSI
#if defined(HSI_VALUE)
#define SYSCLK_STARTUP (HSI_VALUE / PLL_M * PLL_N / 4)
#else
#error "Please set HSI_VALUE"
#endif
#elif CLOCK_SOURCE == HSE
#if defined(HSI_VALUE)
#define SYSCLK_STARTUP (HSE_VALUE / PLL_M * PLL_N / 4)
#else
#error "Please set HSE_VALUE"
#endif
#else
#error "Please set CLOCK_SOURCE to HSI or HSE"
#endif