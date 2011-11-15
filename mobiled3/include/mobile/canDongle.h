//
//
//

#ifndef _CANDONGLE_H_
#define _CANDONGLE_H_

struct reply
{
  unsigned short id;
  unsigned char cmd;
  bool gotcha;
  unsigned char buf[8];
};


class CCANDONGLE
{
private:

  // not even implemented
  CCANDONGLE (const CCANDONGLE &);
  CCANDONGLE &operator= (const CCANDONGLE &);
  bool operator== (const CCANDONGLE &);

protected:

  int _fd;

  CCANDONGLE (void);

  int Send (const unsigned short id,
	    const unsigned char data1,
	    const unsigned char data2 = 0,
	    const unsigned char data3 = 0,
	    const unsigned char data4 = 0,
	    const unsigned char data5 = 0,
	    const unsigned char data6 = 0,
	    const unsigned char data7 = 0,
	    const unsigned char data8 = 0);

  // Receive() returns the ID of the controller which sent the packet

  int Receive (const int maxSize,
	       unsigned char *data);

  int ReceiveTimeout (const int maxSize,
		      unsigned char *data,
		      const unsigned int milliSeconds,
		      const bool ommitWarnings = false);

  int ReceiveMultipleTimeout (const int numReplies,
			      struct reply *replies,
			      const unsigned int milliSeconds);

public:

  virtual ~CCANDONGLE (void);
};


#endif
