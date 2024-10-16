/**
 * @brief  configure button exint
 * @param  none
 * @retval none
 */
#include "uart.h"

uint8_t msg_id;
uint8_t status_rx = 0; //1 - active, 0 - passive

#define COUNTOF(a)                       (sizeof(a) / sizeof(*(a)))

uint8_t usart5_tx_buffer[31];
uint8_t usart5_rx_buffer[31];
uint8_t usart5_tx_counter = 0x00;
uint8_t usart5_rx_counter = 0x00;
uint8_t max_counter = 31;

/**
 * @brief  config usart
 * @param  none
 * @retval none
 */
void usart_configuration(void) {
	gpio_init_type gpio_init_struct;

	/* enable the usart2 and gpio clock */
	crm_periph_clock_enable(CRM_USART5_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOD_PERIPH_CLOCK, TRUE);

	gpio_default_para_init(&gpio_init_struct);

  /* configure the TX pin */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = GPIO_PINS_12;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOC, &gpio_init_struct);

  /* configure the RX pin */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_2;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOD, &gpio_init_struct);

	/* config usart nvic interrupt */
	nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
	nvic_irq_enable(USART5_IRQn, 0, 0);

	/* configure usart5 param */
	usart_init(USART5, 57600, USART_DATA_8BITS, USART_STOP_1_BIT);
	usart_transmitter_enable(USART5, TRUE);
	usart_receiver_enable(USART5, TRUE);

	/* enable usart5 interrupt */
	usart_interrupt_enable(USART5, USART_RDBF_INT, TRUE);
	usart_enable(USART5, TRUE);

//  usart_interrupt_enable(USART2, USART_TDBE_INT, TRUE);
//  usart_interrupt_enable(USART3, USART_TDBE_INT, TRUE);
}


/**
 * @brief  usart3 tx rx interrupt function
 * @param  none
 * @retval none
 */
void usart5_tx_rx_handler(void) {
	if (usart_flag_get(USART5, USART_RDBF_FLAG) != RESET) {

		/* read one byte from the receive data register */
		uint8_t rx_byte = usart_data_receive(USART5);

		if (rx_byte == 0x7e
				&& usart5_rx_counter == 0) {
			status_rx = 1;

		}
		if (status_rx) {
			usart5_rx_buffer[usart5_rx_counter] = rx_byte;

			if(usart5_rx_counter == 1){
				switch(usart5_rx_buffer[usart5_rx_counter]){
					case 1:
						max_counter = 7;
						break;
					case 2:
						max_counter = 7;
						break;
					case 3:
						max_counter = 10;
						break;
					case 4:
						max_counter = 19;
						break;
				}
			}


			if (usart5_rx_counter == max_counter - 1 ) {
				usart5_rx_counter = 0;
				status_rx = 0;            // Деактивация режима приёма
				parseIncomingData(usart5_rx_buffer, max_counter*sizeof(uint8_t));
				return;
				/* disable the usart5 receive interrupt */
//				usart_interrupt_enable(USART5, USART_RDBF_INT, FALSE);

			}
			++usart5_rx_counter;
		}

	}

	if (usart_flag_get(USART5, USART_TDBE_FLAG) != RESET) {
		/* write one byte to the transmit data register */
		usart_data_transmit(USART5, usart5_tx_buffer[usart5_tx_counter++]);

		if (usart5_tx_counter == max_counter) {
			/* disable the usart3 transmit interrupt */
			usart_interrupt_enable(USART5, USART_TDBE_INT, FALSE);
		}
	}
}

// Функция для перезапуска приёма
void restart_usart5_reception(void) {
	usart5_rx_counter = 0;  // Сброс счётчика приёма
	// Включение прерывания на приём
	usart_interrupt_enable(USART5, USART_RDBF_INT, TRUE);
}

/**
 * @brief  compares two buffers.
 * @param  pbuffer1, pbuffer2: buffers to be compared.
 * @param  buffer_length: buffer's length
 * @retval 1: pbuffer1 identical to pbuffer2
 *         0: pbuffer1 differs from pbuffer2
 */
uint8_t buffer_compare(uint8_t *pbuffer1, uint8_t *pbuffer2,
		uint16_t buffer_length) {
	while (buffer_length--) {
		if (*pbuffer1 != *pbuffer2) {
			return 0;
		}
		pbuffer1++;
		pbuffer2++;
	}
	return 1;
}


