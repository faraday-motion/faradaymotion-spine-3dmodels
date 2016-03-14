/*
	Copyright 2012-2014 Benjamin Vedder	benjamin@vedder.se
 
 	This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * packet.h
 *
 *  Created on: 21 mar 2013
 *      Author: benjamin
 */

#ifndef PACKET_H_
#define PACKET_H_

#include <stdint.h>

// Settings
#define PACKET_RX_TIMEOUT		2
#define PACKET_HANDLERS			1
#define PACKET_MAX_PL_LEN		1024

// Functions
void packet_init(void (*s_func)(unsigned char *data, unsigned int len),
void (*p_func)(unsigned char *data, unsigned int len), int handler_num);
void packet_process_byte(uint8_t rx_data, int handler_num);
void packet_timerfunc(void);
void packet_send_packet(unsigned char *data, unsigned int len, int handler_num);

typedef enum {
  COMM_FW_VERSION = 0,
  COMM_JUMP_TO_BOOTLOADER,
  COMM_ERASE_NEW_APP,
  COMM_WRITE_NEW_APP_DATA,
  COMM_GET_VALUES,
  COMM_SET_DUTY,
  COMM_SET_CURRENT,
  COMM_SET_CURRENT_BRAKE,
  COMM_SET_RPM,
  COMM_SET_POS,
  COMM_SET_DETECT,
  COMM_SET_SERVO_OFFSET,
  COMM_SET_MCCONF,
  COMM_GET_MCCONF,
  COMM_SET_APPCONF,
  COMM_GET_APPCONF,
  COMM_SAMPLE_PRINT,
  COMM_TERMINAL_CMD,
  COMM_PRINT,
  COMM_ROTOR_POSITION,
  COMM_EXPERIMENT_SAMPLE,
  COMM_DETECT_MOTOR_PARAM,
  COMM_REBOOT,
  COMM_ALIVE,
  COMM_GET_DECODED_PPM,
  COMM_GET_DECODED_ADC,
  COMM_GET_DECODED_CHUK,
  COMM_FORWARD_CAN
} 
COMM_PACKET_ID;

#endif /* PACKET_H_ */

