#include "adc_dma.h"

#include "stm32f4xx.h"

#include <stdint.h>

/* "Number of transfers to make," the reason those transfers are happening
because we have 2 ADC channels.*/
uint16_t adc_raw_data[NUM_OF_CHANNELS];

void adc_dma_init(void) {
    /************GPIO Configuration**********/
    /* Configure PA0 and PA1 mode as analog to read from sensors */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    // PA0
    GPIOA->MODER |= (1U << 0);
    GPIOA->MODER |= (1U << 1);
    // PA1
    GPIOA->MODER |= (1U << 2);
    GPIOA->MODER |= (1U << 3);

    /************ADC Configuration**********/
    /* Configure ADC read sequence Ch0->1 and send signals to DMA after each conversion */
    // Enable clock access to ADC
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    // Set sequence length to 2 conversions (Formula: Length = L[3:0] + 1)
    // L[3:0] maps to bits 23:20. Setting bit 20 makes L = 1, so Length = 2.
    ADC1->SQR1 |= (1U << 20);
    ADC1->SQR1 &= ~(1U << 21);
    ADC1->SQR1 &= ~(1U << 22);
    ADC1->SQR1 &= ~(1U << 23);

    // Set sequence order: 1st conversion = Ch0 - PA0 (bits 4:0)
    //                     2nd conversion = Ch1 - PA1 (bits 9:5)
    ADC1->SQR3 = (0U << 0) | (1U << 5);

    // Enable Scan Mode: Required to convert multiple channels in a sequence (Ch0 then Ch1)
    ADC1->CR1 = ADC_CR1_SCAN;

    // CR2_CONT: Continuous Mode (Loop the sequence forever).
    // CR2_DMA:  Enable hardware signals to DMA after each conversion.
    // CR2_DDS:  Keep sending DMA requests indefinitely (required for Circular DMA).
    ADC1->CR2 |= (ADC_CR2_CONT | ADC_CR2_DMA | ADC_CR2_DDS);

    /************DMA Configuration**********/
    // Enable stream0 (connected with ADC1)
    // Peripheral addr for DMA to read from and Mem addr to write to
    // Transfer size & num

    // Enable clock access to DMA
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    // Disable DMA stream
    DMA2_Stream0->CR &= ~DMA_SxCR_EN;

    // Wait till DMA is disabled
    while (DMA2_Stream0->CR & DMA_SxCR_EN) {}

    /* By enabling circular mode, you change that behavior into an infinite
    loop. When the DMA finishes transferring the 2 items and NDTR hits 0, the
    hardware automatically does two things:

    It reloads NDTR back to its original value (2).
    It wraps the destination memory pointer back to the very beginning of your
    adc_raw_data array.*/
    DMA2_Stream0->CR |= DMA_SxCR_CIRC;

    // Set MSIZE: Memory data size to half-word
    DMA2_Stream0->CR |= (1 << 13);
    DMA2_Stream0->CR &= ~(1 << 14);

    // Set PSIZE: Peripheral data size to half-word
    DMA2_Stream0->CR |= (1 << 11);
    DMA2_Stream0->CR &= ~(1 << 12);

    // Enable memory addr increment
    DMA2_Stream0->CR |= DMA_SxCR_MINC;

    // Set periph address
    DMA2_Stream0->PAR = (uint32_t)(&ADC1->DR);
    // Set mem address
    DMA2_Stream0->M0AR = (uint32_t)(adc_raw_data);

    // Set number of transfer
    DMA2_Stream0->NDTR = (uint16_t)NUM_OF_CHANNELS;

    // Enable DMA stream
    DMA2_Stream0->CR |= DMA_SxCR_EN;

    /************ADC Configuration**********/
    // Enable ADC
    ADC1->CR2 |= ADC_CR2_ADON;

    // Start ADC
    ADC1->CR2 |= ADC_CR2_SWSTART;
}