#include <Button.h>
#include <SoftwareSerial.h>
#include <Gprs.h>
#include <Timer.h>
#define MESSAGE_LENGTH 160
#define PHONE_NUMBER1 "+79999992935"
#define ON "ВКЛЮЧЕНО"
#define OFF "ВКЛЮЧЕНО"
#define PHONE_NUMBER2 "+79163912754" // "+79163912754" //"+79999992935"
// текст сообщения
char message[161];
// номер, с которого пришло сообщение
char phone[16];
// дата отправки сообщения
char datetime[24];
void LEDcontol(int pin, bool state);
int button_pin = 5;
int led_pin = 6;
int relays_pin = 7;

Button button(button_pin);
bool relays_state = false;
int led = 6;
SoftwareSerial gprs_serial(10, 11);
PDUMessageDecoder smsDecoder = PDUMessageDecoder();
PDUIncomingMessage incomingMessage;
TimerMs tmr(10000, 1, 1);

GPRS gprs(gprs_serial);
void setup()
{
	message[160] = '\0';
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
		LEDcontol(led, true);
		delay(1000);
		Serial.print("GPRS Init error\r\n");
		LEDcontol(led, false);
	}
	// вывод об удачной инициализации GPRS Shield
	Serial.println("GPRS init success");
	// сообщаем о написании и отправке СМС по указанному номеру
	Serial.println("Start to send message ...");
	gprs.sendSMS_PDU(PHONE_NUMBER1, "[СОБЫТИЕ]\nДАЧА запущена");
	pinMode(led, OUTPUT);
	pinMode(relays_pin, OUTPUT);
	tmr.setPeriodMode();
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

char *trimR(char *str) {
    while (*str != '\n') {
        str++;
    }
    return str + 1;
}

char *trimL(char *str) {
    char *begin = str;
    int size = 0;
    while (*begin) {
        ++begin;
        ++size;
    }
    str[size - 2] = 0;
    return str;
}
void loop()
{
	LEDcontol(led, relays_state);
	Serial.println("Tick");
			
			if (gprs.ifSMSNow()){
				gprs.readSMS(message, MESSAGE_LENGTH);
				if (strlen(message)){
					char *msg = new char[161];
					msg[160] = 0;
					strcpy (msg,message);
					String msg_s(trimR(trimL(msg)));
					incomingMessage = smsDecoder.Decode(msg_s, String(PHONE_NUMBER1));
					Serial.println(incomingMessage.Message.c_str());
					bool send_sms = false;
					if (strcmp(incomingMessage.Message.c_str(), "ВКЛ") == 0){
						relays_state = true;
					} else if (strcmp(incomingMessage.Message.c_str(), "ВЫКЛ") == 0) {
						relays_state = false;
					} else if (strcmp(incomingMessage.Message.c_str(), "СТАТУС") == 0){
						if (relays_state) {
							gprs.endData();
							while(send_sms != true){
								send_sms = gprs.sendSMS_PDU(PHONE_NUMBER1, "ВКЛ");
							}
						} else {
							gprs.endData();
							while(send_sms != true){
								send_sms = gprs.sendSMS_PDU(PHONE_NUMBER1, "ВЫКЛ");
							}
						}
					} else {
						gprs.endData();
						delay(1000);
						gprs_serial.write(27);
						gprs_serial.print("\r\n");
						gprs.sendSMS_PDU(PHONE_NUMBER1, "[СОБЫТИЕ]\nНЕРАСПОЗНАНО");
					}

					delete[] msg;
				}
				// Serial.println(incomingMessage.Message);
				// Serial.println(incomingMessage.Message);
			}

	
	// if (tmr.tick()){
	// 		Serial.println("Tick");
			
	// 		if (gprs.ifSMSNow()){
	// 			gprs.readSMS(message, MESSAGE_LENGTH);
	// 			if (strlen(message)){
	// 				char *msg = new char[161];
	// 				msg[160] = 0;
	// 				strcpy (msg,message);
	// 				String msg_s(trimR(trimL(msg)));
	// 				Serial.println(msg_s);
	// 				delete[] msg;
	// 			}
	// 			// Serial.println(incomingMessage.Message);
	// 			// Serial.println(incomingMessage.Message);
	// 		}
			
	// }
	
	
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
}
