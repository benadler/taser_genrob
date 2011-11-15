//
// sicklaser.h
//

#ifndef _SICKLASER_H_
#define _SICKLASER_H_

#include "./sickrawlaser.h"
#include "./rs422.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CSICKLASER : protected CRS422, public _CSICKRAWLASER
{
private:

  // not even implemented

  CSICKLASER (void);
  CSICKLASER (const CSICKLASER &);
  CSICKLASER operator= (const CSICKLASER &);
  CSICKLASER operator== (const CSICKLASER &);

  //

  int _idx;
  bool _hiSpeed;

  // variable(s), used in continuous mode

#define BUFSIZE 1024

  unsigned char _scanBuf[BUFSIZE];
  int _inBuf;
  unsigned int _msgOk, _msgKo;

  // utilities

  int WriteSick (const unsigned char *buf, const int size) const;

  unsigned short ComputeCrc (unsigned char *ptr, int len);

  int SendTelegram (const unsigned char *buf, int len, const bool wait = true);
  int ReceiveTelegram (unsigned char *buf, const int maxLen);

  // low level

  int ResetSick (void);
  int GetType (void);
  int Mode (unsigned char mode, const bool wait = true);

#if 0
  int ScanContinuously (void);
#endif

  //

protected:

  virtual void Reset (void);

public:

  //
  //
  //

  CSICKLASER (const int idx, const bool hiSpeed = true);

  ~CSICKLASER (void);

  //
  //
  //

  virtual void StartScanContinuous (void);

  virtual void GetScanContinuousSick (CSICKSCAN &scan);

  virtual void GetSerialPortStatistics (int &rx,
					int &rxOverrun,
					int &tx);
};


#endif
