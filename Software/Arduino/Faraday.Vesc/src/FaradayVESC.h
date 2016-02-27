#ifndef FaradayVESC_h
#define FaradayVESC_h

#include "HardwareSerial.h"
#include "Arduino.h"
#include "packet.h"
#include "buffer.h"
#include "crc.h"
class FaradayVESC{
public:
	FaradayVESC();
	void init(void(*s_func)(unsigned char *data, unsigned int len), void(*p_func)(unsigned char *data, unsigned int len));
	void update();
	void set_current(float current);
	void set_current_brake(float current);
	void get_values();
};

#endif
