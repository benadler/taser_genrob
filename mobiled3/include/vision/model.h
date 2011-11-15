//
//
//


#ifndef _MODEL_H_
#define _MODEL_H_

#include "image/image.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

struct part
{
  int color;
  int saturation;   // 0..255 (max)
  //float size;
  //float perimeter;

  double xm;
  double ym;
};


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CMODEL
{
  friend class CMODELCLASSIFIER;

private:

  // not even implemented
  CMODEL &operator= (const CMODEL &);
  bool operator== (const CMODEL &);

protected:

public:

  int _numParts;
  struct part *_part;
  char *_name;

  int *_matched;

  CMODEL (const char *name = 0x0);
  CMODEL (const CMODEL &orig);
  virtual ~CMODEL (void);

  int AcquireManual (const CIMAGE &image, char **keys = 0x0);
  int AcquireAutomatic (const CIMAGE &image, CIMAGE *marked = 0x0);
  int AcquireAutomatic2 (const CIMAGE &image, CIMAGE *marked = 0x0);
  int AcquireAutomatic3 (const CIMAGE &image, CIMAGE *marked = 0x0);

  int LoadFile (FILE *fp);
  int LoadFile (const char *fileName);
  int LoadMem (char *model);   // ++itschere20020122

  int SaveFile (FILE *fp);
  int SaveFile (const char *fileName);

  void MarkAllRegions (const CIMAGE &src, CIMAGE &dst);
  void MarkMatchedRegions (CIMAGE &dst);
};


#endif
