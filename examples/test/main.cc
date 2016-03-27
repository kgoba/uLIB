#include <pins>

InputPin<20, kLow, kPullup> pin1;

int main()
{
  pin1.setup();

  byte c = 0;
  if (pin1.get()) {
    c = 7;
  }
  return c;
}

