#include "stm32f1xx.h"

void setup() {
  // Aktifkan clock GPIOA dan TIM2
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

  // PA0 sebagai output
  GPIOA->CRL &= ~(0xF << 0);
  GPIOA->CRL |=  (0x2 << 0);

  // PA1 sebagai input 
  GPIOA->CRL &= ~(0xF << 4);
  GPIOA->CRL |=  (0x8 << 4);
  GPIOA->ODR |= (1 << 1);


  // Asumsi clock timer 72 MHz
  TIM2->PSC = 7199;
  TIM2->ARR = 4999;
  TIM2->CNT = 0;

  TIM2->EGR = TIM_EGR_UG;
  TIM2->SR = 0;
  TIM2->CR1 |= TIM_CR1_CEN;
}

void loop() {

  if (!(GPIOA->IDR & (1 << 1))) {

    if (TIM2->SR & TIM_SR_UIF) {
      TIM2->SR &= ~TIM_SR_UIF;

      GPIOA->ODR ^= (1 << 0);
    }

  } else {
 
    GPIOA->BRR = (1 << 0);

    // Reset hitungan timer
    TIM2->CNT = 0;
    TIM2->SR &= ~TIM_SR_UIF;
  }
}
