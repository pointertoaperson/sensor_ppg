
/**
 * @file    spi.c
 * @brief   SPI driver interface for STM32F103
 * @author  Umesh Puri
 * @date    2025-06-02
 * @version 1.0
 *
 * This file provides the interface for configuring and using the SPI peripheral
 * on the STM32F103. It includes functions for initialization and data transfer.
 *
 * @license MIT License
 * Copyright (c) 2025 Umesh
 */

#include "spi.h"

void spi_init(void)
{
  // Enable GPIOA, GPIOB, AFIO, SPI1
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN |
                  RCC_APB2ENR_IOPBEN |
                  RCC_APB2ENR_AFIOEN |
                  RCC_APB2ENR_SPI1EN;

  // PB6 as INPUT for Handshake
  GPIOB->CRL &= ~GPIO_CRL_MODE6;
  GPIOB->CRL |= GPIO_CRL_CNF6_1;
  GPIOB->CRL &= ~GPIO_CRL_CNF6_0;
  GPIOB->ODR &= ~GPIO_ODR_ODR6;

  // Disable JTAG (free PB3)
  AFIO->MAPR &= ~AFIO_MAPR_SWJ_CFG;
  AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

  // PB3 - SCK (AF-PP)
  GPIOB->CRL &= ~(GPIO_CRL_MODE3 | GPIO_CRL_CNF3);     // Clear bits
  GPIOB->CRL |= (GPIO_CRL_MODE3_1 | GPIO_CRL_MODE3_0); // MODE3 = 11 (50 MHz)
  GPIOB->CRL |= GPIO_CRL_CNF3_1;                       // CNF3_1 = 1
  GPIOB->CRL &= ~GPIO_CRL_CNF3_0;                      // CNF3_0 = 0

  // PB5 - MOSI (AF-PP)
  GPIOB->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5);     // Clear all bits
  GPIOB->CRL |= (GPIO_CRL_MODE5_1 | GPIO_CRL_MODE5_0); // MODE = 11 => 50 MHz
  GPIOB->CRL |= GPIO_CRL_CNF5_1;                       // CNF = 10 => AF push-pull
  GPIOB->CRL &= ~GPIO_CRL_CNF5_0;

  // PB4 - MISO (input floating)
  GPIOB->CRL &= ~(GPIO_CRL_MODE4 | GPIO_CRL_CNF4);
  GPIOB->CRL |= GPIO_CRL_CNF4_0;
  // Remap SPI1 to PB3-5
  AFIO->MAPR |= AFIO_MAPR_SPI1_REMAP;

  GPIOA->BSRR = GPIO_BSRR_BS15; // Set PA15 High
  // Clear MODE15 and CNF15 bits
  GPIOA->CRH &= ~(GPIO_CRH_MODE15 | GPIO_CRH_CNF15);

  // Set MODE15 to 11 and CNF15 to 00
  GPIOA->CRH |= (GPIO_CRH_MODE15_1 | GPIO_CRH_MODE15_0);

  SPI1->CR1 = 0;
  SPI1->CR1 &= ~SPI_CR1_DFF; // Clear DFF bit -> 8-bit frame

  SPI1->CR1 |= SPI_CR1_MSTR;                   // Master mode
  SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;      // Software NSS management, set NSS high
  SPI1->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA); // Mode 0 (CPOL=0, CPHA=0)
  SPI1->CR1 &= ~SPI_CR1_BR;                    // Clear BR bits
  SPI1->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_2;    // BR=101 (bit0 and bit2 set)
  SPI1->CR1 |= SPI_CR1_SPE;                    // Enable SPI
}

void spi_transmit(uint32_t data)
{
  uint8_t *pData = (uint8_t *)&data;

  cs_low();

  // Send write data  command bytes

  SPI1->DR = 0x02;

  while (!(SPI1->SR & SPI_SR_TXE))
    ;
  SPI1->DR = 0x00;

  //GPIOC->BSRR = GPIO_BSRR_BR13; // LED HIGH
  // Send 4 data bytes
  for (int i = 0; i < sizeof(uint32_t); i++)
  {
    while (!(SPI1->SR & SPI_SR_TXE))
      ;
    SPI1->DR = pData[i];
  }

  // Wait for last byte to be fully transmitted
  while (!(SPI1->SR & SPI_SR_TXE))
    ;
  while (SPI1->SR & SPI_SR_BSY)
    ;

  // Clear overrun flags by dummy read
  uint32_t dummy = SPI1->DR;
  dummy = SPI1->SR;

  cs_high();
}

void cs_low()
{
  GPIOA->BSRR = GPIO_BSRR_BR15; // NSS low
  for (uint16_t i = 0; i < 1000; i++)
    ;
}
void cs_high()
{
  GPIOA->BSRR = GPIO_BSRR_BS15; // NSS high
  for (uint16_t i = 0; i < 1000; i++)
    ;
}
