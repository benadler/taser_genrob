//
// rawlaser.cc
//

#include "./rawlaser.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

_CRAWLASER::~_CRAWLASER (void)
{
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void _CRAWLASER::GetSerialPortStatistics (int &rx,
					  int &rxOverrun,
					  int &tx)
{
  rx = rxOverrun = tx = 0;
}