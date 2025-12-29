#include "stm32f4xx.h"

volatile uint32_t tick_ms = 0;
volatile uint32_t delay_ms = 500;

/* ---------- SysTick Interrupt ---------- */
void SysTick_Handler(void)
{
    tick_ms++;
}

/* ---------- Delay using SysTick ---------- */
void delay(uint32_t ms)
{
    uint32_t start = tick_ms;
    while ((tick_ms - start) < ms);
}

int main(void)
{
    /* ---------- Enable GPIOA Clock ---------- */
    RCC->AHB1ENR |= (1U << 0);   // GPIOAEN

    /* ---------- PA5 → LED OUTPUT ---------- */
    GPIOA->MODER &= ~(3U << (5 * 2));
    GPIOA->MODER |=  (1U << (5 * 2));

    /* ---------- PA6 → Button INC INPUT ---------- */
    GPIOA->MODER &= ~(3U << (6 * 2));
    GPIOA->PUPDR &= ~(3U << (6 * 2));
    GPIOA->PUPDR |=  (1U << (6 * 2));   // Pull-up

    /* ---------- PA7 → Button DEC INPUT ---------- */
    GPIOA->MODER &= ~(3U << (7 * 2));
    GPIOA->PUPDR &= ~(3U << (7 * 2));
    GPIOA->PUPDR |=  (1U << (7 * 2));   // Pull-up

    /* ---------- SysTick = 1 ms ---------- */
    SysTick_Config(SystemCoreClock / 1000);

    while (1)
    {
        /* Toggle LED */
        GPIOA->ODR ^= (1U << 5);
        delay(delay_ms);

        /* ---------- Button 1: Increase Delay ---------- */
        if (!(GPIOA->IDR & (1U << 6)))   // PA6 LOW
        {
            if (delay_ms < 1000)
                delay_ms += 100;

            while (!(GPIOA->IDR & (1U << 6))); // wait release
        }

        /* ---------- Button 2: Decrease Delay ---------- */
        if (!(GPIOA->IDR & (1U << 7)))   // PA7 LOW
        {
            if (delay_ms >= 0)
                delay_ms -= 100;
            else
                delay_ms = 0;

            while (!(GPIOA->IDR & (1U << 7))); // wait release
        }
    }
}
