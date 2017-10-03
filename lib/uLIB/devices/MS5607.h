#pragma once
#include "Arduino.h"

class I2C {
public:
  I2C();

  byte write(byte address, const char *buffer, byte bufSize);
  byte read(byte address, char *buffer, byte bufSize);
  void setSlow();
  void setNormal();
  
private:
  int rxTimeout;
};



class MS5607Base {
public:
  enum OversamplingRate {
    kOSR256   = 0,
    kOSR512   = 1,
    kOSR1024  = 2,
    kOSR2048  = 3,
    kOSR4096  = 4,
    kOSR8192  = 5
  };
  
  enum ConversionType {
    kPressure       = 0,
    kTemperature    = 1
  };
};

class MS5607 : public MS5607Base {
public:
  MS5607(I2C &bus, byte subAddress = 0);
  
  bool reset();
  bool readPROM(byte address, word &result);
  bool startConversion(ConversionType type, OversamplingRate osr = kOSR256);
  bool readResult24(long &result);
  bool readResult16(word &result);
    
private:
  I2C     _bus;
  uint8_t _address;
  
  enum Commands {
    kCmdReset      = 0b00011110,
    kCmdReadPROM   = 0b10100000,
    kCmdConvert    = 0b01000000,
    kCmdReadADC    = 0b00000000,
  };
};

class Barometer : public MS5607 {
public:

  Barometer(I2C &bus, byte subaddress = 0);

  bool initialize();
  
  /**
   * Measures and updates both temperature and pressure
   */
  bool update();
  
  /// Returns temperature in Celsium x100 (2000 = 20.00 C)
  int16_t getTemperature();
  
  /// Returns pressure in Pascals (100000 = 100000 Pa = 1000 mbar)
  uint32_t getPressure();
    
private:
  uint16_t PROM[8];
  int16_t  t;        // temperature in Celsium x100 (2000 = 20.00 C)
  uint32_t p;        // pressure in Pascals (100000 = 100000 Pa = 1000 mbar)
};

class BarometerSimple : public MS5607 {
public:

  BarometerSimple(I2C &bus, byte subaddress = 0);

  bool initialize();
  
  /**
   * Measures and updates both temperature and pressure
   */
  bool update();
  
  /// Returns temperature in Celsium x100 (2000 = 20.00 C)
  int16_t getTemperature();
  
  /// Returns pressure in Pascals (100000 = 100000 Pa = 1000 mbar)
  uint16_t getPressure();

  /// Returns altitude in meters ASL
  uint16_t getAltitude();
  uint16_t getAltitude(uint16_t pressure);
    
private:
  uint16_t PROM[8];
  int16_t  t;        // temperature in Celsium x100 (2000 = 20.00 C)
  uint16_t p;        // pressure in Pascals /4 (25000 = 100000 Pa = 1000 mbar)
};
