# Módulo RCC (Reset and Clock Control)

El periférico RCC es fundamental para el funcionamiento del microcontrolador, ya que gestiona la habilitación/deshabilitación de los relojes para los periféricos. Sin un reloj habilitado, un periférico no funcionará.

**Archivos:** `Inc/rcc.h`, `Src/rcc.c`
**Referencia Principal:** RM0351, Sección "Reset and clock control (RCC)".

## Objetivos del Módulo RCC
1.  Proporcionar funciones para habilitar los relojes de los periféricos GPIO, TIM3, USART2, y SYSCFG.
2.  Definir las frecuencias de reloj que se usarán como base para la configuración de otros periféricos (SysTick, UART, TIM), consistentes con los talleres anteriores.

## 1. `Inc/rcc.h`

```c
#ifndef RCC_H
#define RCC_H

#define SYSCLK_FREQ_HZ    4000000UL  
#define HCLK_FREQ_HZ      SYSCLK_FREQ_HZ
#define PCLK1_FREQ_HZ     HCLK_FREQ_HZ   // Asumiendo APB1 Prescaler = 1
#define PCLK2_FREQ_HZ     HCLK_FREQ_HZ   // Asumiendo APB2 Prescaler = 1
#define TIM_PCLK_FREQ_HZ  PCLK1_FREQ_HZ  // TIM3 está en APB1

// Prototipos de funciones
void rcc_system_basic_init(void); // Podría verificar/habilitar HSI si es necesario.
void rcc_gpio_clock_enable(GPIO_TypeDef *gpio_port);
void rcc_usart2_clock_enable(void);
void rcc_tim3_clock_enable(void);
void rcc_syscfg_clock_enable(void);

#endif // RCC_H

```

## 2. `Src/rcc.c`

```c
#include "rcc.h"

void rcc_gpio_clock_enable(GPIO_TypeDef *gpio_port) {
    if (gpio_port == GPIOA) {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    } else if (gpio_port == GPIOB) {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    } else if (gpio_port == GPIOC) {
        RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
    }
    // Añadir más puertos GPIO si son necesarios (D, E, F, G, H)

    // Pequeña espera después de habilitar el reloj para asegurar que esté estable
    // Esto se logra con una lectura ficticia del registro.
    volatile uint32_t dummy_read = RCC->AHB2ENR;
    (void)dummy_read; // Evitar warning de variable no usada
}

void rcc_usart2_clock_enable(void) {
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
    volatile uint32_t dummy_read = RCC->APB1ENR1;
    (void)dummy_read;
}

void rcc_tim3_clock_enable(void) {
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;
    volatile uint32_t dummy_read = RCC->APB1ENR1;
    (void)dummy_read;
}

void rcc_syscfg_clock_enable(void) {
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    volatile uint32_t dummy_read = RCC->APB2ENR;
    (void)dummy_read;
}
```