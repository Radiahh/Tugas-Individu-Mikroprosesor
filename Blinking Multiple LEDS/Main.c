
#include "stm32f1xx.h"
#include <stdint.h>

/*
 * Program 1:
 * Blinking Multiple LEDs at Different Frequencies
 *
 * Board : STM32F103C8 (Blue Pill)
 * Timer : TIM2, TIM3, TIM4
 *
 * LED Merah : PA0 -> TIM2
 * LED Hijau : PA1 -> TIM3
 * LED Biru  : PA2 -> TIM4
 */

void GPIO_Init(void)
{
    // Enable clock GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // PA0, PA1, PA2 sebagai output push-pull 2 MHz
    GPIOA->CRL &= ~((0xFU << 0) |
                    (0xFU << 4) |
                    (0xFU << 8));

    GPIOA->CRL |=  ((0x2U << 0) |
                    (0x2U << 4) |
                    (0x2U << 8));
}

void Timer_Init(void)
{
    // Enable clock TIM2, TIM3, TIM4
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN |
                    RCC_APB1ENR_TIM3EN |
                    RCC_APB1ENR_TIM4EN;

    /*
     * Clock timer = 8 MHz
     * Prescaler = 7999
     * Counter clock = 1000 Hz
     */

    // TIM2: update setiap 1 detik
    TIM2->PSC = 7999;
    TIM2->ARR = 999;
    TIM2->CNT = 0;
    TIM2->EGR = TIM_EGR_UG;
    TIM2->SR = 0;

    // TIM3: update setiap 0.5 detik
    TIM3->PSC = 7999;
    TIM3->ARR = 499;
    TIM3->CNT = 0;
    TIM3->EGR = TIM_EGR_UG;
    TIM3->SR = 0;

    // TIM4: update setiap 0.25 detik
    TIM4->PSC = 7999;
    TIM4->ARR = 249;
    TIM4->CNT = 0;
    TIM4->EGR = TIM_EGR_UG;
    TIM4->SR = 0;

    // Start all timers
    TIM2->CR1 |= TIM_CR1_CEN;
    TIM3->CR1 |= TIM_CR1_CEN;
    TIM4->CR1 |= TIM_CR1_CEN;
}

int main(void)
{
    GPIO_Init();
    Timer_Init();

    while (1)
    {
        // TIM2 controls red LED
        if (TIM2->SR & TIM_SR_UIF)
        {
            TIM2->SR &= ~TIM_SR_UIF;
            GPIOA->ODR ^= (1U << 0);
        }

        // TIM3 controls green LED
        if (TIM3->SR & TIM_SR_UIF)
        {
            TIM3->SR &= ~TIM_SR_UIF;
            GPIOA->ODR ^= (1U << 1);
        }

        // TIM4 controls blue LED
        if (TIM4->SR & TIM_SR_UIF)
        {
            TIM4->SR &= ~TIM_SR_UIF;
            GPIOA->ODR ^= (1U << 2);
        }
    }
}

