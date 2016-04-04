#include <serial.hh>
#include <stream.hh>

const long baudrate  = 9600;
const word timeoutRX = 500;     // in multiples of 10 us
const word timeoutTX = 500;     // in multiples of 10 us

typedef WaitableSerial<BufferedSerial<SimpleSerial<baudrate> >, timeoutRX, timeoutTX> Serial;

TextStream<Serial> serial;

int main()
{
  serial.setup();
  serial.enable();

  byte b = 72;
  
  serial.print("Decimal: ").print(b).eol();
  serial.print("Binary : ").print(b, serial.eBinary).eol();
  serial.print("Hex    : ").print(b, serial.eHex).eol();
    
  while (true) {
    byte b;
    if (serial.readByte(b)) {
      serial.writeByte(b);
    }
  }
}

ISR(USART_RX_vect) {
  serial.onRXComplete();
}

ISR(USART_UDRE_vect) {
  serial.onTXReady();
}
