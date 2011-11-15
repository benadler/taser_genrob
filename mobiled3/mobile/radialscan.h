//
//
//

#ifndef _RADIALSCAN_H_
#define _RADIALSCAN_H_


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CRADIALSCAN
{
private:

  // not even implemented
  CRADIALSCAN (const CRADIALSCAN &);
  bool &operator== (const CRADIALSCAN &);

protected:

public:

  CRADIALSCAN (void);
  virtual ~CRADIALSCAN (void);

  CRADIALSCAN &operator= (const CRADIALSCAN &right);

  // scan data

  int _maxScans;
  int _numScans;
  float *_scanAngle;
  float *_scanDist;

  // mark data

  int _maxMarks;
  int _numMarks;
  float *_markAngle;
  float *_markDist;
};


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CRADIALSCANMATCHED : public CRADIALSCAN
{
private:

  // not even implemented
  CRADIALSCANMATCHED (const CRADIALSCANMATCHED &);
  bool &operator== (const CRADIALSCANMATCHED &);

protected:

public:

  CRADIALSCANMATCHED (void);
  virtual ~CRADIALSCANMATCHED (void);

  CRADIALSCANMATCHED &operator= (const CRADIALSCAN &right);
  CRADIALSCANMATCHED &operator= (const CRADIALSCANMATCHED &right);

  int *_markIdx;
};


#endif
