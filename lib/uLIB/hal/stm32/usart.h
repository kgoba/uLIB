#pragma once

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/spi.h>

#include "../../../uLIB/hal/stm32/rcc.h"

template<uint32_t usart, typename Pin_tx, typename Pin_rx>
class USART : public RCC<usart> {
public:
    static void begin() {
        DigitalAF<Pin_tx, usart>::begin();
        DigitalAF<Pin_rx, usart>::begin();

        RCC<usart>::enableClock();

        usart_set_baudrate(usart, 4800);
        usart_set_databits(usart, 8);
        usart_set_parity(usart, USART_PARITY_NONE);
        usart_set_stopbits(usart, USART_CR2_STOP_1_0BIT);
        usart_set_mode(usart, USART_MODE_TX_RX);
        usart_set_flow_control(usart, USART_FLOWCONTROL_NONE);

        /* Finally enable the USART. */
        usart_enable(usart);
    }

    static void shutdown() {
        usart_disable(usart);
        RCC<usart>::disableClock();
    }

    static void baudrate(uint32_t baud) {
        usart_set_baudrate(usart, baud);
    }

    static int getc() {
        if (usart_get_flag(usart, USART_ISR_RXNE) return -1;    // or USART_SR_RXNE
        return usart_recv(usart);
    }
    
    static int putc(int c) {
        usart_wait_send_ready(usart);
        usart_send(usart, c);
    }
};
