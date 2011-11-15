//
//
//

#ifndef _RAWLASER_H_
#define _RAWLASER_H_

#include "./radialscan.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class _CRAWLASER
{
private:

  // not even implemented

  _CRAWLASER (const _CRAWLASER &);
  _CRAWLASER operator= (const _CRAWLASER &);
  _CRAWLASER operator== (const _CRAWLASER &);

protected:

public:

  //
  //
  //

  _CRAWLASER (void)
    {
    }

  virtual ~_CRAWLASER (void);

  //
  //
  //

  virtual void Reset (void) = 0x0;

  virtual void StartScanContinuous (void) = 0x0;

  virtual void GetScanContinuous (CRADIALSCAN &scan) = 0x0;

  virtual void GetSerialPortStatistics (int &rx,
					int &rxOverrun,
					int &tx);
};


#endif
