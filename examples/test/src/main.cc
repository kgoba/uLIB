#include "../../../lib/PTLib/pins.hh"
#include "../../../lib/PTLib/serial.hh"
#include "../../../lib/PTLib/stream.hh"
#include "../../../lib/PTLib/timer.hh"

const long baudrate  = 9600;
const word timeoutRX = 500;     // in multiples of 10 us
const word timeoutTX = 500;     // in multiples of 10 us

IOPort<PortB>::pin0 led1;

PWMDual<Timer0> pwm0;

typedef WaitableSerial<BufferedSerial<SimpleSerial<baudrate> >, timeoutRX, timeoutTX> Serial;

TextStream<Serial> serial;

int main()
{
  serial.setup();
  serial.enable();

  pwm0.setup();

  led1.set();
  led1.clear();

  pwm0.setPWMA(23);
  pwm0.setPWMB(24);

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
