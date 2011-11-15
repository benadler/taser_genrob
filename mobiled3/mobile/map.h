//
// map.h
//

#ifndef _MAP_H_
#define _MAP_H_

#include "./defines.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CMAP
{
public:

  typedef float datatype_t;

  typedef struct
  {
    datatype_t x0, y0, xe, ye;
  } SLINE_t;

  typedef struct
  {
    datatype_t x, y;
  } SMARK_t;

private:

  // not even implemented

  CMAP (const CMAP &);
  CMAP &operator= (const CMAP &);
  bool operator== (const CMAP &);

protected:

  int _maxLines;

  void LoadMap (const char *fName);

  int _maxMarks;

  void LoadMarks (const char *fName);

public:

  //

  int _numLines;
  SLINE_t *_line;

  int _numMarks;
  SMARK_t *_mark;

  //

  CMAP (void)
  {
    _line = 0x0;
    _numLines = _maxLines = 0;
    LoadMap (MAPFILE);

    _mark = 0x0;
    _numMarks = _maxMarks = 0;
    LoadMarks (MARKFILE);
  }

  virtual ~CMAP (void);
};


#endif
