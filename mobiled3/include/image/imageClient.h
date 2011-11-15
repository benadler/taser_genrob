//
//
//

#ifndef _IMAGECLIENT_H_
#define _IMAGECLIENT_H_

#include "labdev/labdev.h"
#include "labdev/labcmd.h"

#include "image/image.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CIMAGECLIENT : public CLABDEV
{
public:

  CIMAGECLIENT (void);
  virtual ~CIMAGECLIENT (void);

  // get an image of TYPE_YUV888,SIZE_HALF from a remote server. the image size
  // is returned (or -1 in case of an error).

  int GetYuvImage (CIMAGE &image, const EGRABSIZE size);

  // get a JPG image of SIZE_HALF from a remote server. the image memory is
  // newly allocated in `image', not free()ing any possible older image! the
  // image size is returned (or -1 in case of an error).

  int GetJpgImage (void **image, const int quality);
};


#endif
