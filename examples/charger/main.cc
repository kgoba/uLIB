//#include <timer.hh>
#include <pins.hh>

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

// 0 for normal operation, 1 for debug (blink)
#define DEBUG             1

class Timer1 {
public:
	void setup() {
		// Setup Timer1
		TCCR1 = (1 << PWM1A);   // OCR1C = TOP
		TCCR1 |= (1 << CS12) | (1 << CS10); // prescaler /16
		OCR1C = 255;    // set TOP
	}

	enum InterruptType {
		eOverflowInterrupt
	};

	void enableInterrupt(InterruptType type) {
		switch (type) {
		case eOverflowInterrupt: bit_set(TIMSK, TOIE1); break;
		}
	}
};

// Hardware definitions
PortB::pin0 led1;
PortB::pin2 led2;
PortB::pin1 gate;		// OC0B / OC1A
Timer1 timer;

//PWMDual<Timer0> pwm0;

class UI {
public:
	enum LEDState {
		eGREEN,
		eRED,
		eOFF
	};
	void setup() {
		led1.enableOutput();
		led2.enableOutput();
	}
	void set(LEDState state) {
		switch (state) {
			case eGREEN:
			{
				led1.clear();
				led2.set();
				break;
			}
			case eRED:
			{
				led1.set();
				led2.clear();
				break;
			}
			case eOFF:
			{
				led1.clear();
				led2.clear();
			}
		}
	}

private:
};

SysClk sysclk;
UI ui;

int main()
{
//  pwm0.setup();

//  led1.set();
//  led1.clear();

	// Disable system clock prescaler
	sysclk.disablePrescaler();
	timer.setup();
	timer.enableInterrupt(timer.eOverflowInterrupt);
	ui.setup();

	//led1.clear();
	//led2.set();
//  pwm0.setPWMA(23);
//  pwm0.setPWMB(24);

  byte b = 72;
    
  set_sleep_mode(SLEEP_MODE_IDLE);
  sei();
  while (true) {
    sleep_mode();
  }
}

ISR(TIMER1_OVF_vect) {
	static word counter = 0;
	static byte phase = 0;
	counter++;
	if (counter >= 1000) {
		counter = 0;
		if (phase)
			ui.set(UI::eRED);
		else
			ui.set(UI::eOFF);
		phase = !phase;
	}
	return;
}
