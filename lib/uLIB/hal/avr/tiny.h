#pragma once
#include <avr/io.h>
#include <util/delay.h>

#include <stdint.h>
#include <stdlib.h>

#define bit_set(x, bit)   ((x) |= (1 << (bit)))
#define bit_clear(x, bit)   ((x) &= ~(1 << (bit)))
#define bit_check(x, bit)   ((x) & (1 << (bit)))

template<class Port, int pinIndex>
class IOPin {
public:
	void set() {
		Port::set(1 << pinIndex);
	}
	void clear() {
		Port::clear(1 << pinIndex);
	}
	void modeOutput() {
		Port::setDirection(1 << pinIndex);
	}
	void modeInput() {
		Port::clearDirection(1 << pinIndex);
	}
	uint8_t read() {
		return Port::read(1 << pinIndex);
	}
};

class PortB {
public:
	typedef IOPin<PortB, 0> pin0;
	typedef IOPin<PortB, 1> pin1;
	typedef IOPin<PortB, 2> pin2;
	typedef IOPin<PortB, 3> pin3;
	typedef IOPin<PortB, 4> pin4;

	static void set(uint8_t mask) {
		PORTB |= mask;
	}
	static void clear(uint8_t mask) {
		PORTB &= ~mask;
	}
	static void setDirection(uint8_t mask) {
		DDRB |= mask;
	}
	static void clearDirection(uint8_t mask) {
		DDRB &= ~mask;
	}
	static uint8_t read(uint8_t mask) {
		return PINB & mask;
	}
};

template<class Pin>
class DigitalOut: public Pin {
public:
	DigitalOut() {
		Pin::modeOutput();
	}

	void high() {
		Pin::set();
	}

	void low() {
		Pin::clear();
	}
};

template<class Pin>
class DigitalWeakOut: public Pin {
public:
	//DigitalWeakOut() {
	//Pin::modeOutput();
	//}

	void high() {
		Pin::modeInput();
		Pin::set();
	}

	void low() {
		Pin::modeOutput();
		Pin::clear();
	}
};

template<class Pin, int baudrate>
class SoftwareSerialOut {
public:
	void begin() {
		pin.high();
	}

	void write(uint8_t b) {
		pin.low();
		_delay_us(1000000UL / baudrate);
		for (uint8_t mask = 0x01; mask; mask <<= 1) {
			if (b & mask) {
				pin.high();
			} else {
				pin.low();
			}
			_delay_us(1000000UL / baudrate);
		}
		pin.high();
		_delay_us(1000000UL / baudrate);
	}

private:
	Pin pin;
};

class NullOutput {
public:
	void begin() {

	}

	void write(uint8_t b) {

	}
};

template<class BinaryStream>
class TextOutStream: public BinaryStream {
public:

	TextOutStream<BinaryStream> & print(char c) {
		BinaryStream::write((uint8_t) c);
		return *this;
	}

	TextOutStream<BinaryStream> & print(const char *c) {
		while (*c) {
			print(*c++);
		}
		return *this;
	}

	TextOutStream<BinaryStream> & print(uint8_t number) {
		bool nonzero = false;
		uint8_t digit;
		for (digit = 0; number >= 100; number -= 100)
			digit++;
		if (nonzero || digit != 0) {
			print((char) ('0' + digit));
			nonzero = true;
		}
		for (digit = 0; number >= 10; number -= 10)
			digit++;
		if (nonzero || digit != 0) {
			print((char) ('0' + digit));
			nonzero = true;
		}
		for (digit = 0; number >= 1; number -= 1)
			digit++;
		print((char) ('0' + digit));
		return *this;
	}

	TextOutStream<BinaryStream> & print(uint16_t number) {
		bool nonzero = false;
		uint8_t digit;
		for (digit = 0; number >= 10000; number -= 10000)
			digit++;
		if (nonzero || digit != 0) {
			print((char) ('0' + digit));
			nonzero = true;
		}
		for (digit = 0; number >= 1000; number -= 1000)
			digit++;
		if (nonzero || digit != 0) {
			print((char) ('0' + digit));
			nonzero = true;
		}
		for (digit = 0; number >= 100; number -= 100)
			digit++;
		if (nonzero || digit != 0) {
			print((char) ('0' + digit));
			nonzero = true;
		}
		for (digit = 0; number >= 10; number -= 10)
			digit++;
		if (nonzero || digit != 0) {
			print((char) ('0' + digit));
			nonzero = true;
		}
		for (digit = 0; number >= 1; number -= 1)
			digit++;
		print((char) ('0' + digit));
		return *this;
	}

	TextOutStream<BinaryStream> & print(int16_t number) {
		if (number < 0) {
			print('-');
			print((uint16_t) (-number));
		} else {
			print((uint16_t) (number));
		}
		return *this;
	}

	TextOutStream<BinaryStream> & print(unsigned long number) {
		if (number < 0) {
			print('-');
			print((unsigned long) (-number));
		} else {
			print((unsigned long) (number));
		}
		return *this;
	}
};

template<class Stream, typename T>
Stream & operator <<(Stream &str, const T &x) {
	return str.print(x);
}

template<class PinSDA, class PinSCL>
class SoftwareI2C {
public:
	void begin() {
		sda.modeInput();
		scl.modeInput();
		sda.low();
		scl.low();
	}
	void beginTransmission(uint8_t addr) {
		start();
		transmit(addr << 1);
		waitACK();
	}
	size_t write(uint8_t b) {
		if (wasACK) {
			transmit(b);
			waitACK();
			return 1;
		}
		return 0;
	}
	inline size_t write(uint8_t* d, uint8_t n) {
		uint16_t i;
		for (i = 0; i < n; i++)
			write(d[i]);
		return (size_t) n;
	}
	inline size_t write(unsigned long n) {
		return write((uint8_t) n);
	}
	inline size_t write(long n) {
		return write((uint8_t) n);
	}
	inline size_t write(unsigned int n) {
		return write((uint8_t) n);
	}
	inline size_t write(int n) {
		return write((uint8_t) n);
	}
	void send(uint8_t b) {
		write(b);
	}
	void send(uint8_t *d, uint8_t n) {
		write(d, n);
	}
	void send(int n) {
		write((uint8_t) n);
	}
	uint8_t endTransmission() {
		return endTransmission(1);
	}
	uint8_t endTransmission(uint8_t doStop) {
		stop();
		return wasACK ? 0 : 1;
	}
	uint8_t requestFrom(uint8_t addr, uint8_t n) {
		start();
		transmit((addr << 1) | 1);
		waitACK();
		if (wasACK) {
			count = n;
		} else {
			count = 0;
		}
		if (count == 0)
			stop();
		return wasACK ? 0 : 1;
	}
	int read() {
		if (count == 0)
			return -1;
		uint8_t b = 0;
		for (uint8_t mask = 0x80; mask; mask >>= 1) {
			if (readBit())
				b |= mask;
		}
		count--;
		writeBit(count != 0);
		if (count == 0) {
			stop();
		}
		return b;
	}
	int available() {
		return count;
	}
	int peek(void);
	void flush(void);
	uint8_t receive(void) {
		int c = read();
		if (c < 0)
			return 0;
		return c;
	}

private:
	void start() {
		sda.modeOutput();
		_delay_us(10);
		scl.modeOutput();
		_delay_us(10);
	}

	void stop() {
		scl.modeInput();
		_delay_us(10);
		sda.modeInput();
		_delay_us(10);
	}

	void transmit(uint8_t b) {
		for (uint8_t mask = 0x80; mask; mask >>= 1) {
			writeBit(b & mask);
		}
	}

	void writeBit(bool bit) {
		if (bit)
			sda.modeInput();
		else
			sda.modeOutput();
		_delay_us(5);
		scl.modeInput();
		_delay_us(10);
		scl.modeOutput();
		_delay_us(5);
	}

	bool readBit() {
		sda.modeInput();
		_delay_us(5);
		scl.modeInput();
		_delay_us(5);
		uint8_t result = sda.read();
		_delay_us(5);
		scl.modeOutput();
		_delay_us(5);
		return result;
	}

	void waitACK() {
		wasACK = (readBit() == 0);
	}

	PinSDA sda;
	PinSCL scl;
	bool wasACK;
	uint8_t count;
	//uint8_t received[8];
};
