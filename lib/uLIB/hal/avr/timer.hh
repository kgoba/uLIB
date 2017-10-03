#pragma once

#include <avr/io.h>
#include "../uLIB/pins.hh"
#include "../uLIB/util.hh"

#define TIMER0_SETUP(mode, prescaler) \
{ \
  uint8_t mode_b = ((mode & 0x04) << 1); \
  switch(prescaler) { \
    case 0UL: TCCR0B = TIMER0_STOP | mode_b; break; \
    case 1UL: TCCR0B = TIMER0_PRE1 | mode_b; break; \
    case 8UL: TCCR0B = TIMER0_PRE8 | mode_b; break; \
    case 64UL: TCCR0B = TIMER0_PRE64 | mode_b; break; \
    case 256UL: TCCR0B = TIMER0_PRE256 | mode_b; break; \
    case 1024UL: TCCR0B = TIMER0_PRE1024 | mode_b; break; \
  } \
  TCCR0A = (mode & 0x03); \
}

#define TIMER0_NORMAL         0
#define TIMER0_PWM_PHASE      1
#define TIMER0_CTC            2
#define TIMER0_FAST_PWM       3
#define TIMER0_PWM_PHASE_A    5
#define TIMER0_FAST_PWM_A     7

#define TIMER0_STOP       0
#define TIMER0_PRE1       (1 << CS00)
#define TIMER0_PRE8       (1 << CS01)
#define TIMER0_PRE64      ((1 << CS00) | (1 << CS01))
#define TIMER0_PRE256     (1 << CS02)
#define TIMER0_PRE1024    ((1 << CS00) | (1 << CS02))
#define TIMER0_T0_FALL    ((1 << CS01) | (1 << CS02))
#define TIMER0_T0_RISE    ((1 << CS00) | (1 << CS01) | (1 << CS02))

#define TIMER0_PRESCALER(freq)        (F_CPU/(freq) < 256 ? 1UL : (F_CPU/(8UL*freq) < 256 ? 8UL : (F_CPU/(64UL*freq) < 256 ? 64UL : \
                                      (F_CPU/(256UL*freq) < 256 ? 256UL : (F_CPU/(1024UL*freq) < 256 ? 1024UL : 0 )))))

#define TIMER0_COUNTS(freq)           (byte)(F_CPU / (TIMER0_PRESCALER(freq) * freq))


#define TIMER2_SETUP(mode, prescaler) \
{ \
  TCCR2A = (mode & 0x03); \
  uint8_t mode_b = ((mode & 0x04) << 1); \
  switch(prescaler) { \
    case 0UL: TCCR2B = TIMER2_STOP | mode_b; break; \
    case 1UL: TCCR2B = TIMER2_PRE1 | mode_b; break; \
    case 8UL: TCCR2B = TIMER2_PRE8 | mode_b; break; \
    case 32UL: TCCR2B = TIMER2_PRE32 | mode_b; break; \
    case 64UL: TCCR2B = TIMER2_PRE64 | mode_b; break; \
    case 128UL: TCCR2B = TIMER2_PRE128 | mode_b; break; \
    case 256UL: TCCR2B = TIMER2_PRE256 | mode_b; break; \
    case 1024UL: TCCR2B = TIMER2_PRE1024 | mode_b; break; \
  } \
}

#define TIMER2_NORMAL         0
#define TIMER2_PWM_PHASE      1
#define TIMER2_CTC            2
#define TIMER2_FAST_PWM       3
#define TIMER2_PWM_PHASE_A    5
#define TIMER2_FAST_PWM_A     7

#define TIMER2_STOP       0
#define TIMER2_PRE1       (1 << CS20)
#define TIMER2_PRE8       (1 << CS21)
#define TIMER2_PRE32      ((1 << CS20) | (1 << CS21))
#define TIMER2_PRE64      (1 << CS22)
#define TIMER2_PRE128     ((1 << CS20) | (1 << CS22))
#define TIMER2_PRE256     ((1 << CS21) | (1 << CS22))
#define TIMER2_PRE1024    ((1 << CS20) | (1 << CS21) | (1 << CS22))

#define TIMER2_PRESCALER(freq)        (F_CPU/(freq) < 256 ? 1UL : (F_CPU/(8UL*freq) < 256 ? 8UL : (F_CPU/(32UL*freq) < 256 ? 32UL : (F_CPU/(64UL*freq) < 256 ? 64UL : \
                                      (F_CPU/(128UL*freq) < 256 ? 128UL : (F_CPU/(256UL*freq) < 256 ? 256UL : (F_CPU/(1024UL*freq) < 256 ? 1024UL : 0 )))))))

#define TIMER2_COUNTS(freq)           (byte)(F_CPU / (TIMER2_PRESCALER(freq) * freq))






#define TIMER1_SETUP(mode, prescaler) \
{ \
  TCCR1A = (mode & 0x03); \
  uint8_t mode_b = ((mode & 0x0C) << 1); \
  switch(prescaler) { \
    case 0UL: TCCR1B = TIMER1_STOP | mode_b; break; \
    case 1UL: TCCR1B = TIMER1_PRE1 | mode_b; break; \
    case 8UL: TCCR1B = TIMER1_PRE8 | mode_b; break; \
    case 64UL: TCCR1B = TIMER1_PRE64 | mode_b; break; \
    case 256UL: TCCR1B = TIMER1_PRE256 | mode_b; break; \
    case 1024UL: TCCR1B = TIMER1_PRE1024 | mode_b; break; \
  } \
}

#define TIMER1_NORMAL     0
#define TIMER1_PWM_P8     1
#define TIMER1_PWM_P9     2
#define TIMER1_PWM_P10    3
#define TIMER1_CTC_OCR    4
#define TIMER1_PWM_F8     5
#define TIMER1_PWM_F9     6
#define TIMER1_PWM_F10    7
#define TIMER1_PWM_PF_ICR   8
#define TIMER1_PWM_PF_OCR   9
#define TIMER1_PWM_P_ICR    10
#define TIMER1_PWM_P_OCR    11
#define TIMER1_CTC_ICR      12
#define TIMER1_PWM_FAST_ICR   14
#define TIMER1_PWM_FAST_OCR   15

#define TIMER1_STOP       0
#define TIMER1_PRE1       1
#define TIMER1_PRE8       2
#define TIMER1_PRE64      3
#define TIMER1_PRE256     4
#define TIMER1_PRE1024    5
#define TIMER1_T1_FALL    6
#define TIMER1_T1_RISE    7

#define TIMER1_PRESCALER(freq)        (F_CPU/(freq) < 65536 ? 1UL : (F_CPU/(8UL*freq) < 65536 ? 8UL : (F_CPU/(64UL*freq) < 65536 ? 64UL : \
                                      (F_CPU/(256UL*freq) < 65536 ? 256UL : (F_CPU/(1024UL*freq) < 65536 ? 1024UL : 0 )))))

#define TIMER1_COUNTS(freq)           (word)(F_CPU / (TIMER1_PRESCALER(freq) * freq))

/*
void timer2Setup(word frequency) {
  long div32 = F_CPU / frequency;
  if ((div32 >> 16) == 0) {
    word div16 = divisor;
    
  }
  // Setup Timer2
  // Set CTC mode, TOP = OCRA, prescaler 1024
  // Overflow 125Hz (8ms), overflow interrupt enabled
  TCCR2A = (1 << WGM21) | (1 << WGM20);
  TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20) | (1 << WGM22);
  TIMSK2 = (1 << TOIE2); 
  OCR2A = (byte)(F_CPU / (1024UL * TICK_FREQ)) - 1;
}
*/

template<typename cnt_t>
class TimerBase {
public:
	static void setClock();

	static void setCounter(cnt_t value);
	static cnt_t getCounter();
};



class Generic8BitTimer {
public:
	enum ClockSource {
		eStop		  = 0,
		eDiv1 		= 1,
		eDiv8		  = 2,
		eDiv64 		= 3,
		eDiv256		= 4,
		eDiv1024 	= 5,
		eT0Fall 	= 6,
		eT0Rise 	= 7
	};

	enum Mode {
		eNormal 			    = 0,
		ePhaseCorrectPWM8	= 1,
		eCTC				      = 2,
		eFastPWM8			    = 3,
		ePhaseCorrectPWM	= 5,
		eFastPWM			    = 7
	};

	enum OutputMode {
		eNone				  = 0,
		eToggle				= 1,
		eClear				= 2,
		eSet				  = 3
	};

	static ClockSource findClock(long freq) {
		ClockSource source;
		source = (F_CPU/(freq) < 256 ? eDiv1 :
				 (F_CPU/(8*freq) < 256 ? eDiv8 :
				 (F_CPU/(64*freq) < 256 ? eDiv64 :
                 (F_CPU/(256*freq) < 256 ? eDiv256 :
                 (F_CPU/(1024*freq) < 256 ? eDiv1024 : eStop )))));
		return source;
	}

	static word getPrescalerFactor(ClockSource source) {
		switch (source) {
		case eDiv1   : return 1; break;
		case eDiv8   : return 8; break;
		case eDiv64  : return 64; break;
		case eDiv256 : return 256; break;
		case eDiv1024: return 1024; break;
		default		 : return 0; break;
		}
	}
};

class Timer0 : public Generic8BitTimer {
public:
	//static IOPin<PortB, 5>		pinA;
	//static IOPin<PortB, 6>		pinB;

	static void setup(Mode mode = eNormal, ClockSource source = eStop,
					  OutputMode channelA = eNone, OutputMode channelB = eNone)
	{
		TCCR0A = (mode & 0x03);
		uint8_t mode_b = ((mode & 0x04) << 1);
		TCCR0B = source | mode_b;
	}
	//static void setClock(ClockSource source);
	//static void setMode(Mode mode);

	static void setChannelA(byte mode) {
		TCCR0A = (TCCR0A & 0x3F) | (mode << 6);
	}
	static void setChannelB(byte mode) {
		TCCR0A = (TCCR0A & 0xCF) | (mode << 4);
	}

	static void setCounter(byte value) { TCNT0 = value; }
	static byte getCounter() { return TCNT0; }

	static void setCompareA(byte value) { OCR0A = value; }
	static void setCompareB(byte value) { OCR0B = value; }

	static void setOverflowInterrupt(bool enabled) {
	  if (enabled) bit_set(TIMSK0, TOIE0);
    else bit_clear(TIMSK0, TOIE0);
  }
	static void setCompareAInterrupt(bool enabled);
	static void setCompareBInterrupt(bool enabled);
	static void setCaptureInterrupt(bool enabled);
};

class Timer2 : public Generic8BitTimer {
public:
	//static IOPin<PortB, 5>		pinA;
	//static IOPin<PortB, 6>		pinB;

	static void setup(Mode mode = eNormal, ClockSource source = eStop,
					  OutputMode channelA = eNone, OutputMode channelB = eNone)
	{
		TCCR2A = (mode & 0x03);
		uint8_t mode_b = ((mode & 0x04) << 1);
		TCCR2B = source | mode_b;
	}
	//static void setClock(ClockSource source);
	//static void setMode(Mode mode);

	static void setChannelA(byte mode) {
		TCCR2A = (TCCR2A & 0x3F) | (mode << 6);
	}
	static void setChannelB(byte mode) {
		TCCR2A = (TCCR2A & 0xCF) | (mode << 4);
	}

	static void setCounter(byte value) { TCNT2 = value; }
	static byte getCounter() { return TCNT2; }

	static void setCompareA(byte value) { OCR2A = value; }
	static void setCompareB(byte value) { OCR2B = value; }

	static void setOverflowInterrupt(bool enabled) {
	  if (enabled) bit_set(TIMSK2, TOIE2);
    else bit_clear(TIMSK2, TOIE2);
	}
	static void setCompareAInterrupt(bool enabled);
	static void setCompareBInterrupt(bool enabled);
	static void setCaptureInterrupt(bool enabled);
};





template<class Base, long frequency>
class PWMSingle : Base {
	static void setup() {
		typename Base::ClockSource source = Base::findClock(frequency);
		Base::setup(Base::eFastPWM, source);
		word div = Base::getPrescaleFactor(source);
		if (div > 0 && div < frequency) {
			byte top = (byte)(frequency / div - 1);
			Base::setCompareA(top);
		}
	}

	static void enable() {
		Base::pinB.enableOutput();
	}

	static void disable() {
		Base::pinB.disableOutput();
	}
};

template<class Base, bool phaseCorrect = false>
class PWMDual : Base {
public:
	static void setup() {
		//Base::setMode(phaseCorrect ? Base::ePhaseCorrectPWM8 : Base::eFastPWM8);
	}

	static void enable() {
		// setup pins for output
	}

	static void disable() {

	}

	static void setPWMA(byte value) {
		Base::setCompareA(value);
	}

	static void setPWMB(byte value) {
		Base::setCompareB(value);
	}
};

/*
#include "pins.hh"

template<int pin>
class PWMPin: public OutputPin<pin> {
public:
	static void setup() {
		OutputPin<pin>::setup();
	}

	static void setToggleFrequency(uint32_t freq, uint8_t mode = TIMER0_CTC) {
		TCCR0B = TIMER0_STOP;
		if (freq == 0)
			return;

		OCR0A = TIMER0_COUNTS(freq * 2) - 1; //((31250 + freq / 2) / freq) - 1;

		TIMER0_SETUP(mode, TIMER0_PRESCALER(freq * 2));
	}

	static void setFrequency(uint32_t freq, uint8_t mode = TIMER0_CTC) {
		TCCR0B = TIMER0_STOP;
		if (freq == 0)
			return;

		OCR0A = TIMER0_COUNTS(freq) - 1; //((31250 + freq / 2) / freq) - 1;

		TIMER0_SETUP(mode, TIMER0_PRESCALER(freq));
	}

	static void setPWMPercent(uint32_t freq, uint8_t percent, uint8_t mode =
			TIMER0_FAST_PWM) {
		volatile uint8_t *reg;
		switch (pin) {
		case 5:
			reg = &OCR0B;
			break;
		case 6:
			reg = &OCR0A;
			break;
		default:
			return;
		}
		*reg = (percent * 255 + 50) / 100;
		TIMER0_SETUP(mode, TIMER0_PRESCALER(freq));
	}

	static void enableToggle() {
		enable(1);
	}

	static void enable(uint8_t mode = 2) {
		volatile uint8_t *reg;

		switch (pin) {
		case 5:
			mode <<= 4;
			break;
		case 6:
			mode <<= 6;
			break;
		default:
			return;
		}
		switch (pin) {
		case 5:
		case 6:
			reg = &TCCR0A;
			break;
		default:
			return;
		}
		*reg = ((*reg) & 0x0F) | mode;
	}

	static void disable() {
		enable(0);
	}
};

template<bool phaseCorrect = false>
class PWM0Dual {
public:
	static void setup(uint32_t frequency = 0) {
		OutputPin<5>::setup();
		OutputPin<6>::setup();
		if (frequency)
			setFrequency(frequency);
	}

	static void setFrequency(uint32_t frequency) {
		uint8_t mode;
		if (phaseCorrect)
			mode = TIMER0_PWM_PHASE;
		else
			mode = TIMER0_FAST_PWM;

		if (frequency > 0) {
			TIMER0_SETUP(mode, TIMER0_PRESCALER(frequency));
		} else {
			TIMER0_SETUP(mode, TIMER0_STOP);
		}
	}

	static void setToggle(bool channelA, bool channelB) {
		TCCR0A = (TCCR0A & 0xF0) | (channelA ? (1 << COM0A0) : 0)
				| (channelB ? (1 << COM0B0) : 0);
	}

	static void enable(bool inverted) {

	}
};
*/