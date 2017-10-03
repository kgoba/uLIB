#pragma once

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/spi.h>

#include "../../../uLIB/hal/stm32/rcc.h"

template<uint32_t i2c, typename Pin_sda, typename Pin_scl>
class I2C : public RCC<i2c> {
public:
    static void begin() {
        DigitalAF<Pin_sda, i2c>::begin(IODirection::OutputOD);
        DigitalAF<Pin_scl, i2c>::begin(IODirection::OutputOD);

        // Setup clock source:
        // - I2C1: HSI@8Mhz (default) / SYSCLK
        // - I2C2: PCLK(APB)
        //rcc_set_i2c_clock_hsi(i2c);
        RCC<i2c>::enableClock();

        i2c_reset(i2c);
        i2c_peripheral_disable(i2c);

        //i2c_enable_analog_filter(i2c);    //configure ANFOFF DNF[3:0] in CR1
        //i2c_set_digital_filter(i2c, I2C_CR1_DNF_DISABLED);

        normal_speed(); /* Configure normal speed (100 kHz) */

#if defined(STM32F1)
#else
        /* Enable 7 bit addresses (needed on F1). */
        i2c_set_7bit_addr_mode(i2c);
#endif

        /* Enable I2C periph. */
        i2c_peripheral_enable(i2c);
    }
      
    static void shutdown() {
        i2c_peripheral_disable(i2c);
        RCC<i2c>::disableClock();
    }
    
    static void normal_speed() {
#if defined(STM32F1)
#else
        // Setup 100 kHz rate @ 8 MHz clock
        i2c_set_prescaler(i2c, 1);
        i2c_set_data_setup_time(i2c, 4);   // 1.25 us
        i2c_set_data_hold_time(i2c, 2);    // 0.5 us
        i2c_set_scl_high_period(i2c, 16);  // 4.0 us
        i2c_set_scl_low_period(i2c, 20);   // 5.0 us
#endif
    }
    
    static void write(uint16_t address, const uint8_t *data, int length, bool repeated=false) {
        i2c_transfer7(i2c, address, (uint8_t *)data, length, 0, 0);
    }
    
    static void read(uint16_t address, uint8_t *data, int length, bool repeated=false) {
        i2c_transfer7(i2c, address, 0, 0, data, length);
    }
};
