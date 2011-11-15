#include "packet.h"

// This constructor is used to create a new packet TO BE SENT
Packet::Packet(unsigned int command)
{
	Q_ASSERT(sizeof(int) == 4);
	Q_ASSERT(sizeof(unsigned int) == 4);
	Q_ASSERT(sizeof(float) == 4);

	logger = Logger::instance();

	// allocate some memory for our buffer
	_bufferSize = 2048;
	_buffer = new unsigned char[_bufferSize];

	// initialze our pointers. Leave out CRC, as we have to put the
	// checksum AFTER the data, and we don't know how long our data is.
        _packetMagic = (unsigned int*) (_buffer + 0);
	_packetSize = (unsigned int*) (_buffer + 4);
        _packetCommand = (unsigned int*) (_buffer + 8);
	_packetData = (unsigned int*) (_buffer + 12);

	// This pointer always points to the current end of the data payload.
	_endOfPayload = (unsigned char*)_packetData;

        *_packetMagic = MAGICNUMBER;
	*_packetSize = 16;	// At the start, we have 4 MAGIC, 4 SIZE, 4 CMD and 4 CHECKSUM
	*_packetCommand = command;

	_port = _ip = 0;
}

bool Packet::send(QAbstractSocket* socket)
{
// 	logger->Packet("Packet::send()");

	if(socket->socketType() == QAbstractSocket::TcpSocket && socket->state() != QAbstractSocket::ConnectedState)
	{
		logger->Packet("Packet::send(): The given TCP socket is not in connected state, cannot send!");
		return false;
	}

	// compute CRC etc.
	finalize();

// 	logger->Packet("Packet::send(): now calling socket->write()");
	if(socket->write((const char*)_buffer, *_packetSize) < 0)
	{
		logger->Packet("Packet::send(): couldn't send the packet: %s.", qPrintable(socket->errorString()));
		return false;
	}
	else
	{
// 		logger->Packet("Packet::send(): now calling socket->waitForBytesWritten(-1)");
// 		socket->waitForBytesWritten(-1);
// 		logger->Packet("Packet::send(): sent packet with command 0x%08x.", getCommand());
		return true;
	}
}

void Packet::finalize()
{
// 	logger->Packet("Packet::finalize()");
	// The packet has been filled and will now be sent. Append the CRC value after the data.
	_packetCRC = (unsigned int*)_endOfPayload;
	*_packetCRC = getCRC();
// 	logger->Packet("Packet::finalize(): finalizing, packet CRC is %d.", *_packetCRC);
}

unsigned int Packet::getCommand(void) const
{
	return *_packetCommand;
}

void Packet::setCommand(const unsigned int command)
{
	*_packetCommand = command;
}

bool Packet::isValid(void) const
{
	return isValidPacket;
}

Packet::~Packet()
{
	delete[] _buffer;
}


/////////////////////////////////////////////////////////////////////////
// PUSH methods
/////////////////////////////////////////////////////////////////////////

void Packet::pushU32(const unsigned int data)
{
	// bail out if there's no space left in buffer
	if(_endOfPayload - _buffer + sizeof(unsigned int) > _bufferSize) abort("Packet::push(): buffer overflow\n");

	// first assign the value, then increment pointer.
	*(unsigned int *)_endOfPayload = data;
	_endOfPayload += sizeof(unsigned int);

	*_packetSize += sizeof(unsigned int);
}

void Packet::pushS32(const signed int data)
{
	// bail out if there's no space left in buffer
	if(_endOfPayload - _buffer + sizeof(signed int) > _bufferSize) abort("Packet::push(): buffer overflow\n");

	// first assign the value, then increment pointer.
	*(signed int *)_endOfPayload = data;
	_endOfPayload += sizeof(signed int);

	*_packetSize += sizeof(signed int);
}

void Packet::pushF32(const float data)
{
	// bail out if there's no space left in buffer
	if(_endOfPayload - _buffer + sizeof(float) > _bufferSize) abort("Packet::push(): buffer overflow\n");

	*(float *)_endOfPayload = data;
        _endOfPayload += sizeof(float);

	*_packetSize += sizeof(float);
}

void Packet::pushD64(const double data)
{
	// bail out if there's no space left in buffer
	if(_endOfPayload - _buffer + sizeof(double) > _bufferSize) abort("Packet::push(): buffer overflow\n");

	*(double *)_endOfPayload = data;
        _endOfPayload += sizeof(double);

	*_packetSize += sizeof(double);
}

/////////////////////////////////////////////////////////////////////////
// POP methods
/////////////////////////////////////////////////////////////////////////

unsigned int Packet::popU32(void)
{
	// we assume that a packet gets ONLY pop()s XOR push()es,
	// so we use the same _endOfPayload pointer

	unsigned int data = *(unsigned int*)_endOfPayload;
	_endOfPayload += sizeof(unsigned int);
	return data;
}

signed int Packet::popS32(void)
{
	// we assume that a packet gets ONLY pop()s XOR push()es,
	// so we use the same _endOfPayload pointer

	signed int data = *(signed int*)_endOfPayload;
	_endOfPayload += sizeof(signed int);
	return data;
}

float Packet::popF32(void)
{
	// we assume that a packet gets ONLY pop()s XOR push()es,
	// so we use the same _endOfPayload pointer

	float data = *(float*)_endOfPayload;
	_endOfPayload += sizeof(float);
	return data;
}

double Packet::popD64(void)
{
	// we assume that a packet gets ONLY pop()s XOR push()es,
	// so we use the same _endOfPayload pointer

	double data = *(double*)_endOfPayload;
	_endOfPayload += sizeof(double);
	return data;
}

/////////////////////////////////////////////////////////////////////////
// PUT methods
/////////////////////////////////////////////////////////////////////////

void Packet::putU32(const unsigned int data, const int offset)
{
	*(unsigned int *)(_packetData + offset) = data;
}

void Packet::putS32(const signed int data, const int offset)
{
	*(signed int *)(_packetData + offset) = data;
}

void Packet::putF32(const float data, const int offset)
{
	*(float *)(_packetData + offset) = data;
}

void Packet::putD64(const double data, const int offset)
{
	*(double *)(_packetData + offset) = data;
}

/////////////////////////////////////////////////////////////////////////
// PEEK methods
/////////////////////////////////////////////////////////////////////////

unsigned int Packet::peekU32(int offset)
{
	return (unsigned int)*(_packetData + offset);
}

signed int Packet::peekS32(int offset)
{
	return (signed int)*(_packetData + offset);
}

float Packet::peekF32(int offset)
{
	return (float)*(_packetData + offset);
}

double Packet::peekD64(int offset)
{
	return (double)*(_packetData + offset);
}


void Packet::resetPayloadPointer(void)
{
	_endOfPayload = _buffer + 12;
}


// This method is used to create a packet from a (received?) buffer
void Packet::setData(const unsigned char* data, const unsigned int length)
{
	if(length > _bufferSize)
	{
		// reallocate enough memory
		delete[] _buffer;
		_buffer = new unsigned char[length];
	}

	_bufferSize = length;

	// copy the data into our own private buffer
	memcpy(_buffer, data, _bufferSize);

	// initialize the pointers like in the constructor
        _packetMagic = (unsigned int*) (_buffer + 0);
	_packetSize = (unsigned int*) (_buffer + 4);
        _packetCommand = (unsigned int*) (_buffer + 8);
	_packetData = (unsigned int*) (_buffer + 12);
	_packetCRC = (unsigned int*) (_buffer + length - 4);

        _port = _ip = 0;

        if(*_packetMagic == MAGICNUMBER && *_packetCRC == getCRC())
	{
                isValidPacket = true;
		//logger->Packet("Packet::Packet(): reconstructed packet from given data; MAGIC is 0x%08x, CRC is %d, size is %d, command is 0x%08x", *_packetMagic, getCRC(), *_packetSize, *_packetCommand);
	}
        else
	{
		isValidPacket = false;
		logger->Packet("Packet::setData(): cannot reconstruct packet. CMD is 0x%08x, MAGIC is 0x%08x, size %d, CRC is %d, should be %d, bufferlength was %d", *_packetCommand, *_packetMagic, *_packetSize, getCRC(), *_packetCRC, length);
// 		abort("");
		for(int i = 0; i < *_packetSize; i++)
		{
			printf("0x%02x\n", _packetData[i]);
		}
		printf("\n\n");
	}
}

unsigned char* Packet::getData(void) const
{
	return _buffer;
}

unsigned int Packet::getDataLength(void) const
{
	// A packet has 4 MAGIC, 4 SIZE, 4 CMD, N DATA and 4 CRC, so
	// the length of DATA is packetlength - MAGIC-SIZE-CMD-CRC

	return *_packetSize - 16;
}

unsigned int Packet::getPacketLength(void) const
{
	// return the whole packet's length in bytes. This is header + data.
	return *_packetSize;
}

unsigned int Packet::getCRC(void) const
{
// 	logger->Packet("Packet::getCRC()");

	// compute CRC over whole packet, minus the last 4 bytes, which is the CRC field itself.
	unsigned int length = *_packetSize - 4;

	if(length < 4)
	{
		logger->Packet("Packet::getCRC(): length is smaller 4, aborting.");
		abort();
	}

	static unsigned int crctab[256];

	static bool initialized = false;

	if(!initialized)
	{
		for(int i = 0; i < 256; i++)
		{
			unsigned int crc = i << 24;

			for (int j = 0; j < 8; j++)
			{
				if (crc & 0x80000000)
				{
					crc = (crc << 1) ^ 0x04c11db7;
				}
				else
				{
					crc = crc << 1;
				}
			}
			crctab[i] = crc;
		}
	}

	unsigned char *data = _buffer;

	unsigned int result;

	result = *data++ << 24;
	result |= *data++ << 16;
	result |= *data++ << 8;
	result |= *data++;

	result = ~ result;

	length -= 4;

	while (length-- > 0)
	{
		result = (result << 8 | *data++) ^ crctab[result >> 24];
	}

	return ~result;
}

