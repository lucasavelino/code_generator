#ifndef J1939INCLUDES_H
#define J1939INCLUDES_H

#include <stdint.h>
#include <Wstring.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b)) 

bool setByteAt(uint8_t* data, uint16_t length, uint16_t index, uint8_t val)
{
	if ( index >= length )
	{
		return false;
	}
	data[index] = val;
	return true;
}

bool setWordAt(uint8_t* data, uint16_t length, uint16_t index, uint16_t val)
{
	if ( index >= length/2 )
	{
		return false;
	}
	index = index*2;
	data[index] = (uint8_t)val & 0xFF;
	data[index+1] = (uint8_t)val >> 8;
	return true;
}

bool setLongAt(uint8_t* data, uint16_t length, uint16_t index, uint32_t val)
{
	if ( index >= length/4 )
	{
		return false;
	}
	index = index*4;
	data[index] = (uint8_t)(val & 0xFF);
	data[index+1] = (uint8_t)((val >> 8) & 0xFF);
	data[index+2] = (uint8_t)((val >> 16) & 0xFF);
	data[index+3] = (uint8_t)((val >> 24) & 0xFF);
	return true;
}

uint8_t getByteAt(uint8_t* data, uint16_t length, uint16_t index)
{
	if ( index >= length )
	{
		return 0;
	}
	return (((data[index])));
}

uint16_t getWordAt(uint8_t* data, uint16_t length, uint16_t index)
{
	if ( index >= length-1 )
	{
		return 0;
	}
	uint16_t shRetVal = (data[index+1] << 8) | data[index];
	return  shRetVal;
}

uint32_t getLongAt(uint8_t* data, uint16_t length, uint16_t index)
{
	if ( index >= length-3 )
	{
		return 0;
	}
	uint32_t ulRetVal = ((uint32_t)data[index+3] << 24) | ((uint32_t)data[index+2] << 16) | ((uint32_t)data[index+1] << 8) | (uint32_t)data[index];
	return  ulRetVal;
}

template <typename ValueType, const unsigned int byte_index, 
			const unsigned int start_bit, const unsigned int length, const bool intel>
ValueType get_raw_value(const uint8_t * const data)
{
	uint64_t value;
	unsigned int bits_count = MIN(length, 8 - start_bit);
	unsigned int byte_count = byte_index;
	value = ((uint64_t) data[byte_count] >> start_bit) & ((1U << bits_count) - 1U);
	if(intel)
	{
		byte_count++;
	} else
	{
		byte_count--;
	}
	while (bits_count < length)
	{
		unsigned int bits_current_byte = MIN(length - bits_count, 8);
		value |= ((uint64_t) data[byte_count] & ((1U << bits_current_byte) - 1U)) << bits_count;
		bits_count += bits_current_byte;
		if(intel)
		{
			byte_count++;
		} else
		{
			byte_count--;
		}
	}
	return (ValueType)value;
}

template <typename ValueType, const unsigned int byte_index, 
			const unsigned int start_bit, const unsigned int length, const bool intel>
void set_raw_value(uint8_t * const data, ValueType value)
{
	unsigned int bits_count = MIN(length, 8 - start_bit);
	unsigned int byte_iter = byte_index;
	data[byte_iter] &= ~(((1U << bits_count) - 1U) << start_bit);
	data[byte_iter] |= (value & ((1U << bits_count) - 1U)) << start_bit;
	value >>= bits_count;
	if(intel)
	{
		byte_iter++;
	} else
	{
		byte_iter--;
	}
	while (bits_count < length)
	{
		unsigned int bits_current_byte = MIN(length - bits_count, 8);
		data[byte_iter] &= ~((1U << bits_current_byte) - 1U);
		data[byte_iter] |= value & ((1U << bits_current_byte) - 1U);
		bits_count += bits_current_byte;
		value >>= bits_current_byte;
		if(intel)
		{
			byte_iter++;
		} else
		{
			byte_iter--;
		}
	}
}

enum EDIRECTION
{
	DIR_RX = 'R',
	DIR_TX = 'T',
	DIR_ALL
};

enum EJ1939_MSG_TYPE
{
	MSG_TYPE_NONE = 0x0,
	MSG_TYPE_COMMAND,
	MSG_TYPE_REQUEST,
	MSG_TYPE_DATA,
	MSG_TYPE_BROADCAST,
	MSG_TYPE_ACKNOWLEDGEMENT,
	MSG_TYPE_GROUP_FUNCTIONS,

	MSG_TYPE_NM_ACL,
	MSG_TYPE_NM_RQST_ACL,
	MSG_TYPE_NM_CMD_ADDRESS,
	MSG_TYPE_NM_TPCM_BAM,
	MSG_TYPE_NM_TPCM_RTS,
	MSG_TYPE_NM_TPCM_CTS,
	MSG_TYPE_NM_TPCM_EOM_ACK,
	MSG_TYPE_NM_TPCM_CON_ABORT,
	MSG_TYPE_NM_TPDT
};

class J1939Id
{
private:
	uint32_t  extendedId;  // 29bit id = 3bit priority, 18bit PGN, 8bit Source Address
	// 18bit PGN = 1bit reserved, 1 bit datapage, 8bit pdu format, 8 bit pdu specific
public:
	J1939Id& operator = (uint32_t identifier)
	{
		extendedId = identifier;
		return *this;
	}

	operator uint32_t()
	{
		return extendedId;
	}

	void setId(uint32_t _extendedId)
	{
		extendedId = _extendedId;
	}

	void setSourceAddress(uint8_t _sourceAddress)
	{
		extendedId = (extendedId & 0xFFFFFF00) | _sourceAddress;
	}

	void setPGN(uint32_t _pgn)
	{
		// 11100000000000000000011111111
		_pgn = _pgn << 8; // move to the pgn position
		extendedId = (extendedId & 0x1C0000FF) | _pgn;
	}

	void setPDUFormat(uint8_t _pduFormat)
	{
		// 11111000000001111111111111111
		uint32_t pduformat = _pduFormat;
		pduformat = pduformat << 16; // MOVE to the pduformat position
		extendedId = (extendedId & 0x1F00FFFF) | pduformat;
	}

	void setDataPage(uint8_t _dataPage)
	{
		// 11110111111111111111111111111
		uint32_t dataPage = _dataPage;
		//pduformat = (extendedId & 0x1F00FFFF);
		dataPage = dataPage << 24; // move to the datapage position
		extendedId = (extendedId & 0x1EFFFFFF) | dataPage;
	}

	void setPriority(uint8_t _priority)
	{
		uint32_t priority = _priority;
		//pduformat = (extendedId & 0x1F00FFFF);
		priority = priority << 26; // move to the priority bit position
		extendedId = (extendedId & 0x3FFFFFF) | priority;
	}

	void setPDUSpecific(uint8_t _pduSpecific)
	{
		//11111111111110000000011111111
		uint32_t pduspecific = _pduSpecific;
		pduspecific = pduspecific << 8; // move to the pduspecific bit position
		extendedId = (extendedId & 0x1FFF00FF) | pduspecific;
	}

	uint8_t getSourceAddress()
	{
		return (uint8_t)(extendedId & 0xFF);
	}

	uint8_t getPDUFormat()
	{
		uint32_t pgn = extendedId & 0x3FFFFFF;// excluding priority 3 bits
		pgn = (pgn >> 16) & 0xFF; // excluding 8bit Source Address and 8 bit Pdu Specific and 1 bit reserved, 1 bit datapage
		return (uint8_t)pgn;
	}

	uint32_t getPGN()
	{
		uint32_t pgn = 0;
		if(getPDUFormat() < 240)
		{

			pgn = extendedId & 0x3FF00FF; // excluding priority 3 bits
			pgn = (pgn >> 8); // excluding 8bit Source Address //and 8 bit Pdu Specific
		}
		else
		{
			// if canid is 0x1CEAA6FD then PGN is EA and pdu specific is A6
			pgn = extendedId & 0x3FFFFFF; // excluding priority 3 bits
			pgn = pgn >> 8; // excluding 8bit Source Address //and 8 bit Pdu Specific
		}
		return pgn;
	}

	uint8_t getDataPage()
	{
		uint32_t pgn = extendedId & 0x3FFFFFF;// excluding priority 3 bits
		pgn = (pgn >> 24) & 1; // excluding 8bit Source Address and 8 bit Pdu Specific and 1 bit reserved
		return (uint8_t)pgn;
	}

	uint8_t getPriority()
	{
		uint32_t pgn = extendedId & 0x1C000000;
		pgn = pgn >> 26;
		return (uint8_t)pgn;
	}

	uint8_t getPDUSpecific()
	{
		uint32_t pgn = (extendedId >> 8) & 0xFF; // excluding priority 3 bits
		return (uint8_t)pgn;
	}
};

template <const unsigned int length = 8>
class _J1939_MSG
{
public:
	EJ1939_MSG_TYPE         msgType;
	EDIRECTION              direction;
	J1939Id                 id;
	unsigned int 			dlc;
	uint8_t            		data[length];
	
	
	_J1939_MSG(uint32_t _id=0, unsigned int dlc=length) 
		: msgType(MSG_TYPE_BROADCAST), direction(DIR_RX), dlc(dlc)
	{
		id.setPGN(_id);
		memset(data, 0, sizeof(data));
	}
	
	_J1939_MSG(const _J1939_MSG& msg)
		: msgType(msg.msgType), direction(msg.direction), id(msg.id), dlc(msg.dlc)
	{
		memcpy(data, msg.data, dlc);
	}
	
	_J1939_MSG& operator=(const _J1939_MSG& msg)
	{
		msgType = msg.msgType;
		direction = msg.direction;
		id = msg.id;
		dlc = msg.dlc;
		memcpy(data, msg.data, dlc);
		return *this;
	}

	bool byteAt(uint16_t index, uint8_t val)
	{
		return setByteAt(data, dlc, index, val);
	}
	bool wordAt(uint16_t index, uint16_t val)
	{
		return setWordAt(data, dlc, index, val);
	}
	bool longAt(uint16_t index, uint32_t val)
	{
		return setLongAt(data, dlc, index, val);
	}
	uint8_t byteAt(uint16_t index)
	{
		return getByteAt(data, dlc, index);
	}
	uint16_t wordAt(uint16_t index)
	{
		return getWordAt(data, dlc, index);
	}
	uint32_t longAt(uint16_t index)
	{
		return getLongAt(data, dlc, index);
	}
};

typedef _J1939_MSG<> J1939_MSG;

template <typename TSignalType, const unsigned int byte_index, const unsigned int start_bit,
			const unsigned int length, const bool intel>
class Signal
{
private:
	uint8_t* m_pchData; 
	const double offset; 
	const double factor;
public:

	Signal& operator = (TSignalType signalValue)
	{
		rawvalue(signalValue);
		return *this;
	}

	operator TSignalType()
	{
		return rawvalue();
	}

	void operator += (TSignalType signalValue)
	{
		rawvalue(rawvalue()+signalValue);
	}
	void operator -= (TSignalType signalValue)
	{
		rawvalue(rawvalue()-signalValue);
	}
	void operator ++ (int)
	{
		rawvalue(rawvalue()+1);
	}
	void operator ++ ()
	{
		rawvalue(rawvalue()+1);
	}

	void operator -- (int)
	{
		rawvalue(rawvalue()-1);
	}
	
	void operator -- ()
	{
		rawvalue(rawvalue()-1);
	}
	
	Signal(uint8_t* m_pchData, const double offset, const double factor)
		: m_pchData(m_pchData), offset(offset), factor(factor)
	{
	}

	TSignalType rawvalue()
	{
		return get_raw_value<TSignalType,byte_index,start_bit,length,intel>(m_pchData);
	}
	
	void rawvalue(TSignalType rawVal)
	{
		set_raw_value<TSignalType,byte_index,start_bit,length,intel>(m_pchData, rawVal);
	}

	double physicalvalue()
	{
		return offset + (get_raw_value<TSignalType,byte_index,start_bit,length,intel>(m_pchData) * factor);
	}
	
	void physicalvalue(double dPhyval)
	{
		set_raw_value<TSignalType,byte_index,start_bit,length,intel>(m_pchData, (TSignalType)((dPhyval - offset) / factor));
	}
};

#endif //J1939INCLUDES_H
