/*
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <GL/glut.h>

#include "image.h"


static CIMAGE globImage;


#if 0

static inline void DrawColoredCube (const float x,
				    const float y,
				    const float z)
{
  glColor3f (x, y, z);

  glBegin (GL_QUADS);

  glVertex3f (x, y, z);   /* Unten */
  glVertex3f (x+size, y, z);
  glVertex3f (x+size, y, z+size);
  glVertex3f (x, y, z+size);

  glVertex3f (x, y+size, z);  /* Oben */
  glVertex3f (x, y+size, z+size);
  glVertex3f (x+size, y+size, z+size);
  glVertex3f (x+size, y+size, z);

  glEnd ();
  
  glBegin (GL_QUAD_STRIP);

  glVertex3f (x, y, z+size);   /* Vorne */
  glVertex3f (x, y+size, z+size);
  glVertex3f (x+size, y, z+size);
  glVertex3f (x+size, y+size, z+size);

  glVertex3f (x+size, y, z);   /* Rechts */
  glVertex3f (x+size, y+size, z);

  glVertex3f (x, y, z);   /* Hinten */
  glVertex3f (x, y+size, z);

  glVertex3f (x, y, z+size);   /* Links */
  glVertex3f (x, y+size, z+size);

  glEnd ();
}

#endif


//
//
//

GLenum      polygonMode=GL_LINE;
float       rotX=0.0, rotY=0.0;


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static inline void yuv2rgb (const unsigned char *in, int *out)
{
  int y = *in++ - 16;
  int u = *in++ - 128;
  int v = *in   - 128;

  //
  // floating point:
  //
  // r = 1.164383574 * y + 0.000000000 * u + 1.596026777 * v;
  // g = 1.164383574 * y - 0.391762275 * u - 0.812967628 * v;
  // b = 1.164383574 * y + 2.017232129 * u + 0.000000000 * v;
  //
  // integer with 20 bits
  //

  int r = (1220944 * y               + 1673555 * v) >> 20;
  int g = (1220944 * y -  410792 * u -  852458 * v) >> 20;
  int b = (1220944 * y + 2115221 * u              ) >> 20;

  // clip and write values

#if 1
  *out++ = r;
  *out++ = g;
  *out   = b;
#else
  *out++ = (r < 0) ? 0 : (r > 255) ? 255 : r;
  *out++ = (g < 0) ? 0 : (g > 255) ? 255 : g;
  *out   = (b < 0) ? 0 : (b > 255) ? 255 : b;
#endif
}


static inline void rgb2yuv (const unsigned char *in, unsigned char *out)
{
  int r = *in++;
  int g = *in++;
  int b = *in;

  //
  // floating point:
  //
  // y =  16.0 + 0.25678823 * r + 0.50412941 * g + 0.09790588 * b;
  // u = 128.0 - 0.14822290 * r - 0.29099279 * g + 0.43921569 * b;
  // v = 128.0 + 0.43921569 * r - 0.36778832 * g - 0.07142737 * b;
  //
  // integer with 16 bits (would be a bit faster):
  //
  // int y =  16 + ((16828 * r + 33038 * g +  6416 * b) >> 16);
  // int u = 128 + ((-9713 * r - 19070 * g + 28784 * b) >> 16);
  // int v = 128 + ((28784 * r - 24103 * g -  4681 * b) >> 16);
  //
  // integer with 20 bits:
  //

  int y =  16 + (( 269261 * r + 528617 * g + 102661 * b) >> 20);
  int u = 128 + ((-155422 * r - 305128 * g + 460550 * b) >> 20);
  int v = 128 + (( 460550 * r - 385653 * g -  74897 * b) >> 20);

  // clip and write values

  *out++ = (y < 16) ? 16 : (y > 235) ? 235 : y;
  *out++ = (u < 16) ? 16 : (u > 240) ? 240 : u;
  *out   = (v < 16) ? 16 : (v > 240) ? 240 : v;
}


static void drawYuvLine (const int y0, const int u0, const int v0,
			 const int y1, const int u1, const int v1)
{
#if 1

  unsigned char yuv[3];
  int rgb[3];

  glBegin (GL_POINTS);

  for (int i=0; i<=100; i+=2)
    {
      yuv[0] = (i*y0+(100-i)*y1) / 100;
      yuv[1] = (i*u0+(100-i)*u1) / 100;
      yuv[2] = (i*v0+(100-i)*v1) / 100;

      yuv2rgb (yuv, rgb);

      float r = rgb[0] / 255.0;
      float g = rgb[1] / 255.0;
      float b = rgb[2] / 255.0;

      glColor3f (r, g, b);
      glVertex3f (r, g, b);
    }

  glEnd ();

#else

  unsigned char yuv[3] = {y0, u0, v0};
  int rgb[3];

  glBegin (GL_LINES);

  yuv2rgb (yuv, rgb);

  float r = rgb[0] / 255.0;
  float g = rgb[1] / 255.0;
  float b = rgb[2] / 255.0;

  glColor3f (r, g, b);
  glVertex3f (r, g, b);

  yuv[0] = y1;
  yuv[1] = u1;
  yuv[2] = v1;

  yuv2rgb (yuv, rgb);

  r = rgb[0] / 255.0;
  g = rgb[1] / 255.0;
  b = rgb[2] / 255.0;

  glColor3f (r, g, b);
  glVertex3f (r, g, b);

  glEnd ();

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

static void DrawRGBScene (void)
{
  /* Wuerfel aus 2 Quadraten und einen "Quadratstreifen" zusammensetzen */

#if 1
  glBegin (GL_QUADS);

  glColor3f (0.0, 0.0, 0.0); glVertex3f (0.0, 0.0, 0.0);   /* Unten */
  glColor3f (1.0, 0.0, 0.0); glVertex3f (1.0, 0.0, 0.0);
  glColor3f (1.0, 0.0, 1.0); glVertex3f (1.0, 0.0, 1.0);
  glColor3f (0.0, 0.0, 1.0); glVertex3f (0.0, 0.0, 1.0);

  glColor3f (0.0, 1.0, 0.0); glVertex3f (0.0, 1.0, 0.0);  /* Oben */
  glColor3f (0.0, 1.0, 1.0); glVertex3f (0.0, 1.0, 1.0);
  glColor3f (1.0, 1.0, 1.0); glVertex3f (1.0, 1.0, 1.0);
  glColor3f (1.0, 1.0, 0.0); glVertex3f (1.0, 1.0, 0.0);

  glEnd ();
  
  glBegin (GL_QUAD_STRIP);

  glColor3f (0.0, 0.0, 1.0); glVertex3f (0.0, 0.0, 1.0);   /* Vorne */
  glColor3f (0.0, 1.0, 1.0); glVertex3f (0.0, 1.0, 1.0);
  glColor3f (1.0, 0.0, 1.0); glVertex3f (1.0, 0.0, 1.0);
  glColor3f (1.0, 1.0, 1.0); glVertex3f (1.0, 1.0, 1.0);

  glColor3f (1.0, 0.0, 0.0); glVertex3f (1.0, 0.0, 0.0);   /* Rechts */
  glColor3f (1.0, 1.0, 0.0); glVertex3f (1.0, 1.0, 0.0);

  glColor3f (0.0, 0.0, 0.0); glVertex3f (0.0, 0.0, 0.0);   /* Hinten */
  glColor3f (0.0, 1.0, 0.0); glVertex3f (0.0, 1.0, 0.0);

  glColor3f (0.0, 0.0, 1.0); glVertex3f (0.0, 0.0, 1.0);   /* Links */
  glColor3f (0.0, 1.0, 1.0); glVertex3f (0.0, 1.0, 1.0);

  glEnd ();
#endif

  static int first = 1;

  if (first)
    {
      first = 0;

      glNewList (1, GL_COMPILE);

      glBegin (GL_POINTS);

      int width = globImage.GetWidth ();
      int height = globImage.GetHeight ();
      int size = width * height;
      unsigned char *data = (unsigned char *)globImage.GetData ();

      for (int y=0; y<height; y++)
	{
	  for (int x=0; x<width; x++)
	    {
	      // if ((x+y)%2)
		{
		  float r = data[(y*width+x)*3+0] / 255.0;
		  float g = data[(y*width+x)*3+1] / 255.0;
		  float b = data[(y*width+x)*3+2] / 255.0;

		  glColor3f (r, g, b);
		  glVertex3f (r, g, b);
		}
	    }
	}

      glEnd ();

      glEndList ();

      // the YUV cube inside (outside) the RGB cube

      glNewList (2, GL_COMPILE);

      // the UV plane at Y=min
      drawYuvLine (16, 16, 16, 16, 240, 16);
      drawYuvLine (16, 240, 16, 16, 240, 240);
      drawYuvLine (16, 240, 240, 16, 16, 240);
      drawYuvLine (16, 16, 240, 16, 16, 16);

      // the UV plane at Y=max
      drawYuvLine (235, 16, 16, 235, 240, 16);
      drawYuvLine (235, 240, 16, 235, 240, 240);
      drawYuvLine (235, 240, 240, 235, 16, 240);
      drawYuvLine (235, 16, 240, 235, 16, 16);

      //
      drawYuvLine (16, 16, 16, 235, 16, 16);
      drawYuvLine (16, 16, 240, 235, 16, 240);
      drawYuvLine (16, 240, 16, 235, 240, 16);
      drawYuvLine (16, 240, 240, 235, 240, 240);

      glEndList ();
    }

  glCallList (1);
  glCallList (2);

  // 

  glBegin (GL_QUADS);

  static float x0 = 0.0, y0 = 0.0, z0 = 0.0, size = 0.1;

  glColor3f (1, 1, 1);

  glVertex3f (x0, y0, z0);
  glVertex3f (x0, y0+size, z0);
  glVertex3f (x0+size, y0+size, z0);
  glVertex3f (x0+size, y0, z0);

  glVertex3f (x0, y0, z0+size);
  glVertex3f (x0, y0+size, z0+size);
  glVertex3f (x0+size, y0+size, z0+size);
  glVertex3f (x0+size, y0, z0+size);

  glEnd ();

  globImage.Show ();
}


///////////////////////////////////////////////////////////////////////////////

static void glYuvPoint (int y0, int u0, int v0)
{
  unsigned char yuv[3];
  int rgb[3];

  yuv[0] = y0;
  yuv[1] = u0;
  yuv[2] = v0;

  yuv2rgb (yuv, rgb);

  glBegin (GL_POINTS);

  glColor3f (rgb[0] / 255.0, rgb[1] / 255.0, rgb[2] / 255.0);

  glVertex3f ((y0 - 16) / 219.0,
	      (u0 - 16) / 224.0,
	      (v0 - 16) / 224.0);

  glEnd ();
}


static void glYuvLine (int y0, int u0, int v0,
		       int y1, int u1, int v1)
{
  unsigned char yuv[3];
  int rgb[3];

  yuv[0] = y0;
  yuv[1] = u0;
  yuv[2] = v0;

  yuv2rgb (yuv, rgb);

  glBegin (GL_LINES);

  glColor3f (rgb[0] / 255.0, rgb[1] / 255.0, rgb[2] / 255.0);

  glVertex3f ((y0 - 16) / 219.0,
	      (u0 - 16) / 224.0,
	      (v0 - 16) / 224.0);

  yuv[0] = y1;
  yuv[1] = u1;
  yuv[2] = v1;

  yuv2rgb (yuv, rgb);

  glColor3f (rgb[0] / 255.0, rgb[1] / 255.0, rgb[2] / 255.0);

  glVertex3f ((y1 - 16) / 219.0,
	      (u1 - 16) / 224.0,
	      (v1 - 16) / 224.0);

  glEnd ();
}


static void DrawYUVScene (void)
{
  static int first = 1;

  if (first)
    {
      first = 0;

      glNewList (1, GL_COMPILE);

      int width = globImage.GetWidth ();
      int height = globImage.GetHeight ();
      int size = width * height;
      unsigned char *data = (unsigned char *)globImage.GetData ();

      for (int y=0; y<height; y++)
	{
	  for (int x=0; x<width; x++)
	    {
	      // if ((x+y)%2)
		{
		  unsigned char rgb[3];
		  unsigned char yuv[3];

		  rgb[0] = data[(y*width+x)*3+0];
		  rgb[1] = data[(y*width+x)*3+1];
		  rgb[2] = data[(y*width+x)*3+2];

		  rgb2yuv (rgb, yuv);

		  glColor3f (rgb[0], rgb[1], rgb[2]);
		  glYuvPoint (yuv[0], yuv[1], yuv[2]);
		}
	    }
	}

      glEndList ();

      // the YUV cube /////////////////////////////////////////////////////////

      glNewList (2, GL_COMPILE);

      glBegin (GL_LINES);

      // the UV plane at Y=min
      glYuvLine (16, 16, 16, 16, 16, 240);
      glYuvLine (16, 16, 240, 16, 240, 240);
      glYuvLine (16, 240, 240, 16, 240, 16);
      glYuvLine (16, 240, 16, 16, 16, 16);

      // the UV plane at Y=max
      glYuvLine (235, 16, 16, 235, 240, 16);
      glYuvLine (235, 240, 16, 235, 240, 240);
      glYuvLine (235, 240, 240, 235, 16, 240);
      glYuvLine (235, 16, 240, 235, 16, 16);

      //
      glYuvLine (16, 16, 16, 235, 16, 16);
      glYuvLine (16, 16, 240, 235, 16, 240);
      glYuvLine (16, 240, 16, 235, 240, 16);
      glYuvLine (16, 240, 240, 235, 240, 240);

      glEnd ();

      glEndList ();
    }

  glCallList (1);
  glCallList (2);

  //

  globImage.Show ();
}


static void Init (void)
{
  /* Z-Buffer f. Berechnung verdeckter Flaechen einschalten */
  glEnable(GL_DEPTH_TEST);
    
  /* Vorder- u. Rueckseite der Polygone nur als Randlinien darstellen */
  glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
}


static void Reshape (int width, int height)
{
  /* Darstellung auf gesamten Clientbereich des Fensters zulassen */
  glViewport (0, 0, (GLint)width, (GLint)height);

  /* Projektionsmatix initialisieren auf 60 Grad horizontales */
  /* Sichtfeld, Verhaeltnis Breite:Hoehe = 1:1, Clipping fuer z<1 */
  /* und z>200 */
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();

  /* angle, aspect, near Clip, far Clip */
  gluPerspective (30.0, 1.0, 1.0, 10.0);

  /* Modelview Matrix wieder zur aktuellen Matrix machen */
  glMatrixMode (GL_MODELVIEW);
}


static void Key (unsigned char key, int x, int y)
{
  if (key==27) 
    exit(1);
  if (key==' ') {
    if (polygonMode==GL_FILL)
      polygonMode=GL_LINE;
    else
      polygonMode=GL_FILL;

    /* Polygondarstellung zw. Outline und Gefuellt umschalten... */
    glPolygonMode (GL_FRONT_AND_BACK, polygonMode);

    /* ... und Bild erneut rendern */
    glutPostRedisplay ();
  }
}


static void SpecialKey (int key, int x, int y)
{
  switch (key) {
  case GLUT_KEY_UP:
    rotX -= 5;
    break;
  case GLUT_KEY_DOWN:
    rotX += 5;
    break;
  case GLUT_KEY_LEFT:
    rotY -= 5;
    break;
  case GLUT_KEY_RIGHT:
    rotY += 5;
    break;
  default:
    return;
  }
  glutPostRedisplay();
}


static void Ausgabe (void)
{
  /* FrameBuffer und Z-Buffer loeschen */
  glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  /* Modelview Matrix initialisieren */
  glLoadIdentity ();
  
  /* Modeltransformation ausfuehren... */
  glTranslatef (0.0, 0.0 ,-3.0);    /* Verschiebung um -3 in z-Richtung */
  glRotatef (rotY, 0.0, 1.0, 0.0);  /* Rotation um die Y-Achse */
  glRotatef (rotX, 1.0, 0.0, 0.0);  /* Rotation um die X-Achse */
  glTranslatef (-0.5, -0.5 , -0.5); /* Verschiebung um -0.5 in alle Richt. */
  
  /* ...und den Wuerfel unter der Modeltransformation zeichnen. */
#if 0
  DrawYUVScene ();
#else
  DrawRGBScene ();
#endif

  /* Back-Buffer in den Front-Buffer kopieren. */
  glutSwapBuffers ();
}


int main (int argc, char **argv)
{
  glutInit (&argc, argv);

  if (argc == 2)
    {
      if (globImage.LoadPPM (argv[1]) < 0)
	{
	  return -1;
	}
    }
  else
    {
      globImage.LoadPPM ("../test.ppm");
    }

  /* Ausgabefenster definieren */
  glutInitWindowPosition (0, 0);
  glutInitWindowSize (300, 300);

  /* Renderkontext mit Z-Buffer, Doublebuffer fuer RGB-Modus anfordern. */
  glutInitDisplayMode (GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE);

  if (glutCreateWindow ("Sample1") == GL_FALSE)
    {
      exit (1);
    }

  Init ();

  /* Callback Funktionen vereinbaren */
  glutReshapeFunc (Reshape);
  glutKeyboardFunc (Key);
  glutSpecialFunc (SpecialKey);
  glutDisplayFunc (Ausgabe);

  /* Kontrolle an GLUT Eventloop uebergeben */
  glutMainLoop ();

  return 0;
}
