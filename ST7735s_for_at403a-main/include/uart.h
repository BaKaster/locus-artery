
#ifndef __UART
#define __UART

#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
//#include "uart_control.hpp"



void usart_configuration(void);
void usart5_tx_rx_handler(void);
uint8_t buffer_compare(uint8_t* pbuffer1, uint8_t* pbuffer2, uint16_t buffer_length);


void cycle();
void compare();

#endif
