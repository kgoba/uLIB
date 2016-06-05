#pragma once

#include "util.hh"
#include "queue.hh"

#include <avr/interrupt.h>
#include <util/delay.h>


template<long baudrate, bool use2x = false>
class SimpleSerial {
public:
  
  static void setup() {
    UBRR0 = (F_CPU + 8UL * baudrate) / (16UL * baudrate) - 1;

    if (use2x) {
        bit_set(UCSR0A, U2X0);
    }
    
    UCSR0C = bit_mask2(UCSZ01, UCSZ00); /* 8-bit data */
  }
  
  static void enable() {
    UCSR0B = bit_mask2(RXEN0, TXEN0);
  }
  
  static void disable() {
    UCSR0B = 0;
  }

  static bool readByte(byte &b, bool noCheck = false) {
    if (!noCheck && !isRXComplete()) return false;
    b = UDR0;
    return true;
  }

  static bool writeByte(byte b, bool noCheck = false) {
    if (!noCheck && !isTXReady()) return false;
    UDR0 = b;
    return true;
  }  

  static bool isRXComplete() {
    return bit_check(UCSR0A, RXC0);
  }
  
  static bool isTXReady() {
    return bit_check(UCSR0A, UDRE0);
  }
  
  static bool isTXComplete() {
    return bit_check(UCSR0A, TXC0);
  }

  static void setRXCompleteInterrupt(bool enabled) {
    if (enabled) bit_set(UCSR0B, RXCIE0);
    else bit_clear(UCSR0B, RXCIE0);
  }

  static void setTXReadyInterrupt(bool enabled) {
    if (enabled) bit_set(UCSR0B, UDRIE0);
    else bit_clear(UCSR0B, UDRIE0);
  }

  static void setTXCompleteInterrupt(bool enabled) {
    if (enabled) bit_set(UCSR0B, TXCIE0);
    else bit_clear(UCSR0B, TXCIE0);
  }
};


class WaitableBase {
public:
  static const word IMMEDIATE     = 0;
  static const word WAIT_FOREVER  = 0xFFFF;
};

template<class Serial, word rxTimeout = WaitableBase::WAIT_FOREVER, word txTimeout = WaitableBase::WAIT_FOREVER>
class WaitableSerial : public Serial, public WaitableBase
{
public:

  static byte readByte(byte &b, word timeout = rxTimeout) {
    while (!Serial::readByte(b)) {
      if (timeout == IMMEDIATE) 
        return false;
      if (timeout != WAIT_FOREVER) {
        timeout--;
        _delay_us(10);
      }
    }
    return true;
  }
  
  static byte writeByte(byte b, word timeout = txTimeout) {
    while (!Serial::writeByte(b)) {
      if (timeout == IMMEDIATE) 
        return false;
      if (timeout != WAIT_FOREVER) {
        timeout--;
        _delay_us(10);
      }
    }
    return true;
  }
};

template<class Serial, int rxBufferSize = 32, int txBufferSize = 32>
class BufferedSerial : private Serial 
{
public:
  static void setup() {
    Serial::setup();
    rxQueue.clear();
    txQueue.clear();
  }
  
  static void enable() {
    Serial::enable();
    Serial::setRXCompleteInterrupt(true);
  }
  
  static void disable() {
    Serial::disable();
  }
    
  static void flushRX() {
    rxQueue.clear();
    rxOverrun = false;
  }
  
  static void onRXComplete() {
    byte b;
    Serial::readByte(b, true);
    if (!rxQueue.push(b)) 
      rxOverrun = true;
  }
  
  static void onTXReady() {
    byte top;
    if (txQueue.pop(top)) {
      Serial::writeByte(top, true);
    } else {
		  Serial::setTXReadyInterrupt(false);
	  }
  }
  
  static byte availableRX() {
    return rxQueue.count();
  }
  
  static bool readByte(byte &b) {
    ENTER_CRITICAL;
    bool success = rxQueue.pop(b);
    EXIT_CRITICAL;
    return success;
  }
  
  static bool writeByte(byte b) {
    ENTER_CRITICAL;
    bool success = txQueue.push(b);    
    EXIT_CRITICAL;
    
    if (!success) 
      txOverrun = true;
    else 
      Serial::setTXReadyInterrupt(true);
    
    return success;
  }
  
  static Queue<byte, rxBufferSize> rxQueue;
  static Queue<byte, txBufferSize> txQueue;

  static bool rxOverrun;
  static bool txOverrun;
};

template<class Serial, int rxBufferSize, int txBufferSize>
Queue<byte, rxBufferSize> BufferedSerial<Serial, rxBufferSize, txBufferSize>::rxQueue;

template<class Serial, int rxBufferSize, int txBufferSize>
Queue<byte, txBufferSize> BufferedSerial<Serial, rxBufferSize, txBufferSize>::txQueue;

template<class Serial, int rxBufferSize, int txBufferSize>
bool BufferedSerial<Serial, rxBufferSize, txBufferSize>::rxOverrun;

template<class Serial, int rxBufferSize, int txBufferSize>
bool BufferedSerial<Serial, rxBufferSize, txBufferSize>::txOverrun;




template<long baudrate, class RXPin, class Timer, class Callback>
class SoftSerial {
public:
  static void setup() {
    RXPin::Port::enablePCInterrupt();    
    Timer::setFrequency(baudrate);
  }
  
  static void enable() {
    RXPin::setPCMask();
  }
  
  static void disable() {    
    RXPin::clearPCMask();
    Timer::stop();
  }
  
  static void onPinChange() {
    if (RXPin::read()) {
      // Logical high
    }
    else {
      // Logical low
      if (bitIndex == 0) {
        // Falling edge (start bit) received
        rxData = 0;
        rxError = 0;
        bitIndex++;
        Timer::start();
      }
    }
  }
  
  static void onTimer() {
    byte state = RXPin::read();
    if (bitIndex == 0) {
      // Start bit (low) expected
      if (state) {
        rxError = 1;
        bitIndex = 0;
        Timer::stop();
      }
    }
    else if (bitIndex == 10) {
      // Expecting the stop bit (high)
      if (!state) {
        rxError = 1;
      }
      bitIndex = 0;
      Timer::stop();
      Callback::onReceive(rxData, rxError);
    }
    else {
      // Start bit or data bits
      // Shift bit in 
      rxData <<= 1;
      if (state) {
        // Logical high
        rxData |= 1;
      }
      bitIndex++;
    }
  }
  
  static byte rxData;
  static byte rxError;
  static byte bitIndex;
};

template<long baudrate, class RXPin, class Timer, class Callback>
byte SoftSerial<baudrate, RXPin, Timer, Callback>::rxData;

template<long baudrate, class RXPin, class Timer, class Callback>
byte SoftSerial<baudrate, RXPin, Timer, Callback>::rxError;

template<long baudrate, class RXPin, class Timer, class Callback>
byte SoftSerial<baudrate, RXPin, Timer, Callback>::bitIndex;
