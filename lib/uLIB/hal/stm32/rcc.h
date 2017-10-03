#pragma once

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/spi.h>

template<uint32_t periph>
class RCC {
protected:
    static void enableClock() {
        rcc_periph_clock_enable(clock);        
    }
    static void disableClock() {
        rcc_periph_clock_disable(clock);
    }
    const static rcc_periph_clken clock;
};


template<> const rcc_periph_clken RCC<GPIOA>::clock = RCC_GPIOA;
template<> const rcc_periph_clken RCC<GPIOB>::clock = RCC_GPIOB;
template<> const rcc_periph_clken RCC<GPIOC>::clock = RCC_GPIOC;
template<> const rcc_periph_clken RCC<GPIOD>::clock = RCC_GPIOD;
template<> const rcc_periph_clken RCC<GPIOF>::clock = RCC_GPIOF;

template<> const rcc_periph_clken RCC<SPI1>::clock = RCC_SPI1;
template<> const rcc_periph_clken RCC<SPI2>::clock = RCC_SPI2;
template<> const rcc_periph_clken RCC<I2C1>::clock = RCC_I2C1;
template<> const rcc_periph_clken RCC<I2C2>::clock = RCC_I2C2;
