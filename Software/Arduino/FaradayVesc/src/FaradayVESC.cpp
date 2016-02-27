#include "Arduino.h"
#include "packet.h"
#include "packet.c"
#include "buffer.h"
#include "buffer.c"
#include "crc.h"
#include "crc.c"
#include "FaradayVESC.h"

FaradayVESC::FaradayVESC()
{
}

void FaradayVESC::init(void(*s_func)(unsigned char *data, unsigned int len), void(*p_func)(unsigned char *data, unsigned int len))
{
	packet_init(s_func, p_func, 0);
}

void FaradayVESC::update()
{
	packet_timerfunc();
}

void FaradayVESC::set_current(float current) {
	uint8_t buffer[7];
	int32_t send_index = 0;
	buffer[send_index++] = COMM_FORWARD_CAN;
	buffer[send_index++] = 1;
	buffer[send_index++] = COMM_SET_CURRENT;
	buffer_append_double32(buffer, current, 1000.0, &send_index);
	packet_send_packet(buffer, send_index, 0);
	//Hack, send only the last part of the package to the second controller
	packet_send_packet(buffer + 2, send_index - 2, 0);
}


void FaradayVESC::set_current_brake(float current) {
	uint8_t buffer[5];
	int32_t send_index = 0;
	buffer[send_index++] = COMM_FORWARD_CAN;
	buffer[send_index++] = 1;
	buffer[send_index++] = COMM_SET_CURRENT_BRAKE;
	buffer_append_double32(buffer, current, 1000.0, &send_index);
	packet_send_packet(buffer, send_index, 0);
	//Hack, send only the last part of the package to the second controller
	packet_send_packet(buffer + 2, send_index - 2, 0);
}

void FaradayVESC::get_values() {
	uint8_t buffer[1];
	int32_t send_index = 0;
	buffer[send_index++] = COMM_GET_VALUES;
	packet_send_packet(buffer, send_index, 0);
}

/*
void FaradayVESC::get_values2() {
	uint8_t buffer[1];
	int32_t send_index = 0;
	buffer[send_index++] = COMM_FORWARD_CAN;
	buffer[send_index++] = 1;
	buffer[send_index++] = COMM_GET_VALUES;
	packet_send_packet(buffer, send_index, 0);
}
*/






