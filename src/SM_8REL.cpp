extern "C"
{
#include <inttypes.h>
}

#include "Arduino.h"
#include "Wire.h"
#include "SM_8REL.h"

#define RELAY8_INPORT_REG_ADD 0x00
#define RELAY8_OUTPORT_REG_ADD 0x01
#define RELAY8_POLINV_REG_ADD 0x02
#define RELAY8_CFG_REG_ADD 0x03

SM_8REL::SM_8REL(uint8_t stack)
{
	if (stack < 0)
		stack = 0;
	if (stack > 7)
		stack = 7;
	_hwAdd = SLAVE_OWN_ADDRESS_BASE + stack; //(stack ^ 0x07);
	_detected = false;
}

bool SM_8REL::begin()
{
	Serial.println(_hwAdd, HEX);
	uint16_t value = 0;
	Wire.begin();
	if (0 == readWord(I2C_MEM_REVISION_HW_MAJOR_ADD, &value))
	{
		_detected = true;
	}

	return _detected;
}

bool SM_8REL::isAlive()
{
	return _detected;
}


uint8_t SM_8REL::relayToIO(uint8_t relay)
{
    uint8_t val = 0;
	for(int i=0; i< 8; i++)
	{        if((relay & (1 << i)) != 0)
            val = val + relayMaskRemap[i];
	}
    return val;
}

uint8_t SM_8REL::IOToRelay(uint8_t iov)
{
   uint8_t val = 0;
	for(int i=0; i< 8; i++)
	{
        if((iov & relayMaskRemap[i]) != 0)
            val = val + (1 << i);
	}
    return val;
}

uint8_t SM_8REL::check()
{
 	uint8_t value = 0;
	uint8_t ret = 0;

	uint8_t cfg =0;
	// bus.read_byte_data(add, RELAY8_CFG_REG_ADD)
		ret = readByte(RELAY8_CFG_REG_ADD, &cfg);

    if( cfg != 0)
	{
		writeByte(RELAY8_OUTPORT_REG_ADD,  0);
		writeByte(RELAY8_CFG_REG_ADD, 0);
	}
	ret = readByte(RELAY8_INPORT_REG_ADD, &value);
	return value;
}

bool SM_8REL::writeRelay(uint8_t relay, bool val)
{
	bool ret = false;
	uint8_t value = 0;
	uint8_t mask = 0;

	if (relay > RELAY_CH_NO || relay == 0)
	{
		return false;
	}

	     uint8_t oldVal = check();
    oldVal = IOToRelay(oldVal);

	if (val)
	{
            oldVal = oldVal | (1 << (relay - 1));
            oldVal = relayToIO(oldVal);
			if (0 == writeByte(RELAY8_OUTPORT_REG_ADD, oldVal))
		{
			ret = true;
			delay(100);
		}
	}
	else
	{
            oldVal = oldVal & (~(1 << (relay - 1)));
            oldVal = relayToIO(oldVal);
			if (0 == writeByte(RELAY8_OUTPORT_REG_ADD, oldVal))
		{
			ret = true;
			delay(100);
		}
	}

	return ret;
}

bool SM_8REL::writeRelay(uint8_t val)
{
	if (OK == writeByte(I2C_MEM_RELAY_SET, 0x0f & val))
{
			delay(100);
	return true;
}
	return false;
}

bool SM_8REL::readButton()
{
	int ret = 0;
	uint8_t val = 0;

	ret = readByte(I2C_MEM_BUTTON, &val);

	if (ret < 0)
		return false;
	if (val & 1)
	{
		return true;
	}
	return false;
}
/*
 ***************** 8mosfet_I2C access functions ****************************
 **********************************************************************
 */
int SM_8REL::writeByte(uint8_t add, uint8_t value)
{
	// Wire.begin();
	Wire.beginTransmission(_hwAdd);
	Wire.write(add);
	Wire.write(value);
	return Wire.endTransmission();
}

int SM_8REL::writeWord(uint8_t add, uint16_t value)
{
	uint8_t buff[2];

	memcpy(buff, &value, 2);
	// Wire.begin();
	Wire.beginTransmission(_hwAdd);
	Wire.write(add);
	Wire.write(buff[0]);
	Wire.write(buff[1]);
	return Wire.endTransmission();
}

int SM_8REL::writeDWord(uint8_t add, uint32_t value)
{
	uint8_t buff[4];
	int i = 0;

	memcpy(buff, &value, 4);
	// Wire.begin();
	Wire.beginTransmission(_hwAdd);
	Wire.write(add);
	for (i = 0; i < 4; i++)
	{
		Wire.write(buff[i]);
	}
	return Wire.endTransmission();
}

int SM_8REL::readByte(uint8_t add, uint8_t *value)
{
	if (0 == value)
	{
		return -1;
	}
	// Wire.begin();
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

int SM_8REL::readWord(uint8_t add, uint16_t *value)
{
	uint8_t buff[2];

	if (0 == value)
	{
		return -1;
	}
	// Wire.begin();
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

int SM_8REL::readDWord(uint8_t add, uint32_t *value)
{
	uint8_t buff[4];
	int i = 0;

	if (0 == value)
	{
		return -1;
	}
	// Wire.begin();
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
