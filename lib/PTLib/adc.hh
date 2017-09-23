#pragma once

#include "util.hh"

#include <avr/io.h>
#include <avr/sleep.h>

template<class ADCPeriph, byte channel>
class ADCChannel {
public:
	void select() {
		ADCPeriph::setChannel(channel);
	}
	word value() {
		return ADC;
	}
};

class AnalogConverter {
public:
	typedef ADCChannel<AnalogConverter, 0> ADC0;
	typedef ADCChannel<AnalogConverter, 1> ADC1;
	typedef ADCChannel<AnalogConverter, 2> ADC2;
	typedef ADCChannel<AnalogConverter, 3> ADC3;
	typedef ADCChannel<AnalogConverter, 15> TempSensor;

	enum Reference {
		ReferenceVCC = 0,
		ReferenceExternal = 1,
		ReferenceInternal1V1 = 2,
		ReferenceInternal2V56 = 6,
		ReferenceInternal2V56WithCap = 7
	};

	enum Prescaler {
		Div2    = 1,
		Div4    = 2,
		Div8	= 3,
		Div16	= 4,
		Div32	= 5,
		Div64	= 6,
		Div128	= 7
	};

	enum TriggerSource {
		FreeRunning = 0,
		TriggerAnalogComparator = 1,
		TriggerExternalINT0 = 2,
		TriggerTimer0OutputCompareA = 3,
		TriggerTimer0Overflow = 4,
		TriggerTimer0OutputCompareB = 5,
		TriggerPinChange = 6
	};

	void setup(Prescaler prescaler = Div2, TriggerSource trigger = FreeRunning) {
		ADCSRA = (byte)prescaler;
		ADCSRB = (byte)trigger;
	}

	void startConversion() {
		bit_set(ADCSRA, ADSC);
	}

	void enable() {
		bit_set(ADCSRA, ADEN);
	}

	void disable() {
		bit_clear(ADCSRA, ADEN);
	}

	void enableInterrupt() {
		bit_set(ADCSRA, ADIE);
	}

	void setMUX(byte channel, Reference reference = ReferenceVCC, bool leftAdjust = false) {
		byte value = 	(channel & 0x0F) |
						(leftAdjust ? (1 << ADLAR) : 0) |
						(reference << 6);
		ADMUX = value;
	}
};
