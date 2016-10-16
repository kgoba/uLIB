#pragma once
#include "util.hh"


template<uint16_t polynomial, bool reverse = false, uint16_t initialValue = 0xFFFF>
class CRC16 {
public:

  CRC16();

  void clear();
  uint16_t update(uint8_t value);
  uint16_t update(const char *string);
  uint16_t update(const uint8_t *data, uint16_t length);

  static void makeTable();

private:
  uint16_t getRemainder(uint8_t dividend);

  uint16_t remainder;

  static uint16_t  crcTable[256];
  static bool tableReady;
};



template<uint16_t polynomial, bool reverse, uint16_t initialValue>
uint16_t  CRC16<polynomial, reverse, initialValue>::crcTable[256];

template<uint16_t polynomial, bool reverse, uint16_t initialValue>
bool CRC16<polynomial, reverse, initialValue>::tableReady = false;

template<uint16_t polynomial, bool reverse, uint16_t initialValue>
CRC16<polynomial, reverse, initialValue>::CRC16()
{
  makeTable();
  clear();
}

template<uint16_t polynomial, bool reverse, uint16_t initialValue>
void CRC16<polynomial, reverse, initialValue>::clear()
{
  remainder = initialValue;
}

template<uint16_t polynomial, bool reverse, uint16_t initialValue>
uint16_t CRC16<polynomial, reverse, initialValue>::getRemainder(uint8_t dividend)
{
  uint16_t remainder;
       /*
       * Start with the dividend followed by zeros.
       */
      if (reverse) {
        remainder = dividend;
      }
      else {
        remainder = dividend << 8;
      }

      /*
       * Perform modulo-2 division, a bit at a time.
       */
      for (uint8_t bit = 8; bit > 0; --bit)
      {
          /*
           * Try to divide the current data bit.
           */
          if (reverse) {
            if (remainder & 0x0001)
            {
                remainder = (remainder >> 1) ^ polynomial;
            }
            else
            {
                remainder = (remainder >> 1);
            }
          }
          else {
            if (remainder & 0x8000)
            {
                remainder = (remainder << 1) ^ polynomial;
            }
            else
            {
                remainder = (remainder << 1);
            }
         }
      }
      return remainder;
}

template<uint16_t polynomial, bool reverse, uint16_t initialValue>
void CRC16<polynomial, reverse, initialValue>::makeTable(void)
{
  if (tableReady) return;

  uint16_t  remainder;

  /*
   * Compute the remainder of each possible dividend.
   */
  for (uint16_t dividend = 0; dividend < 256; ++dividend)
  {
      /*
       * Store the result into the table.
       */
      //crcTable[dividend] = getRemainder(dividend);
  }
  tableReady = true;
}

template<uint16_t polynomial, bool reverse, uint16_t initialValue>
uint16_t CRC16<polynomial, reverse, initialValue>::update(uint8_t value)
{
  /*
   * Divide the message by the polynomial, a byte at a time.
   */
  if (reverse) {
    uint8_t data = value ^ remainder;
    //remainder = crcTable[data] ^ (remainder >> 8);
    remainder = getRemainder(data) ^ (remainder >> 8);
  }
  else {
    uint8_t data = value ^ (remainder >> 8);
    //remainder = crcTable[data] ^ (remainder << 8);
    remainder = getRemainder(data) ^ (remainder << 8);
  }

  /*
   * The final remainder is the CRC.
   */
  return remainder;
}

template<uint16_t polynomial, bool reverse, uint16_t initialValue>
uint16_t CRC16<polynomial, reverse, initialValue>::update(const char *string)
{
  if (!string) return remainder;
  while (*string) {
    update(*string);
    string++;
  }
  return remainder;
}

template<uint16_t polynomial, bool reverse, uint16_t initialValue>
uint16_t CRC16<polynomial, reverse, initialValue>::update(const uint8_t *data, uint16_t length)
{
  //Serial.print("CRC: ");
  while (length > 0) {
    //Serial.print((char)*data);
    update(*data);
    data++;
    length--;
  }
  //Serial.print(" = ");
  //Serial.print(remainder, HEX);
  //Serial.println();
  return remainder;
}

typedef CRC16<0x1021, false, 0xFFFF>    CRC16_CCITT;
