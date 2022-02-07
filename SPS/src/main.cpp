#include <Button.h>
#include <SoftwareSerial.h>
#include <Gprs.h>

#define PHONE_NUMBER1 "+79999999999"
#define PHONE_NUMBER2 "+79999999999"

static uint8_t start_phrase[] = {0x3c, 0xd8, 0xe0, 0xdf, 0x14, 0x04, 0x30, 0x04, 0x47, 0x04, 0x30, 0x04, 0x20, 0x00, 0x37, 0x04, 0x30, 0x04, 0x3f, 0x04, 0x43, 0x04, 0x41, 0x04, 0x42, 0x04, 0x38, 0x04, 0x3b, 0x04, 0x30, 0x04, 0x41, 0x04, 0x4c, 0x04, 0x21, 0x00, 0x3c, 0xd8, 0xe0, 0xdf};
int button_pin = 5;
int led_pin = 6;
int relays_pin = 7;

Button button(button_pin);
bool relays_state = false;
int led = 6;
SoftwareSerial gprs_serial(10, 11);

GPRS gprs(gprs_serial);
void setup()
{
	Serial.begin(9600);
	gprs_serial.begin(9600);
	// включаем GPRS-шилд
	gprs.powerOn();
	// проверяем, есть ли связь с GPRS-устройством
	while (!gprs.init())
	{
		// если связи нет, ждём 1 секунду
		// и выводим сообщение об ошибке;
		// процесс повторяется в цикле,
		// пока не появится ответ от GPRS-устройства
		delay(1000);
		Serial.print("GPRS Init error\r\n");
	}
	// вывод об удачной инициализации GPRS Shield
	Serial.println("GPRS init success");
	// сообщаем о написании и отправке СМС по указанному номеру
	Serial.println("Start to send message ...");
	gprs.sendSMS(PHONE_NUMBER1, (char *)start_phrase);
	pinMode(led, OUTPUT);
	pinMode(relays_pin, OUTPUT);
}

void LEDcontol(int pin, bool state)
{
	if (state)
	{
		digitalWrite(pin, HIGH);
		digitalWrite(relays_pin, HIGH);
	}
	else
	{
		digitalWrite(pin, LOW);
		digitalWrite(relays_pin, LOW);
	}
}

void loop()
{
	if (button.click())
	{
		Serial.println("Button has been pressed");
		relays_state = !relays_state;
		if (relays_state)
		{
			digitalWrite(relays_pin, HIGH);
		}
		else
		{
			digitalWrite(relays_pin, LOW);
		}
	}
	LEDcontol(led, relays_state);
}
