//
// mobileMapd.cc
//

#include <math.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

extern "C"
{
#include <jpeglib.h>
  //#include <png.h>
}

#include "image/image.h"
#include "labdev/labdevd.h"
#include "mobile/genbase.h"

#include "thread/tracelog.h"


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

//
// jpg helper functions
//

static void jpegCompressInitDestination (j_compress_ptr cinfo)
{
}


static boolean jpegCompressEmptyOutputBuffer (j_compress_ptr cinfo)
{
  return TRUE;
}


static void jpegCompressTermDestination (j_compress_ptr cinfo)
{
}


static void jpegCompress (void **dstData,
			  int *dstSize,
			  const CIMAGE &image)
{
  // create compressor

  struct jpeg_compress_struct cinfo;

  struct jpeg_error_mgr jerr;

  cinfo.err = jpeg_std_error (&jerr);

  jpeg_create_compress (&cinfo);

  // set destination manager
  //
  // assumption: sizeof(dstImg) <= sizeof(srcImg)
  //

  struct jpeg_destination_mgr dst;

  int srcSize = 3 * image.GetWidth () * image.GetHeight ();
  *dstData = malloc (srcSize);

  dst.next_output_byte = (JOCTET *)*dstData;
  dst.free_in_buffer = srcSize;

  dst.init_destination = jpegCompressInitDestination;
  dst.empty_output_buffer = jpegCompressEmptyOutputBuffer;
  dst.term_destination = jpegCompressTermDestination;

  cinfo.dest = &dst;

  // set compression parameters

  cinfo.image_width = image.GetWidth ();
  cinfo.image_height = image.GetHeight ();
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_RGB;

  jpeg_set_defaults (&cinfo);

  jpeg_set_quality (&cinfo, 80, TRUE);
  cinfo.smoothing_factor = 20;

  //cinfo.dct_method = JDCT_ISLOW;
  cinfo.dct_method = JDCT_IFAST;
  //cinfo.dct_method = JDCT_FLOAT;

  //

  jpeg_start_compress (&cinfo, TRUE);

  const unsigned char *data = image.GetData ();
  int dataInc = 3 * image.GetWidth ();

  while (cinfo.next_scanline < cinfo.image_height)
    {
      JSAMPROW row[1] = {(JSAMPLE *)data};

      jpeg_write_scanlines (&cinfo, row, 1);

      data += dataInc;
    }

  jpeg_finish_compress (&cinfo);

  //

  *dstSize = srcSize - dst.free_in_buffer;

  // destroy compressor

  jpeg_destroy_compress (&cinfo);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#if 0

static void user_error_fn (png_structp png_ptr,
			   png_const_charp error_msg)
{
}

static void user_warning_fn (png_structp png_ptr,
			     png_const_charp warning_msg)
{
}


static void pngCompress (void **dstData,
			 int *dstSize,
			 const CIMAGE &image)
{
  //

  png_structp png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING,
						 //(png_voidp)user_error_ptr,
						 (png_voidp)0x0,
						 user_error_fn,
						 user_warning_fn);

  if (!png_ptr)
    {
      return;
    }

  png_infop info_ptr = png_create_info_struct (png_ptr);

  if (!info_ptr)
    {
      png_destroy_write_struct (&png_ptr,
				(png_infopp)NULL);
      return;
    }

  //

  if (setjmp(png_ptr->jmpbuf))
    {
      png_destroy_write_struct (&png_ptr, &info_ptr);
      return;
    }

  //

  FILE *fp = fopen ("map.png", "w");

  png_init_io (png_ptr, fp);

  //

  png_set_filter (png_ptr,
		  0,
		  PNG_FILTER_NONE | PNG_FILTER_SUB | PNG_FILTER_PAETH);

  //

  png_set_compression_level (png_ptr, Z_BEST_COMPRESSION);

  // fill struct

  png_set_IHDR (png_ptr,
		info_ptr,
		image.GetWidth (),
		image.GetHeight (),
		8,
		PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT);

  png_color color[16];

  png_set_PLTE (png_ptr,
		info_ptr,
		color,
		16);

  png_set_sRGB_gAMA_and_cHRM (png_ptr,
			      info_ptr,
			      PNG_SRGB_INTENT_ABSOLUTE);

  // png_set_sBIT (png_ptr, info_ptr, 8);

#error "das ist doch alles scheisse!"  

  //
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

class CMAPD : public CLABDEVD
{
protected:

  virtual int Allow (unsigned int addr);
  virtual void Event (CLABMSG &msg);

public:

  CMAPD (void);
  virtual ~CMAPD (void);

  void GetMapImage (CLABMSG &msg);
};


//
//
//

CMAPD::CMAPD (void) : CLABDEVD ("mapd")
{
  GTL ("CMAPD::CMAPD()");
}


//
//
//

CMAPD::~CMAPD (void)
{
  GTL ("CMAPD::~CMAPD()");
}


//
//
//

int CMAPD::Allow (unsigned int addr)
{
  GTL ("CMAPD::Allow()");

  if ((addr & 0xffffff00) == 0x81468c00)   // 129.70.140.*
    {
      GTLPRINT (("access granted\n"));
      return 1;
    }

  GTLFATAL (("access denied\n"));

  return 0;
}


//
//
//

void CMAPD::Event (CLABMSG &msg)
{
  GTL ("CMAPD::Event()");

  int cmd = msg._cmd;

  msg._cmd = -1;

  switch (cmd)
    {
    case CMD_GET_MAP_IMAGE:
      GetMapImage (msg);
      break;

    default:   // return to sender
	  break;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static inline int isqr (const int arg)
{
  return arg * arg;
}


void CMAPD::GetMapImage (CLABMSG &msg)
{
  GTL ("CMAPD::GetMapImage()");

  int width = msg._intData[0];
  int height = msg._intData[1];

  if ((width < 0) ||
      (width > 1024) ||
      (height < 0) ||
      (height > 768))
    {
      GTLFATAL (("illegal map image size\n"));
      return;
    }

  //

  CGENBASE *genBase;

  try
    {
#if 1
      genBase = new CGENBASE ("localhost");
#else
      genBase = new CGENBASE ("bunsen");
#endif
    }

#if 0

  catch (const char *msg)
    {
      GTLFATAL (("failed to connect to genbase (%s)\n", msg));
      return;
    }

#endif

  catch (...)
    {
      GTLFATAL (("failed to connect to genbase\n"));
      return;
    }

  //

  static CIMAGE image (TYPE_RGB888, width, height);

  image.SetColor (0, 0, 0);

  image.Pbox (0, 0, width-1, height-1);

  // draw map

  CGENBASE::MAP *map = genBase->GetMap ();
  float *fPtr = map->line;
  int xm = width / 4;
  int ym = 2 * height / 5;

  image.SetColor (128, 128, 128);

  for (int i=0; i<map->numLines; i++)
    {
      image.Line ((int)(xm + fPtr[1] * 42.0),
		  (int)(ym + fPtr[0] * 42.0),
		  (int)(xm + fPtr[3] * 42.0),
		  (int)(ym + fPtr[2] * 42.0));

      fPtr += 4;
    }

  // draw expected markers

  fPtr = map->mark;

  image.SetColor (0, 0, 255);

  for (int i=0; i<map->numMarks; i++)
    {
      int xMark = (int)(xm + fPtr[1] * 42.0);
      int yMark = (int)(ym + fPtr[0] * 42.0);

      image.Pcircle (xMark, yMark, 2);

      fPtr += 2;
    }

  // draw mobile

  double x0, y0, a0;

  genBase->GetFrame (x0, y0, a0);

  image.SetColor (255, 255, 0);

  int xMobile = (int)(xm + y0 * 42.0);
  int yMobile = (int)(ym + x0 * 42.0);

  image.Pcircle (xMobile, yMobile, (int)(0.4 * 42.0));

  int xHead = (int)(xMobile + 0.3 * 42.0 * sin (M_PI * a0 / 180.0));
  int yHead = (int)(yMobile + 0.3 * 42.0 * cos (M_PI * a0 / 180.0));

  image.SetColor (0, 0, 0);

  image.Pcircle (xHead, yHead, 3);

  // draw laser scans

  CGENBASE::CSCAN scan1, scan2;

  genBase->Scan (scan1, CGENBASE::FRONT);
  genBase->Scan (scan2, CGENBASE::REAR);

  image.SetColor (255, 0, 0);

#if 0

  for (int i=0; i<362; i++)
    {
      image.Plot (xm + scan1.sy[i] * 42.0,
		  ym + scan1.sx[i] * 42.0);

      image.Plot (xm + scan2.sy[i] * 42.0,
		  ym + scan2.sx[i] * 42.0);

    }

#else

  int xOld = (int)(xm + scan1.sy[0] * 42.0);
  int yOld = (int)(ym + scan1.sx[0] * 42.0);

  for (int i=1; i<362; i++)
    {
      int x = (int)(xm + scan1.sy[i] * 42.0);
      int y = (int)(ym + scan1.sx[i] * 42.0);

      if ((isqr (xOld - x) + isqr (yOld - y)) < 32)
	{
	  image.Line (xOld, yOld, x, y);
	}

      xOld = x;
      yOld = y;
    }

  //

  xOld = (int)(xm + scan2.sy[0] * 42.0);
  yOld = (int)(ym + scan2.sx[0] * 42.0);

  for (int i=1; i<362; i++)
    {
      int x = (int)(xm + scan2.sy[i] * 42.0);
      int y = (int)(ym + scan2.sx[i] * 42.0);

      if ((isqr (xOld - x) + isqr (yOld - y)) < 32)
	{
	  image.Line (xOld, yOld, x, y);
	}

      xOld = x;
      yOld = y;
    }

#endif

  // draw seen markers

  image.SetColor (0, 255, 0);

  for (int i=0; i<scan1.numMarkers; i++)
    {
      int xMark = (int)(xm + scan1.my[i] * 42.0);
      int yMark = (int)(ym + scan1.mx[i] * 42.0);

      image.Line (xMark-2, yMark-2, xMark+2, yMark+2);
      image.Line (xMark-2, yMark+2, xMark+2, yMark-2);
    }

  for (int i=0; i<scan2.numMarkers; i++)
    {
      int xMark = (int)(xm + scan2.my[i] * 42.0);
      int yMark = (int)(ym + scan2.mx[i] * 42.0);

      image.Line (xMark-2, yMark-2, xMark+2, yMark+2);
      image.Line (xMark-2, yMark+2, xMark+2, yMark-2);
    }

  //

#if 0
  {
    static int cnt = 1;
    char fName[64];
    sprintf (fName, "map%i.ppm", cnt++);
    image.SavePPM (fName);
  }
#endif

#if 0
  image.Show ();
#endif

#if 0
  CIMAGE r, g, b;

  image.Split (r, g, b);

  r.Smooth (2);
  g.Smooth (2);
  b.Smooth (2);

  image.Combine (TYPE_RGB888, r, g, b);
#endif

  //

  //pngCompress (&msg._auxData, &msg._auxDataSize, image);
  jpegCompress (&msg._auxData, &msg._auxDataSize, image);

  msg._cmd = 0;

  //

  delete genBase;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////


int main (void)
{
  //

  nice (30);

  signal (SIGPIPE, SIG_IGN);

  //

  CMAPD *mapd;

  try
    {
      mapd = new CMAPD;
    }

  catch (...)
    {
      fprintf (stderr, "failed to register in infobase\n");
      return -1;
    }

  //

  while (42)
    {
      mapd->Run (1000);
    }

  return 0;
}
