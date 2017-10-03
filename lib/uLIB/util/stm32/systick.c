#include "../../../uLIB/util/stm32/systick.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/systick.h>


#define SYSTICK_HZ      1000

void systick_setup(void)
{
    systick_set_reload(rcc_ahb_frequency / SYSTICK_HZ);
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    systick_counter_enable();
    systick_interrupt_enable();
}

volatile uint32_t system_millis;

void delay(uint32_t delay)
{
    uint32_t wake = system_millis + delay;
    while (wake > system_millis)
    {
        // do nothing
    }
}

void sys_tick_handler(void)
{
    system_millis++;
}

uint32_t get_millis_elapsed(void)
{
    return system_millis;
}
