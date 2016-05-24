#pragma once

#include "util.hh"
#include "queue.hh"

#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/twi.h>

// See http://www.chrisherring.net/all/tutorial-interrupt-driven-twi-interface-for-avr-part1/

/*
enum TWIMode {
  TWIMaster,
  TWISlave
};
*/

class I2C {
public:
  enum TransferMode {
    kNoStop,        // Used to send repeated start in the subsequent transfer
    kSendStop
  };
};

template<class I2CPeriph, byte address>
class I2CDevice {
public:
  static void send(byte *const data, byte nBytes, I2C::TransferMode mode = I2C::kSendStop) {
    I2CPeriph::write(address, data, nBytes, mode);
  }
  static void receive(byte * data, byte nBytes, I2C::TransferMode mode = I2C::kSendStop) {
    I2CPeriph::read(address, data, nBytes, mode);
  }
};

class TWIMaster : public I2C {
public:  
  static void setup(uint32_t clockFrequency = 100000ul) {
  	_state = kReady;
  	_errorCode = 0xFF;
  	_repStart = 0;
  	// Set prescaler (no prescaling) (TODO: set correct prescaler)
    // Note that no prescaler is necessary for 100kHz clock
  	TWSR = 0;
  	// Set bit rate
  	TWBR = ((F_CPU / clockFrequency) - 16) / 2;
  	// Enable TWI and interrupt
  	TWCR = (1 << TWIE) | (1 << TWEN);
  }
  
  static byte write(byte address, byte *const data, byte nBytes, TransferMode mode = kSendStop) {
    if (!isTWIReady()) return 0;
    _sla = (address << 1) | TW_WRITE;
    _mode = mode;
    _data = data;
    _dataLength = nBytes;
    TWISendStart();
  }
  
  static byte read(byte address, byte *data, byte nBytes, TransferMode mode = kSendStop) {
    _sla = (address << 1) | TW_READ;
    _mode = mode;
    _data = data;
    _dataLength = nBytes;
    TWISendStart();
  }
  
  static bool isReady(void) {
  	return (_state == kReady) || (_state == kRepeatedStartSent);
  }
  
  static bool onTWIInterrupt() {
    interruptImpl();
  }
  
private:
  
  enum State {
  	kReady,
  	kInitializing,
  	kRepeatedStartSent,
  	kTransmit,
  	kReceive
  };
  
  static byte    _sla;
  static State   _state;
  static byte    _errorCode;
  static TransmitMode    _mode;
  
  static byte   * _data;
  static byte   _dataLength; // The total number of bytes to transmit
  
  
  /************************ Helper methods ************************/
  
  // Send the START signal, enable interrupts and TWI, clear TWINT flag to resume transfer.
  static void TWISendStart() { 
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)|(1<<TWIE);
  }
  
  // Send the STOP signal, enable interrupts and TWI, clear TWINT flag.
  static void TWISendStop() { 
    TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN)|(1<<TWIE);
  } 

  // Used to resume a transfer, clear TWINT and ensure that TWI and interrupts are enabled.
  static void TWISendTransmit()	{
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE);
  } 
  
  // FOR MR mode. Resume a transfer, ensure that TWI and interrupts are enabled 
  // and respond with an ACK if the device is addressed as a slave or after it receives a byte.
  static void TWISendACK() {
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);
  } 

  // FOR MR mode. Resume a transfer, ensure that TWI and interrupts are enabled 
  // but DO NOT respond with an ACK if the device is addressed as a slave or after 
  // it receives a byte.
  static void TWISendNACK() { 
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE);
  }
  
  /********************* TWI state machine *********************/
  
  static void interruptImpl() {
  	switch (TW_STATUS)
  	{
      // ----\/ ---- START CONDITION ----\/ ----  //
      case TW_START: // Start condition has been transmitted
        TWDR = _sla;    // Transmit either SLA+W or SLA+R
        TWISendTransmit();
        break;
  		// ----\/ ---- MASTER TRANSMITTER ----\/ ----  //
  		case TW_MT_SLA_ACK: // SLA+W transmitted and ACK received
  		case TW_MT_DATA_ACK: // Data byte has been transmitted, ACK received
  			if (_dataLength > 0) // If there is more data to send
  			{
  				TWDR = *_data++; // Load data to transmit buffer
          _dataLength--;
  				_errorCode = TW_NO_INFO;
  				TWISendTransmit(); // Send the data
  			}
  			// All transmissions are complete, exit
  			else
  			{
  				_state = kReady;
  				_errorCode = 0xFF;
          if (_mode == kSendStop) {
    				TWISendStop();            
          }
  			}
  			break;
		
  		// ----\/ ---- MASTER RECEIVER ----\/ ----  //
		
  		case TW_MR_SLA_ACK: // SLA+R has been transmitted, ACK has been received
  			// Switch to Master Receiver mode
  			_state = kReceive;
				_errorCode = TW_NO_INFO;
  			// If there is more than one byte to be read, receive data byte and return an ACK
  			if (_dataLength > 1)
  			{
  				TWISendACK();
  			}
  			// Otherwise when a data byte (the only data byte) is received, return NACK
  			else
  			{
  				TWISendNACK();
  			}
  			break;
		
  		case TW_MR_DATA_ACK: // Data has been received, ACK has been transmitted.
  			/// -- HANDLE DATA BYTE --- ///
  			*_data++ = TWDR;
        _dataLength--;
				_errorCode = TW_NO_INFO;
  			// If there is more than one byte to be read, receive data byte and return an ACK
  			if (_dataLength > 1)
  			{
  				TWISendACK();
  			}
  			// Otherwise when a data byte (the only data byte) is received, return NACK
  			else
  			{
  				TWISendNACK();
  			}
  			break;
		
  		case TW_MR_DATA_NACK: // Data byte has been received, NACK has been transmitted. End of transmission.
		
  			/// -- HANDLE DATA BYTE --- ///
  			*_data+++ = TWDR;	
        _dataLength--;
  			// All transmissions are complete, exit
				_state = kReady;
				_errorCode = 0xFF;
        if (_mode == kSendStop) {
  				TWISendStop();            
        }
  			break;
		
  		// ----\/ ---- MT and MR common ----\/ ---- //
		
  		case TW_MR_SLA_NACK: // SLA+R transmitted, NACK received
  		case TW_MT_SLA_NACK: // SLA+W transmitted, NACK received
  		case TW_MT_DATA_NACK: // Data byte has been transmitted, NACK received
  		case TW_MT_ARB_LOST: // Arbitration has been lost
      _state = kReady;
  			_errorCode = TW_STATUS;
  			TWISendStop();
  			break;
  		case TW_REP_START: // Repeated start has been transmitted
  			// Set the mode but DO NOT clear TWINT as the next data is not yet ready
  			_state = kRepeatedStartSent;
  			break;
		
  		// ----\/ ---- SLAVE RECEIVER ----\/ ----  //
		
  		// TODO  IMPLEMENT SLAVE RECEIVER FUNCTIONALITY
		
  		// ----\/ ---- SLAVE TRANSMITTER ----\/ ----  //
		
  		// TODO  IMPLEMENT SLAVE TRANSMITTER FUNCTIONALITY
		
  		// ----\/ ---- MISCELLANEOUS STATES ----\/ ----  //
  		case TW_NO_INFO: 
        // It is not really possible to get into this ISR on this condition
  			// Rather, it is there to be manually set between operations
  			break;
  		case TW_BUS_ERROR: // Illegal START/STOP, abort and return error
  			_errorCode = TW_STATUS;
  			_state = kReady;
  			TWISendStop();
  			break;
  	}        
  }
};
