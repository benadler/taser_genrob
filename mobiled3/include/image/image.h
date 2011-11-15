//
// image.h
// (C) 12/2000 by TeSche (Torsten Scherer)
// <itschere@techfak.uni-bielefeld.de>
//

#ifndef _IMAGE_H_
#define _IMAGE_H_


// some includes

#include <string.h>
#include <X11/Xlib.h>


// some types

typedef enum EGRABTYPE
{
  TYPE_NONE = 0,
  TYPE_GRAY8 = 1,       // G
  TYPE_GREY8 = 1,       // G
  TYPE_RGB888,          // RGB
  TYPE_RGB888_METEOR,   // BGRx
  TYPE_YUV888           // YUV
};


typedef enum EGRABSIZE
{
  SIZE_UNDEF = 0,
  SIZE_FULL,
  SIZE_HALF,
  SIZE_HALF_SPECIAL,
  SIZE_HALF_SPECIAL2
#if 0
  SIZE_QUARTER // may be usefull for YUV images where color information is only
               // available for every second pixel anyway
#endif
};


typedef enum EGRABCHANNEL
{
  CHANNEL_UNDEF,
  CHANNEL_SVHS,
  CHANNEL_COAX
};


///////////////////////////////////////////////////////////////////////////////
//
// the big one... no, not really. :-)
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

class _CTMPIMAGE
{
  friend class CIMAGE;

private:

  // not even implemented
  _CTMPIMAGE &operator= (const _CTMPIMAGE &);

protected:

  EGRABTYPE _type;   // if TYPE_NONE the following are meaningless...
  int _width, _height;
  int _size;
  unsigned char *_data;   // ...except for this

  _CTMPIMAGE (void)
  {
    _type = TYPE_NONE;
    _data = 0x0;
  }

public:

  virtual ~_CTMPIMAGE (void);

  _CTMPIMAGE (const _CTMPIMAGE &right);

  // low level routines

  EGRABTYPE GetType (void) const {return _type;};
  int GetWidth (void) const {return _width;};
  int GetHeight (void) const {return _height;};
  int GetSize (void) const {return _size;};
  const unsigned char *GetData (void) const {return _data;};
  unsigned char *GetData (void) {return _data;};

  int Create (const EGRABTYPE type,
	      const int width,
	      const int height,
	      void * const ptr = 0x0);

  int CreateClear (const EGRABTYPE type,
		   const int width,
		   const int height);

  // move.cc

  _CTMPIMAGE Move (const int dx,
		   const int dy) const;

  void MoveSelf (const int dx,
		 const int dy);

  // crop.cc

  _CTMPIMAGE Crop (const int x0,
		   const int y0,
		   const int width,
		   const int height) const;
  void CropSelf (const int x0,
		 const int y0,
		 const int width,
		 const int height);

  // scale.cc

  _CTMPIMAGE Scale (const int width,
		    const int height) const;

  void ScaleSelf (const int width,
		  const int height);

  // size.cc

  _CTMPIMAGE AdjustSize (const int width,
			 const int height) const;

  void AdjustSizeSelf (const int width,
		       const int height);

  // shrink.cc

  void ShrinkSmoothSelf (const int newSidth,
			 const int newHeight);

  // paste.cc

  void Paste (const int x0,
	      const int y0,
	      const _CTMPIMAGE &image);

  // invert.cc

  _CTMPIMAGE Invert (void) const;

  void InvertSelf (void);

  // deskew.cc

  _CTMPIMAGE Deskew (void) const;

  void DeskewSelf (void);
};


///////////////////////////////////////////////////////////////////////////////
//
// this is the big one.
//
///////////////////////////////////////////////////////////////////////////////

class CIMAGE : public _CTMPIMAGE
{
private:

  // not even implemented

protected:

  int greyCol;
  int rCol, gCol, bCol;   // not initialized by default

  char *_title;

  Window win;   // if 0 the following are all meaningless
  int winX, winY;
  int winWidth, winHeight;
  GC gc;
  XImage *image;   // if NULL the following is meaningless
  void *imageData;

  int LoadPPM (const int fh);
  int SavePPM (const int fh) const;

public:

  CIMAGE (const char * const title = 0x0)
    {
      if (title)
	{
	  _title = ::strdup (title);
	}
      else
	{
	  _title = 0x0;
	}

      win = 0;
      image = 0x0;
    }

  CIMAGE (const EGRABTYPE newType,
	  const int width,
	  const int height,
	  const char * const title = 0x0)
    {
      _type = TYPE_NONE;

      if (title)
	{
	  _title = ::strdup (title);   // if this fails it fails...
	}
      else
	{
	  _title = 0x0;
	}

      win = 0;
      image = 0x0;

      Create (newType, width, height);
    }

  virtual ~CIMAGE (void);

  CIMAGE (const _CTMPIMAGE &src);   // not really const
  CIMAGE (const CIMAGE &src);

  //CIMAGE &operator= (const _CTMPIMAGE &);
  CIMAGE &operator= (const CIMAGE &);

  // I/O

  int LoadPPM (const char * const filename);
  int SavePPM (const char * const filename) const;
  int SaveGzipPPM (const char * const filename, const int zipLevel = 9) const;

  int LoadJPG (const char * const filename);
  int SaveJPG (const char * const filename,
	       const int quality = 75,
	       const int smooth = 0) const;

  // a grab routine (currently Matrox Meteor-1 @ Linux only)

  virtual int Grab (const EGRABTYPE newType,
		    const EGRABSIZE newSize,
		    const EGRABCHANNEL newChannel);

  // show an image on a X11 display

  int SetTitle (const char *title);

  const char *GetTitle (void) const
    {
      return _title;
    }

  /////////////////////////////////////////////////////////////////////////////
  //
  // in `imageShow.cc'
  //
  /////////////////////////////////////////////////////////////////////////////

  int Show (const int x, const int y);

  int Show (void)
  {
    return Show (-1, -1);
  }

  int GetPointerCoords (int &x, int &y);

  static int GetDisplaySize (int &width, int &height);

  /////////////////////////////////////////////////////////////////////////////
  //
  //
  //
  /////////////////////////////////////////////////////////////////////////////

  // drawing primitives (not all images types supported (yet))

  int SetColor (const int gray);
  int SetColor (const int r, const int g, const int b);
  int GetColor (int *r, int *g, int *b) const;

  int Plot (const int x, const int y);
  int Hline (int x0, int y0, int xe);
  int Vline (int x0, int y0, int ye);

  int Box (const int x0, const int y0, int width, int height);
  int Pbox (const int x0, const int y0, int width, int height);

  int Circle (int x0, int y0, int r);
  int Pcircle (int x0, int y0, int r);

  int Ellipse (const int x0, const int y0,
	       const double angle,
	       const int r1, const int r2);

  int Line (const int x0, const int y0, const int x0, const int ye);

  //

  int ConvertToGreyscale (const float redScale,
			  const float greenScale,
			  const float blueScale);

  int ConvertToGreyscale (const float redScale,
			  const float greenScale,
			  const float blueScale,
			  const float offset);

  int ConvertToGreyscale (void)
  {
    return ConvertToGreyscale (0.21, 0.67, 0.12);
  }

  // some color space conversions. may reduce to a dummy if the image is
  // already in the desired format.

  // int ConvertToHsv (void);
  int ConvertToRgb (void);
  int ConvertToYuv (void);

  // some color space splits / combines. doesn't matter which color space the
  // image is in because they all use three parameters.

  int Split (CIMAGE &c1,
	     CIMAGE &c2,
	     CIMAGE &c3) const;

  int Combine (const EGRABTYPE newType,
	       const CIMAGE &c1,
	       const CIMAGE &c2,
	       const CIMAGE &c3);

  // other low level image manipulations

  int Average (const CIMAGE &i1,
	       const CIMAGE &i2);

  int Average (const CIMAGE &i1,
	       const CIMAGE &i2,
	       const CIMAGE &i3);

  int Average (const CIMAGE &i1,
	       const CIMAGE &i2,
	       const CIMAGE &i3,
	       const CIMAGE &i4,
	       const CIMAGE &i5,
	       const CIMAGE &i6,
	       const CIMAGE &i7,
	       const CIMAGE &i8);

  int Quantize (int levels);

  int MaximizeContrast (void);
  int MaximizeContrast (unsigned char neutral);

  int HalveSize (void);

  int Dim (const double factor);

  int LowerThreshold (const int threshold);

  // int SUSAN (void);

  //
  // Smooth() computes an average filter on gray scale images.
  //
  // averages are typically very sensitive to outliers in the source data, so
  // if that is a problem try using Median() instead.
  //

  int SmoothSelf (const int filterSize = 1);
  int VarianceSelf (const int filterSize = 1);

  //
  // Median() computes a median filter on gray scale images.
  //
  // it numerically sorts the (2*`filterSize'+1)^2 source pixels around each
  // pixel, then ommitting `ommit' pixels at each margin and using the average
  // value of the remaining pixels as value for the target pixel. It is
  // therefore much less sensitive to limited outliers in the source data.
  //

  int MedianSelf (const int filterSize, int ommit);

  //
  // SmartSmooth() computes a `smart' average on gray scale images.
  //
  // it is smart in that it uses the variance in the filter core to compute a
  // measure f=0..1 to weigh the original (f) and the average pixel (1-f) value
  // to obtain a behaviour that smoothes already similar regions but keeps
  // sharp edges intact.
  //

  int SmartSmoothSelf (const int filterSize, const double scale);

  int Edges (void);   // don't use this!!!

  int EdgesHorizontal (void);   // don't use this!!!
  int EdgesVertical (void);   // don't use this!!!

  //
  // operator* ("AND", "MIN"): weigh an (gray, rgb, yuv) image with a
  // (grayscale) image. actually it's not quite a real "MIN" operator, but
  // something similar...
  //

  _CTMPIMAGE operator* (const CIMAGE &weight) const;
  CIMAGE &operator*= (const CIMAGE &weight);

  // operator| ("OR", "MAX"): combine two (gray) images

  _CTMPIMAGE operator| (const CIMAGE &right) const;
  CIMAGE &operator|= (const CIMAGE &right);

  //

  _CTMPIMAGE operator- (const _CTMPIMAGE &sub) const;
};


#endif
