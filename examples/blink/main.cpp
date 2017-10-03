#include <ulib>

OutputPin<LED> led;

int main()
{
	while (1) {
		led.toggle();
		delay_ms(1000);
	}
}
