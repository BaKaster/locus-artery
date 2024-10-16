#include "uart_control.h"

// Функция для вычисления CRC16
uint16_t crc16(const uint8_t *data, size_t length) {
	uint16_t crc = 0xFFFF; // Инициализация CRC

	for (size_t i = 0; i < length; ++i) {
		crc ^= data[i]; // XOR с текущим байтом
		for (uint8_t j = 0; j < 8; ++j) { // 8 бит
			if (crc & 0x0001) {
				crc = (crc >> 1) ^ 0xA001; // Обратное полиномное деление
			} else {
				crc >>= 1;
			}
		}
	}
	return crc;
}

// Функция для обработки входных данных
uint8_t parseIncomingData(const uint8_t *data, size_t length) {
	// Проверяем, достаточно ли данных
	if (length < 4)
		return 0; // Минимальная длина: 1 байт заголовка + 1 байт ID + 2 байта CRC

	// Проверка заголовка
	if (data[0] != 0x7E)
		return 1;

	// Извлечение ID
	uint8_t msgId = data[1];

	// Проверка CRC
	uint16_t receivedCrc = (data[length - 2] | (data[length - 1] << 8)); // Получаем CRC из последних двух байт
	uint16_t calculatedCrc = crc16(data, length - 2); // Вычисляем CRC для всех данных, кроме CRC

	if (receivedCrc != calculatedCrc) {
		return 0; // CRC не совпадает
	}

	// Обработка различных типов сообщений
	switch (msgId) {
	case TELEMETRY_EVENT: {
		TelemetryEvent msg;
		memcpy(&msg, data + 4, sizeof(TelemetryEvent)); // Копируем данные структуры
		command_handler_id1(&msg);
		break;
	}
	case MSG_MOTOR_CONTROL_DATA: {
		MotorControlData msg;
		memcpy(&msg, data + 2, sizeof(MotorControlData)); // Копируем данные структуры
		command_handler_id2(&msg);
		break;
	}
	case MSG_PID_PARAMETERS: {
		PID msg;
		memcpy(&msg, data + 2, sizeof(PID)); // Копируем данные структуры
		change_pid_param(work_param_ptr, &msg);
		break;
	}
	case LED_MODE: {
		PID msg;
		memcpy(&msg, data + 2, sizeof(led_recieve)); // Копируем данные структуры
		command_handler_id4(&msg);
		break;
	}
	default: {

		return 0;
	}

		return 1; // Успешно обработано
	}
}

//reset microcontroller
void feed_watch_dog(void) {
	if (workParams.reset == 0) { // не сбрасываем программу
		wdt_counter_reload();
	} else if (workParams.reset == 1) { // сбрасываем программу
		return;
	}
}

void led_set(uint16_t *src_buffer, led_param *led, led_recieve *param_struct,
		enum Regims led_regime) {

	led->red1 = param_struct->first_led.red;
	led->green1 = param_struct->first_led.green;
	led->blue1 = param_struct->first_led.blue;
	led->red2 = param_struct->second_led.red;
	led->green2 = param_struct->second_led.green;
	led->blue2 = param_struct->second_led.blue;

	if (led_regime == BLINK_3) {
		led->red3 = param_struct->third_led.red;
		led->green3 = param_struct->third_led.green;
		led->blue3 = param_struct->third_led.blue;
		led->regim = led_regime;
		return;
	}
	else if(led_regime == BLINK_4){
		led->red3 = param_struct->third_led.red;
		led->green3 = param_struct->third_led.green;
		led->blue3 = param_struct->third_led.blue;
		led->red4 = param_struct->fourth_led.red;
		led->green4 = param_struct->fourth_led.green;
		led->blue4 = param_struct->fourth_led.blue;
		led->regim = led_regime;
	}else {
		led->regim = led_regime;
		if (led_regime == WAVE)
			return;
		if (TURN_ON_SNGL_LED) {
			led->bright = param_struct->led_brightness;
			led->led_number = param_struct->led_sequence_number;
			return;
		}
	}
}



void command_handler_id1(TelemetryEvent *param_struct) {
	//логика вывода или ввода 
}

void command_handler_id2(BeaconAmplitude *param_struct) {

}



