/**
 * @file    adc.c
 * @brief   ADC driver implementation for STM32F103
 * @author  Umesh Puri
 * @date    2025-06-02
 * @version 1.0
 *
 * This file implements analog-to-digital conversion using the STM32 ADC peripheral.
 * It includes functions to initialize the ADC, read values, and manage channels.
 *
 * @license MIT License
 * Copyright (c) 2025 Umesh
 */



#include "stm32f1xx.h"
#include "uart.h"
#include "adc.h"
#include "delay.h"

volatile int16_t adc_buffer[ADC_BUFFER];
volatile int16_t adc_val = 0;

bool ACC_COMP = false;

#define ADC_Channels 0x01

#define ADC_CH0 0x00 // PA0

void adc_init_cont(void)
{

	NVIC_SetPriority(DMA1_Channel1_IRQn, 1);
	
	//  Enable Clocks
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // GPIOA
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // ADC1
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;	// DMA1
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // TIM3

	//  Set PA0 to Analog Mode (ADC CH0)
	GPIOA->CRL &= ~GPIO_CRL_CNF0;
	GPIOA->CRL &= ~GPIO_CRL_MODE0;

	//  ADC Configuration
	ADC1->CR2 &= ~ADC_CR2_CONT; // Disable continuous mode
	ADC1->CR1 &= ~ADC_CR1_SCAN; // No scan (single channel)

	ADC1->CR2 &= ~ADC_CR2_SWSTART; // disable software trigger

	ADC1->CR2 &= ~ADC_CR2_EXTSEL;
	ADC1->CR2 |= (4 << ADC_CR2_EXTSEL_Pos); // 0b100 = TIM3_TRGO
	ADC1->CR2 |= ADC_CR2_EXTTRIG;

	ADC1->CR2 |= ADC_CR2_DMA;
	; // Enable DMA

	ADC1->SQR1 = 0; // Only 1 conversion
	ADC1->SQR3 = 0; // CH0 as first conversion

	// DMA Setup for ADC1
	DMA1_Channel1->CCR = 0; // Clear all first
	DMA1->IFCR |= DMA_IFCR_CTCIF1 | DMA_IFCR_CHTIF1 | DMA_IFCR_CTEIF1;

	DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0	// Peripheral size = 16-bit
						  | DMA_CCR_MSIZE_0 // Memory size = 16-bit
						  | DMA_CCR_TCIE	// Transfer complete interrupt
						  | DMA_CCR_CIRC;	// Circular mode
	// No circular mode
	DMA1_Channel1->CNDTR = 1; // Count exactly one transfer
	DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;
	DMA1_Channel1->CMAR = (uint32_t)&adc_val;

	NVIC_EnableIRQ(DMA1_Channel1_IRQn);

	// TIM3 Setup for ADC Trigger
	TIM3->PSC = 36000 - 1; // 0.5 ms tick
	TIM3->ARR = 12 - 1;	   // 12 x 0.5 = 6ms

	TIM3->CR2 &= ~TIM_CR2_MMS;
	TIM3->CR2 |= TIM_CR2_MMS_1; // MMS = 010: UEV as TRGO

	

	//  Power up ADC
	ADC1->CR2 |= ADC_CR2_ADON; // Power up
	for (volatile int i = 0; i < 1000; i++)
		;					   // Wait
	ADC1->CR2 |= ADC_CR2_ADON; // Start conversion / enable

	// Enable Everything
	DMA1_Channel1->CCR |= DMA_CCR_EN;
	TIM3->CR1 |= TIM_CR1_CEN;
}

void stop_process(void)
{
	// Stop Timer
	TIM3->CR1 &= ~TIM_CR1_CEN;
	// Stop ADC
	ADC1->CR2 &= ~ADC_CR2_ADON; // Start calibration / enable
	// Stop DMA
	DMA1_Channel1->CCR &= ~DMA_CCR_EN;
}

void start_process(void)
{

	ADC1->CR2 |= ADC_CR2_ADON; // Power up ADC
	for (volatile int i = 0; i < 1000; i++)
		;					   // delay
	ADC1->CR2 |= ADC_CR2_ADON; // Start conversion

	// Start DMA
	DMA1_Channel1->CCR |= DMA_CCR_EN;

	// Start Timer
	TIM3->CR1 |= TIM_CR1_CEN;
}
