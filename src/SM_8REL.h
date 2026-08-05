/*!
 * @file SM_SM_8REL.h
 * 
 * Designed specifically to work with the Sequent Microsysatems 
 * HALL Current Sensor DAQ 8-Layer Stackable HAT for Raspberry Pi
 * 
 * ----> https://sequentmicrosystems.com/products/hall-current-sensor-daq-stackable-hat-for-raspberry-pi
 * 
 *   This card use CRT8 to communicate.
 *   
 *   Written by Alexandru Burcea for Sequent Microsystems
 * 
 *   Software License Agreement (BSD License)
 *
 *  Copyright (c) 2025, Sequent Microsystems 
 *  
 */
#ifndef ___
#define ___

#define SLAVE_OWN_ADDRESS_BASE 0x0e

#define RELAY_CH_NO 8
#define IN_CH_NO 8
#define COUNT_SIZE 4
#define ENC_COUNT_SIZE 4
#define ENC_NO 2
#define SCAN_FREQ_SIZE 2
#define PWM_IN_FILL_SIZE 2
#define PWM_IN_SCALE 100
#define IN_FREQENCY_SIZE 2


#define OK 0

enum
{
	I2C_MEM_RELAY_VAL = 0,
	I2C_MEM_RELAY_SET,
	I2C_MEM_RELAY_CLR,
	I2C_MEM_DIG_IN,
	I2C_MEM_AC_IN,
	I2C_MEM_LED_VAL,
	I2C_MEM_LED_SET,
	I2C_MEM_LED_CLR,
	I2C_MEM_LED_MODE, //0-auto, 1 - manual;
	I2C_MEM_EDGE_ENABLE,
	I2C_MEM_ENC_ENABLE,
	I2C_MEM_SCAN_FREQ,
	I2C_MEM_PULSE_COUNT_START = I2C_MEM_SCAN_FREQ + SCAN_FREQ_SIZE,
	I2C_MEM_PPS = I2C_MEM_PULSE_COUNT_START + (IN_CH_NO * COUNT_SIZE),
	I2C_MEM_ENC_COUNT_START = I2C_MEM_PPS + IN_CH_NO * IN_FREQENCY_SIZE,
	I2C_MEM_PWM_IN_FILL = I2C_MEM_ENC_COUNT_START + (ENC_NO * ENC_COUNT_SIZE),
	I2C_MEM_IN_FREQENCY = I2C_MEM_PWM_IN_FILL + (IN_CH_NO * PWM_IN_FILL_SIZE),
	I2C_MEM_IN_FREQENCY_END = I2C_MEM_IN_FREQENCY + (IN_CH_NO * IN_FREQENCY_SIZE) - 1,
	I2C_MEM_PULSE_COUNT_RESET,//2 bytes to be one modbus register
	I2C_MEM_ENC_COUNT_RESET = I2C_MEM_PULSE_COUNT_RESET + 2,//2 bytes to be one modbus register
	I2C_MODBUS_SETINGS_ADD = I2C_MEM_ENC_COUNT_RESET + 2,
	I2C_NBS1,
	I2C_MBS2,
	I2C_MBS3,
	I2C_MODBUS_ID_OFFSET_ADD,
	I2C_MEM_EXTI_ENABLE,
	I2C_MEM_BUTTON, //bit0 - state, bit1 - latch


	I2C_MEM_REVISION_HW_MAJOR_ADD = 0x78,
	I2C_MEM_REVISION_HW_MINOR_ADD,
	I2C_MEM_REVISION_MAJOR_ADD,
	I2C_MEM_REVISION_MINOR_ADD,

};

const uint8_t mosfetsRemap[8] =
{
	0x01,
	0x02,
	0x04,
	0x08,
	0x10,
	0x20,
	0x40,
	0x80};


/*!
 * @brief SM_SM_8REL class
 */
class SM_8REL
{
public:
	/*!
	 * @brief Class constructor.
	 */
	SM_8REL(uint8_t stack = 0);

	/*!
	 * @brief Check card presence
	 * @return Returns true is successful
	 */
	bool begin();

	/*!
	 * @brief Return card existance status
	 * @return Returns true if card is present
	 */
	bool isAlive();

		/*!
	 * @brief Set one relay state
	 * @param relay -  Relay number 1..8 as are printed on the card
	 * @param val The new state of the relay, true: energised
	 * @return Returns true if successful
	 */
	bool writeRelay(uint8_t relay, bool val);

	/*!
	 * @brief Write all relays state as a 4 bits bitmap
	 * @param val The bitmap of the relays states
	 * @return Returns true if successful
	 */
	bool writeRelay(uint8_t val);

    /*!
	 * @brief Read optically isolated ports as a bitmap.
	 * @return the state of all opto inputs
	 */

	
	 /*!
	 * @brief Read button current state.
	 * @return true - pushed; false - released.
	 */
	bool readButton();
	
	 
private:
	uint8_t _hwAdd;
	bool _detected;
	int writeByte(uint8_t add, uint8_t value);
	int writeWord(uint8_t add, uint16_t value);
	int writeDWord(uint8_t add, uint32_t value);
	int readByte(uint8_t add, uint8_t* value);
	int readWord(uint8_t add, uint16_t* value);
	int readDWord(uint8_t add, uint32_t* value);
};
#endif // ___