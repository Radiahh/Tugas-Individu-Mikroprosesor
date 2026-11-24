#include "stm32f1xx.h"

void setup() {
  // Aktifkan clock GPIOA dan TIM2
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

  // PA0 sebagai output push-pull untuk LED
  GPIOA->CRL &= ~(0xF << 0);
  GPIOA->CRL |=  (0x2 << 0);

  // PA1 sebagai input pull-up untuk tombol
  GPIOA->CRL &= ~(0xF << 4);
  GPIOA->CRL |=  (0x8 << 4);
  GPIOA->ODR |= (1 << 1);

  // Konfigurasi TIM2
  // Asumsi clock timer 72 MHz
  TIM2->PSC = 7199;
  TIM2->ARR = 4999;
  TIM2->CNT = 0;

  TIM2->EGR = TIM_EGR_UG;
  TIM2->SR = 0;
  TIM2->CR1 |= TIM_CR1_CEN;
}

void loop() {
  // Tombol ditekan = PA1 LOW
  if (!(GPIOA->IDR & (1 << 1))) {

    // Periksa timer setiap 500 ms
    if (TIM2->SR & TIM_SR_UIF) {
      TIM2->SR &= ~TIM_SR_UIF;

      // Toggle LED
      GPIOA->ODR ^= (1 << 0);
    }

  } else {
    // Tombol dilepas: LED mati
    GPIOA->BRR = (1 << 0);

    // Reset hitungan timer
    TIM2->CNT = 0;
    TIM2->SR &= ~TIM_SR_UIF;
  }
}
