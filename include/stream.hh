#pragma once

#include "util.hh"

class ByteFormat {
public:
  IntFormat(byte b) {
    str[0] = b;
  }
  
  const char * getString() { return str; }
  
private:
  char str[3];
};

template<class Base>
class TextStream : public Base 
{
public:
  enum IntBase {
    eDecimal,
    eBinary,
    eHex
  };
  
  TextStream & print(const char * string, byte size = 0) {
    while(*string) {
      Base::writeByte(*string);
      string++;
      if (size == 0) continue;
      size--;
      if (size == 0) break;
    }
    return *this;
  }
  
  TextStream & print(byte b, IntBase base = eDecimal) {
    switch (base) {
      case eHex: {
        printDigit(b >> 4);
        printDigit(b & 15);
        break;
      }
      case eDecimal: {
        printDigit(b / 100);
        printDigit((b / 10) % 10);
        printDigit(b % 10);
        break;
      }
      case eBinary: {
        byte mask = 0x80;
        while (mask) {
          Base::writeByte((b & mask) ? '1' : '0');
          mask >>= 1;
        }
        break;
      }
    }
    return *this;
  }
  
  TextStream & print(word b, IntBase base = eDecimal) {
    switch (base) {
      case eHex:
      case eBinary: {
        print(b >> 8, base);
        print(b & 0xFF, base);
        break;
      }
      case eDecimal: {
        byte d1 = b / 10000;
        printDigit(d1);
        b -= d1 * 10000;
        byte d2 = b / 1000;
        printDigit(d2);
        b -= d2 * 1000;
        print(b, base);
        break;
      }
    }
    return *this;
  }
  
  TextStream & eol() {
    Base::writeByte('\r');
    Base::writeByte('\n');
    return *this;
  }
  
private:
  bool printDigit(byte digit) {
    return Base::writeByte((digit < 10) ? '0' + digit : 'A' + digit - 10);
  }
};