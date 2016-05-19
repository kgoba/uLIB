#pragma once

#include "util.hh"

#include <avr/io.h>
#include <avr/sleep.h>

class SysClk {
public:
	static void disablePrescaler() {
		CLKPR = (1 << CLKPCE);
		CLKPR = 0;
	}
	static void sleep() {

	}
};

template<typename port, byte pin>
class IOPin {
public:
  typedef port Port;
    
	static void set() {
		port::set(1 << pin);
	}
	static void clear() {
		port::clear(1 << pin);
	}
	static void toggle() {
		port::toggle(1 << pin);
	}
	static byte read() {
		return port::read(1 << pin);
	}
	static void enableOutput() {
		port::enableOutput(1 << pin);
	}
	static void disableOutput() {
		port::disableOutput(1 << pin);
	}
  static void setPCMask() {
    port::setPCMask(1 << pin);
  }
};

#ifdef PORTB
class PortB {
public:
	typedef IOPin<PortB, 0> pin0;
	typedef IOPin<PortB, 1> pin1;
	typedef IOPin<PortB, 2> pin2;
	typedef IOPin<PortB, 3> pin3;
	typedef IOPin<PortB, 4> pin4;
	typedef IOPin<PortB, 5> pin5;
	typedef IOPin<PortB, 6> pin6;
	typedef IOPin<PortB, 7> pin7;

	static void set(byte mask = 0xFF) {
		(PORTB) |= mask;
	}
	static void clear(byte mask = 0xFF) {
		(PORTB) &= ~mask;
	}
	static void toggle(byte mask = 0xFF) {
		(PORTB) ^= mask;
	}
	static byte read(byte mask = 0xFF) {
		return (PINB) & mask;
	}
	static void enableOutput(byte mask = 0xFF) {
		(DDRB) |= mask;
	}
	static void disableOutput(byte mask = 0xFF) {
		(DDRB) &= ~mask;
	}
	static void enablePCInterrupt(byte mask = 0xFF) {
		bit_set(PCICR, PCIE0);
	}
	static void disablePCInterrupt(byte mask = 0xFF) {
		bit_clear(PCICR, PCIE0);
	}
	static void setPCMask(byte mask = 0xFF) {
		(PCMSK0) |= mask;
	}
	static void clearPCMask(byte mask = 0xFF) {
		(PCMSK0) &= ~mask;
	}
};
#endif

#ifdef PORTC
class PortC {
public:
	typedef IOPin<PortC, 6> pin6;
	typedef IOPin<PortC, 7> pin7;

	static void set(byte mask = 0xFF) {
		(PORTC) |= mask;
	}
	static void clear(byte mask = 0xFF) {
		(PORTC) &= ~mask;
	}
	static void toggle(byte mask = 0xFF) {
		(PORTC) ^= mask;
	}
	static byte read(byte mask = 0xFF) {
		return (PINC) & mask;
	}
	static void enableOutput(byte mask = 0xFF) {
		(DDRC) |= mask;
	}
	static void disableOutput(byte mask = 0xFF) {
		(DDRC) &= ~mask;
	}
};
#endif

typedef PortB::pin4 ArduPin8;
typedef PortC::pin7 ArduPin13;

/*
template<int pin>
class IOPin {
public:
	static void setup() {
		if (!isValid())
			return;
	}
	static bool getInput() {
		if (!isValid())
			return false;
		return bit_check(*regPIN(), bit());
	}
	static void setOutput(bool high) {
		if (!isValid())
			return;
		if (high)
			bit_set(*regPORT(), bit());
		else
			bit_clear(*regPORT(), bit());
	}
	static bool getOutput() {
		if (!isValid())
			return false;
		return bit_check(*regPORT(), bit());
	}

protected:
	static bool isValid() {
		return (pin <= 22);
	}
	static uint8_t bit() {
		if (pin < 8)
			return pin;
		if (pin < 16)
			return pin - 8;
		if (pin < 24)
			return pin - 16;
		return 0;
	}
	static volatile uint8_t* regDDR() {
		if (pin < 8)
			return &DDRD;
		if (pin < 16)
			return &DDRB;
		if (pin < 24)
			return &DDRC;
		return 0;
	}
	static volatile uint8_t* regPORT() {
		if (pin < 8)
			return &PORTD;
		if (pin < 16)
			return &PORTB;
		if (pin < 24)
			return &PORTC;
		return 0;
	}
	static volatile uint8_t* regPIN() {
		if (pin < 8)
			return &PIND;
		if (pin < 16)
			return &PINB;
		if (pin < 24)
			return &PINC;
		return 0;
	}
	static volatile uint8_t* regPCMSK() {
		if (pin < 8)
			return &PCMSK2;
		if (pin < 16)
			return &PCMSK0;
		if (pin < 24)
			return &PCMSK1;
		return 0;
	}
	static uint8_t pcIntBit() {
		if (pin < 8)
			return PCIE2;
		if (pin < 16)
			return PCIE0;
		if (pin < 24)
			return PCIE1;
	}
};
*/

/*

 Example:

 InputPin<2, kActiveLow, kPullup> pin;      // active low (default high), enable pullup (no pullup by default)
 pin.setup();      //
 if (pin.get()) doSomething();

 */

/*
typedef enum {
	kActiveLow = 0, kActiveHigh = 1
} LogicLevel;
typedef enum {
	kNoPullup = 0, kPullup = 1
} PullupMode;

template<int pin, LogicLevel activeLevel = kActiveHigh, PullupMode pullup =
		kNoPullup>
class InputPin: public IOPin<pin> {
public:

	static void setup() {
		if (!IOPin<pin>::isValid())
			return;
		bit_clear(*IOPin<pin>::regDDR(), IOPin<pin>::bit());
		setPullup(pullup);
	}
	static bool get() {
		if (!IOPin<pin>::isValid())
			return false;
		return (activeLevel == kActiveLow) ^ IOPin<pin>::getInput();
	}
	static void setPullup(bool enabled) {
		IOPin<pin>::setOutput(enabled);
	}
	static void enablePCInt() {
		bit_set(PCICR, IOPin<pin>::pcIntBit());
		bit_set(*IOPin<pin>::regPCMSK(), IOPin<pin>::bit());
	}
	static void disablePCInt() {
		bit_set(PCICR, IOPin<pin>::pcIntBit());
		bit_set(*IOPin<pin>::regPCMSK(), IOPin<pin>::bit());
	}
};

template<int pin, byte debounce, LogicLevel activeLevel = kActiveHigh,
		PullupMode pullup = kNoPullup>
class InputDebouncePin: public InputPin<pin, activeLevel, pullup> {
public:

	static bool get() {
		return (_on && _counter == debounce) || (!_on && _counter > 0);
	}

	static void update() {
		bool on = InputPin<pin, activeLevel, pullup>::get();
		if (on) {
			if (_counter < debounce)
				_counter++;
			_on = true;
		} else {
			if (_counter > 0)
				_counter--;
			_on = false;
		}
	}

private:
	static byte _counter;
	static bool _on;
};

template<int pin, byte debounce, LogicLevel activeLevel, PullupMode pullup>
byte InputDebouncePin<pin, debounce, activeLevel, pullup>::_counter = 0;

template<int pin, byte debounce, LogicLevel activeLevel, PullupMode pullup>
bool InputDebouncePin<pin, debounce, activeLevel, pullup>::_on = false;

*/

/*

 Example:

 OutputPin<2, kActiveHigh> pin;      // active low (default high)
 pin.setup(true);             // initial on
 doSomething();
 pin.off();                  // switch off

 */

/*

template<int pin, LogicLevel activeLevel = kActiveHigh>
class OutputPin: public IOPin<pin> {
public:
	static void setup(bool initialOn = false) {
		if (!IOPin<pin>::isValid())
			return;
		bit_set(*IOPin<pin>::regDDR(), IOPin<pin>::bit());
		set(initialOn);
	}
	static void set(bool on) {
		IOPin<pin>::setOutput(on ^ (activeLevel == kActiveLow));
	}
	static void on() {
		set(true);
	}
	static void off() {
		set(false);
	}
	static bool get() {
		return IOPin<pin>::getOutput() ^ (activeLevel == kActiveLow);
	}
};

*/
