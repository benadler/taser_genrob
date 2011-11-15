//
// normal.h
// (C) 04/2002 by Torsten Scherer (TeSche)
// <itschere q@techfak.uni-bielefeld.de>
//
// normal distributed random number generator
// 99% stolen from older GNU libg++ code
//

#ifndef _NORMAL_H_
#define _NORMAL_H_


///////////////////////////////////////////////////////////////////////////////
//
// libg++/RNG.h
//
///////////////////////////////////////////////////////////////////////////////

// This may look like C code, but it is really -*- C++ -*-
/* 
Copyright (C) 1988 Free Software Foundation
    written by Dirk Grunwald (grunwald@cs.uiuc.edu)

This file is part of the GNU C++ Library.  This library is free
software; you can redistribute it and/or modify it under the terms of
the GNU Library General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.  This library is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <assert.h>
#include <math.h>
#include <_G_config.h>

union PrivateRNGSingleType
{		   	// used to access floats as unsigneds
  float s;
  _G_uint32_t u;
};

union PrivateRNGDoubleType
{		   	// used to access doubles as unsigneds
  double d;
  _G_uint32_t u[2];
};

//
// Base class for Random Number Generators. See ACG and MLCG for instances.
//

class RNG
{
  static PrivateRNGSingleType singleMantissa;	// mantissa bit vector
  static PrivateRNGDoubleType doubleMantissa;	// mantissa bit vector
public:
  RNG (void);
  //
  // Return a long-words word of random bits
  //
  virtual _G_uint32_t asLong (void) = 0;
  virtual void reset (void) = 0;
  //
  // Return random bits converted to either a float or a double
  //
  float asFloat (void);
  double asDouble (void);
};


///////////////////////////////////////////////////////////////////////////////
//
// libg++/ACG.h
//
///////////////////////////////////////////////////////////////////////////////

// This may look like C code, but it is really -*- C++ -*-
/* 
Copyright (C) 1988 Free Software Foundation
    written by Dirk Grunwald (grunwald@cs.uiuc.edu)

This file is part of the GNU C++ Library.  This library is free
software; you can redistribute it and/or modify it under the terms of
the GNU Library General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.  This library is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <math.h>

//
//	Additive number generator. This method is presented in Volume II
//	of The Art of Computer Programming by Knuth. I've coded the algorithm
//	and have added the extensions by Andres Nowatzyk of CMU to randomize
//	the result of algorithm M a bit	by using an LCG & a spatial
//	permutation table.
//
//	The version presented uses the same constants for the LCG that Andres
//	uses (chosen by trial & error). The spatial permutation table is
//	the same size (it's based on word size). This is for 32-bit words.
//
//	The ``auxillary table'' used by the LCG table varies in size, and
//	is chosen to be the the smallest power of two which is larger than
//	twice the size of the state table.
//

class ACG : public RNG {

    _G_uint32_t initialSeed;	// used to reset generator
    int initialTableEntry;

    _G_uint32_t *state;
    _G_uint32_t *auxState;
    short stateSize;
    short auxSize;
    _G_uint32_t lcgRecurr;
    short j;
    short k;

protected:

public:
    ACG(_G_uint32_t seed = 0, int size = 55);
    virtual ~ACG();
    //
    // Return a long-words word of random bits
    //
    virtual _G_uint32_t asLong();
    virtual void reset();
};


///////////////////////////////////////////////////////////////////////////////
//
// libg++/Random.h
//
///////////////////////////////////////////////////////////////////////////////

// This may look like C code, but it is really -*- C++ -*-
/* 
Copyright (C) 1988 Free Software Foundation
    written by Dirk Grunwald (grunwald@cs.uiuc.edu)

This file is part of the GNU C++ Library.  This library is free
software; you can redistribute it and/or modify it under the terms of
the GNU Library General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.  This library is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <math.h>

class Random
{
protected:
  RNG *pGenerator;
public:

  Random (RNG *generator)
    {
      pGenerator = generator;
    }

  virtual ~Random (void)
    {
    }

  virtual double operator() (void) = 0;

  RNG *generator (void) const
    {
      return pGenerator;
    }

  void generator (RNG *p)
    {
      pGenerator = p;
    }
};


///////////////////////////////////////////////////////////////////////////////
//
// libg++/Normal.h
//
///////////////////////////////////////////////////////////////////////////////

// This may look like C code, but it is really -*- C++ -*-
/* 
Copyright (C) 1988 Free Software Foundation
    written by Dirk Grunwald (grunwald@cs.uiuc.edu)

This file is part of the GNU C++ Library.  This library is free
software; you can redistribute it and/or modify it under the terms of
the GNU Library General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.  This library is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

class Normal: public Random
{
  char haveCachedNormal;
  double cachedNormal;

private:

  ACG acg;

  double pMean;
  double pVariance;
  double pStdDev;

public:

  Normal (const double xmean, const double xvariance)
    : Random (&acg)
    {
      pMean = xmean;
      pVariance = xvariance;
      pStdDev = sqrt (pVariance);
      haveCachedNormal = 0;
    }

  virtual ~Normal (void)
    {
    }

#if 0

  double mean (void) const
    {
      return pMean;
    }

  double mean (const double x)
    {
      double t = pMean;
      pMean = x;
      return t;
    }

  double variance (void) const
    {
      return pVariance;
    }

  double variance (const double x)
    {
      double t = pVariance;
      pVariance = x;
      pStdDev = sqrt (pVariance);
      return t;
    }

#endif

  virtual double operator() (void);
};


#endif
