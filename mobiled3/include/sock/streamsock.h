//
// streamsock.h -
// (C) 08/1999 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//


#ifndef _STREAMSOCK_H
#define _STREAMSOCK_H


#include "sock/sock.h"


class WINFLAGS CSTREAMSOCK : public _CSOCK
{
private:

protected:

  CSTREAMSOCK (const int fd,
	       unsigned int *rem_ipv4_addr,
	       unsigned short *rem_ipv4_port);

public:

  CSTREAMSOCK (void);
  virtual ~CSTREAMSOCK (void);

  int Bind (unsigned short local_ipv4_port,
	    unsigned int local_ipv4_addr = 0) const;

  int Listen (const int backlog) const;

  CSTREAMSOCK *Accept (unsigned int *rem_ipv4_addr = 0,
		       unsigned short *rem_ipv4_port = 0) const;

  int Connect (const unsigned int rem_ipv4_addr,
	       const unsigned short rem_ipv4_port,
	       const int timeout_s = -1) const;
  int Connect (const char *rem_ipv4_addr,
	       const unsigned short rem_ipv4_port,
	       const int timeout_s = -1) const;

  //
  // Read()
  //
  // blocking read() until all requested data is received (using multiple
  // system read()s if necessary).
  //

  int Read (int size, void *buf) const;

  //
  // ReadTimeout1(), ReadTimeout2()
  //
  // ReadTimeout1() is a Read() with a timeout. the function will try reading
  // exactly as much data as requested by means of calling ReadSomeTimeout() in
  // a loop, but will return whatever it has got if for the duration of the
  // timeout no new data was received. technically it just passes the timeout
  // value unchanged to each new ReadSomeTimeout() call. note that this still
  // means that the complete read may take an arbitrary amount of time, just
  // the time between two chunks is limited. this may especially lead to
  // unexpected behaviour if data comes in in very small chunks, say, single
  // bytes with the minimal allowed rate. the timeout will therefore typically
  // be a rather small value, compared with ReadTimeout2().
  //
  // ReadTimeout2() is similar to ReadTimout1(), except that the timeout is
  // applied to the complete transfer. technically this means that the timeout
  // passed to each ReadSomeTimeout() is reduced by the amount of time that has
  // already elapsed. the timeout will therefore typically be a rather large
  // value, compared with ReadTimeout1(). note that this function may return
  // with less than the required amount of data because of the timeout even if
  // new data is continuously coming in.
  //
  // note that both ReadTimeout() are a lot more inefficient than Read(), so
  // use them only when you're damn shure you need them.
  //

  int ReadTimeout1 (int size, void *buf, const milliseconds_t timeout) const;
  int ReadTimeout2 (int size, void *buf, const milliseconds_t timeout) const;

  //
  // ReadSome()
  //
  // returns all immediately available data. if no data is available it waits
  // until at least 1 byte becomes available. as soon as at least 1 byte is
  // available all available data is returned. in all cases the amount of data
  // returned is clipped against `size', possibly leaving some data with the
  // system.
  //

  int ReadSome (int size, void *buf) const;

  //
  // ReadSomeTimeout()
  //
  // returns all immediately available data. if no data is available it waits
  // until either at least 1 byte becomes available or the specified amount of
  // time elaspes. if any of these condition becomes true all currently
  // available data is returned (including nothing). in all cases the amount of
  // data returned is clipped against `size', possibly leaving some data with
  // the system.

  int ReadSomeTimeout (int size, void *buf, const milliseconds_t timeout) const;

  //

  int Write (int size, const void *buf) const;   // blocking

  int Shutdown (void) const;
};


#endif // _STREAMSOCK_H
