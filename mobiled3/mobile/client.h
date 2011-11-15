//
//
//

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "sock/daemon.h"
#include "sock/streamsock.h"
#include "thread/thread.h"
#include "thread/condition.h"

#include "genBase.h"
#include "mobile/telegram.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CCLIENT : public CCONNECTION, protected CDETACHEDTHREAD
{
private:

  // not even implemented
  CCLIENT (void);
  CCLIENT (const CCLIENT &);
  CCLIENT &operator= (const CCLIENT &);
  bool operator== (const CCLIENT &);

  //

protected:

  CGENBASE *_genBase;

  CRCVBUFFER _buffer;
  CCONDITION _cond;

  virtual void DataAvailable (CDAEMON *daemon);
  virtual void Fxn (void);

  //

  void Execute (CRCVTELEGRAM *telegram);

  //

  void Ping (void) const;
  void CheckProtocol (void) const;
  void Debugging (CRCVTELEGRAM *telegram) const;

  void GetPosition (void) const;
  void GetPositionOdo (void) const;
  void SetPosition (CRCVTELEGRAM *telegram) const;
  void GetPositionAndVariance (void) const;

  void GetAllMarks (void) const;
  void GetAllMarksInitial (void) const;
  void GetAllMarksWithVariances (void) const;

  void GetAllLines (void) const;

  void GetNumScanners (void) const;
  void GetScannerPosition (CRCVTELEGRAM *telegram) const;
  void GetScanScanner (CRCVTELEGRAM *telegram) const;          // ++westhoff20050808
  void GetScanRadialScanner (CRCVTELEGRAM *telegram) const;    // ++westhoff20050715
  void GetScanPlatform (CRCVTELEGRAM *telegram) const;
  void GetScanWorld (CRCVTELEGRAM *telegram) const;

  void StopMotion (void) const;
  void Translate (CRCVTELEGRAM *telegram) const;
  void RotateAngle (CRCVTELEGRAM *telegram) const;
  //  void RotatePoint (CRCVTELEGRAM *telegram) const;         // --westhoff20060905
  void TurnAngle (CRCVTELEGRAM *telegram) const;               // ++westhoff20060421
  void Move (CRCVTELEGRAM *telegram) const;
  //  void MovePoint (CRCVTELEGRAM *telegram) const;           // --westhoff20060905
  void Forward (CRCVTELEGRAM *telegram) const;

  void IsCompleted (void) const;
  void WaitForCompleted (void) const;

  void Stalled (void) const;           // ++westhoff20050506

  void ApplyBrakes (void) const;       // ++westhoff20050803
  void ReleaseBrakes (void) const;     // ++westhoff20050803
  void AreBrakesReleased (void) const; // ++westhoff20050803


  void GetScale (void) const;
  void SetScale (CRCVTELEGRAM *telegram) const;
  void ModifyScale (CRCVTELEGRAM *telegram) const;

  void GetMode (void) const;
  void SetMode (CRCVTELEGRAM *telegram) const;

  // ++westhoff20050202:
  void StartOdoLogging (void) const;
  void StopOdoLogging (void) const;

  // ++westhoff20050330:
  void GetBatteryVoltage (void) const;
  void GetDriveTemperatures (void) const;

public:

  CCLIENT (CSTREAMSOCK *sock, CGENBASE *genBase);

  virtual ~CCLIENT (void);
};


#endif
