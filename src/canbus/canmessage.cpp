#include "canmessage.h"

CanMessage::CanMessage(void)
{
	_id = 0;

	_data[0] = 0;
	_data[1] = 0;
	_data[2] = 0;
	_data[3] = 0;
	_data[4] = 0;
	_data[5] = 0;
	_data[6] = 0;
	_data[7] = 0;
}

CanMessage::CanMessage(
		unsigned short id,
		const unsigned char data0,
		const unsigned char data1,
		const unsigned char data2,
		const unsigned char data3,
		const unsigned char data4,
		const unsigned char data5,
		const unsigned char data6,
		const unsigned char data7)
{
	_id = id;

	_data[0] = data0;
	_data[1] = data1;
	_data[2] = data2;
	_data[3] = data3;
	_data[4] = data4;
	_data[5] = data5;
	_data[6] = data6;
	_data[7] = data7;
}

CanMessage::~CanMessage(void)
{
}

int CanMessage::getId(void) const
{
	return _id;
}

void CanMessage::setId(int id)
{
	_id = id;
}

void CanMessage::setData(
		const unsigned char data0,
		const unsigned char data1,
		const unsigned char data2,
		const unsigned char data3,
		const unsigned char data4,
		const unsigned char data5,
		const unsigned char data6,
		const unsigned char data7)
{
	_data[0] = data0;
	_data[1] = data1;
	_data[2] = data2;
	_data[3] = data3;
	_data[4] = data4;
	_data[5] = data5;
	_data[6] = data6;
	_data[7] = data7;
}

unsigned char* CanMessage::getData(void)
{
	return _data;
}

unsigned char CanMessage::getData(int bytenumber)
{
	return _data[bytenumber];
}

int CanMessage::getError(void) const
{
	// As documented in can.h, the error is the last two bits of the last data byte
	return _data[7] & 0x03;
}

void CanMessage::reset(void)
{
	setId(0);
	setData(0,0,0,0,0,0,0,0);
}
