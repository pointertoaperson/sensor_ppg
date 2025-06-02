
/**
 * @file    HSE_CLK.c
 * @brief   HSE clock configuration header for STM32F103 microcontroller series
 * @author  Umesh Puri
 * @date    2025-06-02
 * @version 1.0
 *
 * This file contains functions and definitions for initializing and configuring
 * the High-Speed External (HSE) clock source for the STM32F103 system clock.
 *
 * @license MIT License
 * Copyright (c) 2025 Umesh
 */

#include "stm32f1xx.h"
#include "system_stm32f1xx.h"

#include "HSE_CLK.h"
void X_Clock_Init(void)
{
  /* 1. Enable HSE (High Speed External clock) */
  RCC->CR |= RCC_CR_HSEON;
  while ((RCC->CR & RCC_CR_HSERDY) == 0)
    ; // Wait until HSE is ready

  /* 2. Configure Flash prefetch and latency */
  FLASH->ACR |= FLASH_ACR_PRFTBE;    // Enable prefetch buffer
  FLASH->ACR &= ~FLASH_ACR_LATENCY;  // Clear latency bits
  FLASH->ACR |= FLASH_ACR_LATENCY_2; // 2 wait states for 72 MHz

  /* 3. Set AHB, APB1, and APB2 prescalers */
  RCC->CFGR &= ~(RCC_CFGR_HPRE |  // AHB prescaler
                 RCC_CFGR_PPRE1 | // APB1 prescaler
                 RCC_CFGR_PPRE2); // APB2 prescaler

  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;  // AHB = SYSCLK / 1 = 72 MHz
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; // APB1 = HCLK / 2 = 36 MHz (max)
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV1; // APB2 = HCLK / 1 = 72 MHz

  /* 4. Configure PLL: source = HSE, multiplier = 9 → 8 MHz × 9 = 72 MHz */
  RCC->CFGR &= ~(RCC_CFGR_PLLSRC |
                 RCC_CFGR_PLLXTPRE |
                 RCC_CFGR_PLLMULL);
  RCC->CFGR |= RCC_CFGR_PLLSRC;   // PLL source = HSE (not divided)
  RCC->CFGR |= RCC_CFGR_PLLMULL9; // PLL multiplier = 9

  /* 5. Enable PLL */
  RCC->CR |= RCC_CR_PLLON;
  while ((RCC->CR & RCC_CR_PLLRDY) == 0)
    ; // Wait until PLL is ready

  /* 6. Select PLL as system clock */
  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_SW_PLL;
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    ; // Wait for switch
  for (int i = 0; i < 72000000; i++)
    ; // 1 sec wait time
      /* 7. Update SystemCoreClock variable */
  // SystemCoreClock = 72000000;
}
