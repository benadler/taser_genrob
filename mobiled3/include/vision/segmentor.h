//
//
//

#ifndef _SEGMENTOR_H_
#define _SEGMENTOR_H_

#include "image/image.h"

class CSEGMENTOR
{
private:

  // not even implemented
  CSEGMENTOR (const CSEGMENTOR &);
  CSEGMENTOR &operator= (const CSEGMENTOR &);

protected:

  int SearchBrightestPixel (const CIMAGE &image,
			    int *xPtr,
			    int *yPtr);

public:

  class CREGION
    {
    private:
      // not even implemented;
      CREGION (const CREGION &);
      CREGION &operator= (const CREGION &);
    public:
      CIMAGE maskImage;   // region as white (255) mask in GRAY (0..255) image
      CIMAGE similarityImage;   // region as gray (1..255) mask in GRAY (0..255) image
      int xRoot, yRoot;   // root pixel (with highest ranking)
      int x0, y0, xe, ye;   // bounding box of region
      int xs, ys;        // center of gravity coordinates
      double xsf, ysf;   // center of gravity coordinates with more precision
      int size;          // number of pixels
      int hullSize;   // number of hull pixels, to appear first in `coords' if
                      // `flags & SEGMENTOR_HULLPIXELS' and 0 otherwise.
      short *coords;     // coordinates of pixels {x1, y1, x2, y2, x3, y3...}
      CREGION (void)
	{
	  size = -1;
	  coords = 0x0;
	}
      virtual ~CREGION (void);
      int FindHull (void);
      int FindBoundingBox (void);
    };

 public:

  int Fill (CIMAGE &image,
	    int x0,
	    int y0,
	    const int threshold,
	    CREGION &region);

  int Fill2 (CIMAGE &image,
	     int x0,
	     int y0,
	     const int threshold,
	     CREGION &region);

 public:

  int Fill (CIMAGE &u,
	    CIMAGE &v,
	    int x0,
	    int y0,
	    CREGION &region);

protected:

  void Project (const CIMAGE &uImage,
		const CIMAGE &vImage,
		const double angle,
		CIMAGE &dist,
		const bool verbose = false);

  void Project2 (const CIMAGE &uImage,
		 const CIMAGE &vImage,
		 const double angle,
		 CIMAGE &dist,
		 const double satMin,
		 const double satMax);

public:

  CSEGMENTOR (void)
  {
  }

  virtual ~CSEGMENTOR (void);

  //
  //
  //

  int ColorDistance (CIMAGE &image,
		     const double angle,
		     const int filterSize = 0);

  int ColorDistance2 (CIMAGE &image,
		      const double angle,
		      const int filterSize = 0);

#define SEGMENTOR_HULLPIXELS 1

  int GetNextRegion (CIMAGE &dist,
		     CREGION &region,
		     const int flags = 0);

  int GetNextRegionFromRoot (CIMAGE &dist,
			     CREGION &region,
			     const int xRoot,
			     const int yRoot,
			     const int threshold,
			     const int flags);

  /////////////////////////////////////////////////////////////////////////////
  //
  // SegmentColors()
  //
  // converts to YUV888 if necessary and goes into a loop: search the `in'
  // image for the color with the highest saturation, use the angle of this
  // color for a call to `ColorDistance()', take the first (best) region in the
  // distance image, fill this region with the initial color in the `out' image
  // and repeat this until no more saturated colors are found.
  //
  /////////////////////////////////////////////////////////////////////////////

  int SegmentColors (CIMAGE &in, CIMAGE &out);
};


#endif
