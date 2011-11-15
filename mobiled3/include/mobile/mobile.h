//
//
//

#ifndef _MOBILE_H_
#define _MOBILE_H_


#include "mobile/telegram.h"
#include "mobile/mobiletypes.h"
#include "math/vec.h"
#include "iotypes.h"

class CMOBILE
{
private:

  // not even implemented

  CMOBILE (void);
  CMOBILE (const CMOBILE &);
  CMOBILE &operator= (const CMOBILE &);
  bool operator== (const CMOBILE &);

protected:

  CSTREAMSOCK _sock;

  CRCVBUFFER _buffer;

  CRCVTELEGRAM *GetReply (unsigned int id);

public:

  CMOBILE (const unsigned int addr, const unsigned short port = 9002);
  CMOBILE (const char *hostname, const unsigned short port = 9002);

  virtual ~CMOBILE (void);

  //

  int Ping (void);
  void CheckProtocol (void);
  int Debugging (const int flag);

  //

  int GetPosition (meter_t &x, meter_t &y, degree_t &a);
  int SetPosition (meter_t  x, meter_t  y, degree_t  a);

  int GetPositionAndVariance (meter_t &x, meter_t &y, degree_t &a,
			      double &varXX,
			      double &varXY,
			      double &varYX,
			      double &varYY,
			      double &varAA);
  int GetPositionOdo (meter_t &x, meter_t &y, degree_t &a);

  int GetAllMarks (CVEC &marks);
  int GetAllMarksInitial (CVEC &marks);
  int GetAllMarksWithVariances (CVEC &marks, CVEC &variances);

  int GetAllLines (CVEC &lines);

  int Stop (void);
  int Forward (const meter_t d);
  int RotateAngle (const degree_t a);
  int RotatePoint (const meter_t xHead, const meter_t yHead);
  int Move (const meter_t x, const meter_t y, const degree_t a);
  int MovePoint (const meter_t x, const meter_t y);
  int IsCompleted (void);
  int WaitForCompleted (void);

  /////////////////////////////////////////////////////////////////////////////
  //
  // map related stuff
  //
  /////////////////////////////////////////////////////////////////////////////

  class CMAP
  {
    friend class CMOBILE;
  private:
    // not even implemented
    CMAP (const CMAP &);
    CMAP &operator= (const CMAP &);
    bool operator== (const CMAP &);
  protected:
  public:
    CMAP (void)
    {
      numLines = numMarks = 0;
    }
    virtual ~CMAP (void);
    int numLines;
    CVEC line;
    int numMarks;
    CVEC mark;
  };

  int GetMap (CMAP &map);

  /////////////////////////////////////////////////////////////////////////////
  //
  // scan related stuff
  //
  /////////////////////////////////////////////////////////////////////////////

  class CSCAN
  {
    friend class CMOBILE;
  private:
    // not even implemented
    CSCAN (const CSCAN &);
    CSCAN &operator= (const CSCAN &);
    bool operator== (const CSCAN &);
  protected:
  public:
    CSCAN (void)
    {
      numScans = numMarks = 0;
    }
    virtual ~CSCAN (void);
    int numScans;
    float sx[361];
    float sy[361];
    int numMarks;
    float mx[180];
    float my[180];
    int mIdx[180];
  };

  int GetNumScanners (void);
  int GetScannerPosition (meter_t &x, meter_t &y, degree_t &a, const unsigned int idx);
  int GetScanPlatform (CSCAN &scan, const unsigned int idx);
  int GetScanWorld    (CSCAN &scan, const unsigned int idx);

  /////////////////////////////////////////////////////////////////////////////
  //
  //
  //
  /////////////////////////////////////////////////////////////////////////////

  int ComputeError (const meter_t xe,
		    const meter_t ye,
		    const degree_t ae,
		    meter_t *xErr,
		    meter_t *yErr,
		    degree_t *aErr);

  int MoveDirect (const meter_t xe,
		  const meter_t ye,
		  const degree_t ae,
		  meter_t &xErr,
		  meter_t &yErr,
		  degree_t &aErr);

  int MoveViaIntermediate (const meter_t xe,
			   const meter_t ye,
			   const degree_t ae,
			   meter_t &xErr,
			   meter_t &yErr,
			   degree_t &aErr,
			   const meter_t approachDistance);

  int MoveViaIntermediate (const meter_t xe,
			   const meter_t ye,
			   const degree_t ae,
			   const meter_t approachDistance)
    {
      meter_t xErr;
      meter_t yErr;
      degree_t aErr;

      return MoveViaIntermediate (xe, ye, ae,
				  xErr, yErr, aErr,
				  approachDistance);
    }

  /////////////////////////////////////////////////////////////////////////////
  //
  //
  //
  /////////////////////////////////////////////////////////////////////////////

#if 0
  int GetSpeed (meter_t &tSpeed,
		degree_t &rSpeed);

  int SetSpeed (const meter_t tSpeedNew,
		const degree_t rSpeedNew,
		meter_t &tSpeedOld,
		degree_t &rSpeedOld);
#endif

  int GetScale (double &tScale,
		double &rScale);

  int SetScale (const double tScale,
		const double rScale,
		double &tScaleRet,
		double &rScaleRet);

  int ModifyScale (const double tScaleFactor,
		   const double rScaleFactor,
		   double &tScaleOld,
		   double &rScaleOld);

  //

  int SetScale (const double tScale,
		const double rScale)
  {
    double dummy;

    return SetScale (tScale, rScale, dummy, dummy);
  }

  /////////////////////////////////////////////////////////////////////////////
  //
  //
  //
  /////////////////////////////////////////////////////////////////////////////

  MOBILEMODE GetMode (void);                    // returns old mode
  MOBILEMODE SetMode (const MOBILEMODE mode);   // returns old mode
};


#endif
