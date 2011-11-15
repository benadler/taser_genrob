#ifndef PACKET_H
#define PACKET_H

#include <QTcpSocket>
#include <logger.h>

#define MAGICNUMBER 0x4f4c4548

/// @class Packet
/// @brief This class represents a network packet containing a command and possibly data.
/// Packets of this class will be sent between the UdpServer and its client, as well
/// as MobileD and its client.
///
/// 4 bytes MAGIC
/// 4 bytes SIZE (of the whole packet)
/// 4 bytes COMMAND
/// n bytes DATA
/// 4 bytes CRC (of MAGIC, SIZE, COMMAND and DATA).

class Packet
{
	private:
		// just for debugging.
		void resetPayloadPointer(void);

		unsigned int getCRC() const;

		Logger *logger;
		int _ip;			///< for received packets, this is the source ip, for packets to be sent, its the destination ip
		int _port;			///< for received packets, this is the source port, for packets to be sent, its the destination port

		unsigned char* _endOfPayload;	///< a pointer to the current end-of-data in the buffer. Used when pushing more data into it.
		unsigned char* _buffer;		///< the data in this packet.
		unsigned int _bufferSize;	///< the buffersize in bytes

		unsigned int* _packetMagic;	///< a pointer to the 4 bytes MAGIC-number
		unsigned int* _packetSize;	///< a pointer to the 4 bytes datalength in the buffer
		unsigned int* _packetCommand;	///< a pointer to the 4 bytes command-number
		unsigned int* _packetData;	///< a pointer to the N bytes of data
		unsigned int* _packetCRC;	///< a pointer to the 4 bytes CRC in the buffer

		bool isValidPacket;		///< this bool is true if MAGIC number and CRC match

	public:
		Packet(unsigned int command = 0);
		~Packet(void);

		unsigned int getCommand(void) const;
		void setCommand(const unsigned int command);

		void pushU32(const unsigned int data);
		void pushS32(const signed int data);
		void pushD64(const double data);
		void pushF32(const float data);

		void putU32(const unsigned int data, const int offset);
		void putS32(const signed int data, const int offset);
		void putF32(const float data, const int offset);
		void putD64(const double data, const int offset);

		unsigned int	popU32(void);
		signed int	popS32(void);
		double		popD64(void);
		float		popF32(void);

		unsigned int	peekU32(const int offset);
		signed int	peekS32(const int offset);
		float		peekF32(const int offset);
		double		peekD64(const int offset);

		void setData(const unsigned char* data, const unsigned int length);
		unsigned char* getData(void) const;

		/// returns the data's length in bytes.
		unsigned int getDataLength(void) const;
		unsigned int getPacketLength() const;

		bool send(QAbstractSocket* socket);

		bool isValid(void) const;
		void finalize();
};

#endif
