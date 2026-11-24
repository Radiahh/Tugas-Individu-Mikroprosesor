#include "stm32f1xx.h"
#include <stdint.h>

volatile unsigned int pressed = 0;


#define VT_N 76

static uint32_t vtable[VT_N] __attribute__((aligned(512)));

void my_exti1_handler(void);

void install_irq_handler(void)
{
    uint32_t *original = (uint32_t *)SCB->VTOR;

 
    for (int i = 0; i < VT_N; i++)
    {
        vtable[i] = original[i];
    }

  
    vtable[16 + EXTI1_IRQn] =
        ((uint32_t)(uintptr_t)my_exti1_handler) | 1U;

    __DSB();
    SCB->VTOR = (uint32_t)vtable;
    __DSB();
    __ISB();
}

void GPIO_Init(void)
{
    // Aktifkan clock GPIOA dan AFIO
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN |
                    RCC_APB2ENR_AFIOEN;

    // PA0 sebagai output LED
    GPIOA->CRL &= ~(0xFU << 0);
    GPIOA->CRL |=  (0x2U << 0);

    // PA1 sebagai input pull-up
    GPIOA->CRL &= ~(0xFU << 4);
    GPIOA->CRL |=  (0x8U << 4);
    GPIOA->ODR |= (1U << 1);
}

void Timer_Init(void)
{
    // Aktifkan clock TIM2
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // Konfigurasi timer
    TIM2->PSC = 7999;
    TIM2->ARR = 99;
    TIM2->CNT = 0;


    TIM2->SR = 0;
}

void Interrupt_Init(void)
{
    // Hubungkan EXTI1 dengan PA1
    AFIO->EXTICR[0] &= ~(0xFU << 4);

    // Aktifkan EXTI1
    EXTI->IMR |= (1U << 1);

  
    EXTI->FTSR |= (1U << 1);
    EXTI->RTSR &= ~(1U << 1);

   
    EXTI->PR = (1U << 1);

 
    install_irq_handler();
    NVIC_EnableIRQ(EXTI1_IRQn);
}

void my_exti1_handler(void)
{
    if (EXTI->PR & (1U << 1))
    {
       
        EXTI->PR = (1U << 1);

  
        EXTI->IMR &= ~(1U << 1);

//semapore
        pressed = 1;
    }
}

int main(void)
{
    pressed = 0;

    GPIO_Init();
    Timer_Init();
    Interrupt_Init();

    while (1)
    {
        // Periksa semaphore
        if (pressed)
        {
            // Toggle LED
            GPIOA->ODR ^= (1U << 0);

            pressed = 0;

            // timer debounce mulai
            TIM2->CNT = 0;
            TIM2->SR = 0;
            TIM2->CR1 |= TIM_CR1_CEN;
        }

        // periksa timer
        if (TIM2->SR & TIM_SR_UIF)
        {
          
            TIM2->CR1 &= ~TIM_CR1_CEN;

            TIM2->SR &= ~TIM_SR_UIF;

            EXTI->PR = (1U << 1);

            EXTI->IMR |= (1U << 1);
        }
    }
}

