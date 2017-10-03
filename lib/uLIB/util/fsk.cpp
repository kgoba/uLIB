#include "../../uLIB/util/fsk.h"

FSKTransmitter::FSKTransmitter() {
  autoShutdown = false;
  txLength = 0;
  bitIndex = 0;
  active = false;
}

void FSKTransmitter::mark() {
  pinTXMod.mode(IOMode::OutputHigh);
}

void FSKTransmitter::space() {
  pinTXMod.mode(IOMode::Input);
}

void FSKTransmitter::enable() {
  pinTXMod.mode(IOMode::OutputHigh);
  pinTXEnable.mode(IOMode::OutputHigh);
}

void FSKTransmitter::disable() {
  pinTXMod.mode(IOMode::OutputLow);
  pinTXEnable.mode(IOMode::OutputLow);    
}

void FSKTransmitter::test(byte idx) {
  switch (idx) {
    case 0: pinTXMod.mode(IOMode::OutputHigh); pinTXEnable.mode(IOMode::OutputHigh); break;
    case 1: pinTXMod.mode(IOMode::OutputHigh); pinTXEnable.mode(IOMode::OutputHigh); break;
  }
}

void FSKTransmitter::transmit(const uint8_t *buffer, uint16_t length) {
  if (active) return;
  txBuffer = buffer;
  txLength = length;
  if (txLength > 0) {
    active = true;
    enable();
    shiftNew();
  }
}

bool FSKTransmitter::isBusy() {
  return active;
}

void FSKTransmitter::tick() {
  if (!active) return;
  
  if (bitIndex == 10) {
    if (txLength > 0) 
      shiftNew();
    else {
      if (autoShutdown) {
        disable();
      }
      else {
        mark();
      }
      active = false;
      return;
    }
  }
  if (bitIndex == 0) {
    space();
  }
  else if (bitIndex >= 8) {
    mark();
  }
  else {
    if (txShift & 1) mark();
    else space();

    txShift >>= 1;
  }
  bitIndex++;
}

void FSKTransmitter::shiftNew() {
  txShift = *txBuffer++;
  txLength--;
  bitIndex = 0;
}
