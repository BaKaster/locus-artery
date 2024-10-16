 //my_class.h
#ifndef MY_CLASS_HPP
#define MY_CLASS_HPP
#include"stdio.h"
#include"string.h"


extern void* src_buffer_ptr;

extern uint8_t heartbeat_check;


#define TIMER3_PERIOD 25499


void feed_watch_dog(void);

typedef struct __attribute__((packed)){
  uint8_t refs; //можно считать, что тут всегда 0xFE (маркер начала пакета)
  uint8_t size; // размер полезных данных структуры
  uint8_t addr;
  uint8_t num; //message id
} Event;

//event - 4 байта

typedef struct __attribute__((packed)) { // 27+4 bytes
  Event e;
  uint32_t orientation; // packed Euler angles: bits 0..10 - roll; 11..20 - pitch; 21..31 - yaw
  int32_t pos[3];
  int16_t vel[3];
  uint16_t voltage;
  uint8_t beacons;
  uint8_t status;
  uint8_t posError;

} TelemetryEvent;
//


typedef struct __attribute__((packed)){
  Event e;
  uint16_t amp[4];
} BeaconAmplitude;

// Структура типов пакетов
enum
{
  TELEMETRY_EVENT = 0x02,
  BEACON_AMPLITUDE_EVENT = 0x33,
};


uint8_t parseIncomingData(const uint8_t *data, size_t length); // Объявление функции C++, доступной для C

// Объявления функций
uint16_t crc16(const uint8_t *data, size_t length);



#endif // MY_CLASS_HPP
