extern "C" {
#include <inttypes.h>
}

#include "Arduino.h"
#include "Wire.h"
#include "SM_8REL8.h"





SM_8REL8::SM_8REL8(uint8_t stack)
{
		if (stack < 0)
		stack = 0;
	if (stack > 7)
		stack = 7;
	_hwAdd = SLAVE_OWN_ADDRESS_BASE + stack;//(stack ^ 0x07);
	_detected = false;
}

bool SM_8REL8::begin()
{
	Serial.println(_hwAdd, HEX);
	uint16_t value = 0;
	Wire.begin();
	if (0 == readWord(I2C_MEM_REVISION_HW_MAJOR_ADD, &value))
	{
		_detected = true;
	}
		Serial.println(_detected);

	return _detected;
}

bool SM_8REL8::isAlive()
{
	return _detected;
}


bool SM_8REL8::writeRelay(uint8_t relay, bool val)
{
	if (relay > RELAY_CH_NO || relay == 0)
	{
		return false;
	}
	if (val)
	{
		if (0 == writeByte(I2C_MEM_RELAY_SET, relay))
			return true;
	}
	else
	{
		if (0 == writeByte(I2C_MEM_RELAY_CLR, relay))
			return true;
	}
	return false;
}

bool SM_8REL8::writeRelay(uint8_t val)
{
	if (OK == writeByte(I2C_MEM_RELAY_VAL, 0x0f & val))
		return true;
	return false;
}


bool SM_8REL8::readButton()
{
	int ret = 0;
	uint8_t val = 0;

	ret = readByte(I2C_MEM_BUTTON, &val);
	if (ret < 0)
		return false;
	if(val & 1)
	{
		return true;
	}
	return false;
}
/*
 ***************** 8mosfet_I2C access functions ****************************
 **********************************************************************
 */
 int SM_8REL8::writeByte(uint8_t add, uint8_t value)
{
	//Wire.begin();
	Wire.beginTransmission(_hwAdd);
	Wire.write(add);
	Wire.write(value);
	return Wire.endTransmission();
}

int SM_8REL8::writeWord(uint8_t add, uint16_t value)
{
	uint8_t buff[2];

	memcpy(buff, &value, 2);
	//Wire.begin();
	Wire.beginTransmission(_hwAdd);
	Wire.write(add);
	Wire.write(buff[0]);
	Wire.write(buff[1]);
	return Wire.endTransmission();

}

int SM_8REL8::writeDWord(uint8_t add, uint32_t value)
{
	uint8_t buff[4];
	int i = 0;

	memcpy(buff, &value, 4);
	//Wire.begin();
	Wire.beginTransmission(_hwAdd);
	Wire.write(add);
	for (i = 0; i < 4; i++)
	{
		Wire.write(buff[i]);
	}
	return Wire.endTransmission();

}

int SM_8REL8::readByte(uint8_t add, uint8_t *value)
{
	if (0 == value)
	{
		return -1;
	}
	//Wire.begin();
	Wire.beginTransmission(_hwAdd);
	Wire.write(add);
	if (Wire.endTransmission() != 0)
	{
		return -1;
	}
	Wire.requestFrom(_hwAdd, (uint8_t)1);
	if (1 <= Wire.available())
	{
		*value = Wire.read();
	}
	else
	{
		return -1;
	}
	return 0;
}

int SM_8REL8::readWord(uint8_t add, uint16_t *value)
{
	uint8_t buff[2];

	if (0 == value)
	{
		return -1;
	}
	//Wire.begin();
	Wire.beginTransmission(_hwAdd);
	Wire.write(add);
	if (Wire.endTransmission() != 0)
	{
		return -1;
	}
	Wire.requestFrom(_hwAdd, (uint8_t)2);
	if (2 <= Wire.available())
	{
		buff[0] = Wire.read();
		buff[1] = Wire.read();
	}
	else
	{
		return -1;
	}
	memcpy(value, buff, 2);
	return 0;
}

int SM_8REL8::readDWord(uint8_t add, uint32_t *value)
{
	uint8_t buff[4];
	int i = 0;

	if (0 == value)
	{
		return -1;
	}
	//Wire.begin();
	Wire.beginTransmission(_hwAdd);
	Wire.write(add);
	if (Wire.endTransmission() != 0)
	{
		return -1;
	}
	Wire.requestFrom(_hwAdd, (uint8_t)4);
	if (4 <= Wire.available())
	{
		for (i = 0; i < 4; i++)
		{
			buff[i] = Wire.read();
		}
	}
	else
	{
		return -1;
	}
	memcpy(value, buff, 4);
	return 0;
}

 
 
