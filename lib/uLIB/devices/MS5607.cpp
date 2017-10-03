#include "../../PTLib/x1/MS5607.h"

#include <Wire.h>

I2C::I2C() {
  rxTimeout = 200;
  Wire.begin();
}
  
byte I2C::write(byte address, const char *buffer, byte bufSize) {
  Wire.beginTransmission(address); 
  Wire.write(buffer, bufSize);       
  return Wire.endTransmission();
}

byte I2C::read(byte address, char *buffer, byte bufSize) {
  Wire.requestFrom(address, bufSize);

  long start = millis();
  while (bufSize > 0) {
    while (Wire.available() > 0) {
      *buffer++ = Wire.read();
      bufSize--;
      if (bufSize == 0) return 0;
    }
    word elapsed = millis() - start;
    if (elapsed > rxTimeout) return 1;
  }
  return 0;
}

void I2C::setSlow() {
  // F_CPU / (16 + 2*TWBR*prescaler)
  // Prescaler: 1/4/16/64
  //TWBR = 92;
  TWBR = 64;
  TWSR = 0;   // Prescaler: /1
}

void I2C::setNormal() {
  // F_CPU / (16 + 2*TWBR*prescaler)
  // Prescaler: 1/4/16/64
  TWBR = 32;
  TWSR = 0;   // Prescaler: /1
}


MS5607::MS5607(I2C &bus, byte subAddress) : _bus(bus) {
  _address = (0x76 | (subAddress & 1));
}
    
bool MS5607::reset() {
  char cmd = kCmdReset;
  return 0 == _bus.write(_address, &cmd, 1);
}

bool MS5607::readPROM(byte address, word &result) {
  byte cmd[1];
  cmd[0] = kCmdReadPROM | ((address & 0x07) << 1);
  if (0 != _bus.write(_address, (char *)cmd, 1)) {
    return false;
  }
  
  byte reply[2];
  if (0 != _bus.read(_address, (char *)reply, 2)) {
    return false;
  }

  result = ((word)reply[0] << 8) | reply[1];
  return true;
}

bool MS5607::startConversion(ConversionType type, OversamplingRate osr) {
  byte cmd[1];
  cmd[0] = kCmdConvert | ((byte)type << 4) | ((byte)osr << 1);
  return 0 == _bus.write(_address, (char *)cmd, 1);
}

bool MS5607::readResult24(long &result) {
  byte cmd[1];
  cmd[0] = kCmdReadADC;
  if (0 != _bus.write(_address, (char *)cmd, 1)) {
    return false;
  }
  
  byte reply[3];
  if (0 != _bus.read(_address, (char *)reply, 3)) {
    return false;
  }
 
  result = ((long)reply[0] << 16) | ((word)reply[1] << 8) | reply[2];
  return true;
}

bool MS5607::readResult16(word &result) {
  byte cmd[1];
  cmd[0] = kCmdReadADC;
  if (0 != _bus.write(_address, (char *)cmd, 1)) {
    return false;
  }
  
  byte reply[2];
  if (0 != _bus.read(_address, (char *)reply, 2)) {
    return false;
  }
  
  result = ((word)reply[0] << 8) | reply[1];
  return true;
}

Barometer::Barometer(I2C &bus, byte subaddress) 
  : MS5607(bus, subaddress) 
{
  
}

bool Barometer::initialize() 
{
  uint8_t nTry = 3;

  while (nTry > 0) {
    bool success = true;
    for (byte idx = 0; idx < 7; idx++) {
      if (!readPROM(idx, PROM[idx])) {
        success = false;
        break;
      }
    }
    if (success) return true;
    nTry--;
    delay(30);
  }
  return false;
}

/**
 * Measures and updates both temperature and pressure
 */
bool Barometer::update() {
  p = t = 0;
  if (!startConversion(kTemperature, kOSR1024)) {
    return false;
  }
 
  delay(2);
  long d2;
  if (!readResult24(d2)) {
    return false;
  }
  int32_t dt = d2 - ((int32_t)PROM[5] << 8);
  t = 2000 + ((dt * PROM[6]) >> 23);
  
  if (!startConversion(kPressure, kOSR4096)) {
    return false;
  }

  delay(10);
  long d1;
  if (!readResult24(d1)) {
    return false;
  }
  uint64_t off  = ((uint64_t)PROM[2] << 17) + ((PROM[4] * (int64_t)dt) >> 6);
  uint64_t sens = ((uint64_t)PROM[1] << 16) + ((PROM[3] * (int64_t)dt) >> 7);
  p = (((d1 * sens) >> 21) - off) >> 15;

  //uint16_t off  = PROM[2] + ((PROM[4] * (int32_t)dt) >> 15);
  //uint16_t sens = PROM[1] + ((PROM[3] * (int32_t)dt) >> 15);
  //p = (((uint32_t)d1 * sens) >> 14) - off;

  return true;
}

/// Returns temperature in Celsium x100 (2000 = 20.00 C)
int16_t Barometer::getTemperature() {
  return t;
}

/// Returns pressure in Pascals (100000 = 100000 Pa = 1000 mbar)
uint32_t Barometer::getPressure() {
  return p;
}


BarometerSimple::BarometerSimple(I2C &bus, byte subaddress) 
  : MS5607(bus, subaddress) 
{
  
}

bool BarometerSimple::initialize() 
{
  for (byte idx = 0; idx < 7; idx++) {
    if (!readPROM(idx, PROM[idx])) 
      return false;
  }
  return true;
}

/**
 * Measures and updates both temperature and pressure
 */
bool BarometerSimple::update() {
    p = t = 0;
    if (!startConversion(kTemperature, kOSR1024)) {
      return false;
    }
    _delay_ms(30);
    word d2;
    if (!readResult16(d2)) {
      return false;
    }
    int16_t dt = d2 - PROM[5];
    t = 2000 + (((int32_t)dt * PROM[6]) >> 15);
    
    if (!startConversion(kPressure, kOSR1024)) {
      return false;
    }
    _delay_ms(30);
    word d1;
    if (!readResult16(d1)) {
      return false;
    }
    uint16_t off  = PROM[2] + ((PROM[4] * (int32_t)dt) >> 15);
    uint16_t sens = PROM[1] + ((PROM[3] * (int32_t)dt) >> 15);
    p = (((uint32_t)d1 * sens) >> 14) - off;
    
    return true;
}

/// Returns temperature in Celsium x100 (2000 = 20.00 C)
int16_t BarometerSimple::getTemperature() {
  return t;
}

/// Returns pressure in Pascals (100000 = 100000 Pa = 1000 mbar)
uint16_t BarometerSimple::getPressure() {
  return p;
}

uint16_t BarometerSimple::getAltitude() {
  return getAltitude(p);
}

uint16_t BarometerSimple::getAltitude(uint16_t pressure) {
  // Atmospheric pressure in Pa/4 for altitudes 0..32 km in steps of 2 km
  // Source: http://www.pdas.com/atmos.html
  const uint16_t lookup[] = {
    25325, 19875, 15415, 11805,
    8913, 6625, 4850, 3543, 2588, 1891,
    1382, 1012, 743, 547, 404, 299, 222
  };

  const uint16_t stepSize = 2000;     // 5C

  // Find the pair of closest lookup table entries
  uint8_t idx = 0;
  uint16_t alt = 0;
  while (lookup[idx] > pressure) {
    idx++;
    alt += stepSize;
    if (alt > 32000) {
      return 32000;
    }
  }
  if (idx == 0) return 0;

  // Linear interpolation between adjacent table entries
  uint16_t diff = lookup[idx - 1] - lookup[idx];
  alt -= ((uint32_t)stepSize * (pressure - lookup[idx]) + diff/2) / diff;
  
  return alt;  
}

