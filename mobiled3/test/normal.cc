//
// normal.cc
//
// normal distributed random numbers, stolen from libg++
//

#include <stdio.h>

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
#ifndef _RNG_h
#define _RNG_h 1

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

#endif


///////////////////////////////////////////////////////////////////////////////
//
// libg++/RNG.cc
//
///////////////////////////////////////////////////////////////////////////////

// This may look like C code, but it is really -*- C++ -*-
/* 
Copyright (C) 1989 Free Software Foundation

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

// These two static fields get initialized by RNG::RNG().
PrivateRNGSingleType RNG::singleMantissa;
PrivateRNGDoubleType RNG::doubleMantissa;

//
//	The scale constant is 2^-31. It is used to scale a 31 bit
//	long to a double.
//

//static const double randomDoubleScaleConstant = 4.656612873077392578125e-10;
//static const float  randomFloatScaleConstant = 4.656612873077392578125e-10;

static char initialized = 0;

RNG::RNG (void)
{
  if (!initialized)
    {
      assert (sizeof(double) == 2 * sizeof(_G_uint32_t)); 

      //
      //	The following is a hack that I attribute to
      //	Andres Nowatzyk at CMU. The intent of the loop
      //	is to form the smallest number 0 <= x < 1.0,
      //	which is then used as a mask for two longwords.
      //	this gives us a fast way way to produce double
      //	precision numbers from longwords.
      //
      //	I know that this works for IEEE and VAX floating
      //	point representations.
      //
      //	A further complication is that gnu C will blow
      //	the following loop, unless compiled with -ffloat-store,
      //	because it uses extended representations for some of
      //	of the comparisons. Thus, we have the following hack.
      //	If we could specify #pragma optimize, we wouldn't need this.
      //

      PrivateRNGDoubleType t;
      PrivateRNGSingleType s;

#if _IEEE == 1

      t.d = 1.5;

      if ( t.u[1] == 0 ) {		// sun word order?
	t.u[0] = 0x3fffffff;
	t.u[1] = 0xffffffff;
      }
      else
	{
	  t.u[0] = 0xffffffff;	// encore word order?
	  t.u[1] = 0x3fffffff;
	}

      s.u = 0x3fffffff;

#else

      volatile double x = 1.0; // volatile needed when fp hardware used,
                               // and has greater precision than memory doubles

      double y = 0.5;

      do
	{			    // find largest fp-number < 2.0
	  t.d = x;
	  x += y;
	  y *= 0.5;
	}
      while (x != t.d && x < 2.0);

      volatile float xx = 1.0; // volatile needed when fp hardware used,
                               // and has greater precision than memory floats

      float yy = 0.5;

      do
	{			    // find largest fp-number < 2.0
	  s.s = xx;
	  xx += yy;
	  yy *= 0.5;
	}
      while (xx != s.s && xx < 2.0);

#endif

      // set doubleMantissa to 1 for each doubleMantissa bit

      doubleMantissa.d = 1.0;
      doubleMantissa.u[0] ^= t.u[0];
      doubleMantissa.u[1] ^= t.u[1];

      // set singleMantissa to 1 for each singleMantissa bit

      singleMantissa.s = 1.0;
      singleMantissa.u ^= s.u;

      initialized = 1;
    }
}

float RNG::asFloat (void)
{
  PrivateRNGSingleType result;

  result.s = 1.0;
  result.u |= (asLong() & singleMantissa.u);
  result.s -= 1.0;

  assert( result.s < 1.0 && result.s >= 0);

  return( result.s );
}
	
double RNG::asDouble (void)
{
  PrivateRNGDoubleType result;

  result.d = 1.0;
  result.u[0] |= (asLong() & doubleMantissa.u[0]);
  result.u[1] |= (asLong() & doubleMantissa.u[1]);
  result.d -= 1.0;

  assert( result.d < 1.0 && result.d >= 0);

  return( result.d );
}


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
#ifndef _Random_h
#define _Random_h 1

#include <math.h>

class Random
{
protected:
  RNG *pGenerator;
public:
  Random(RNG *generator);
  virtual ~Random (void)
  {
  }
  virtual double operator()(void) = 0;

  RNG *generator (void);
  void generator(RNG *p);
};


Random::Random (RNG *gen)
{
  pGenerator = gen;
}

RNG *Random::generator (void)
{
  return pGenerator;
}

void Random::generator (RNG *p)
{
  pGenerator = p;
}

#endif


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
#ifndef _Normal_h
#define _Normal_h 1

class Normal: public Random
{
  char haveCachedNormal;
  double cachedNormal;

protected:

  double pMean;
  double pVariance;
  double pStdDev;

public:
  Normal(double xmean, double xvariance, RNG *gen);
  virtual ~Normal (void)
  {
  }

  double mean (void);
  double mean (const double x);
  double variance (void);
  double variance (const double x);
  virtual double operator()(void);
};


Normal::Normal (double xmean, double xvariance, RNG *gen)
  : Random(gen)
{
  pMean = xmean;
  pVariance = xvariance;
  pStdDev = sqrt(pVariance);
  haveCachedNormal = 0;
}

double Normal::mean (void)
{
  return pMean;
}

double Normal::mean (const double x)
{
  double t=pMean;
  pMean = x;
  return t;
}

double Normal::variance (void)
{
  return pVariance;
}

double Normal::variance (const double x)
{
  double t=pVariance;
  pVariance = x;
  pStdDev = sqrt(pVariance);
  return t;
};

#endif


///////////////////////////////////////////////////////////////////////////////
//
// libg++/Normal.cc
//
///////////////////////////////////////////////////////////////////////////////

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

//
//	See Simulation, Modelling & Analysis by Law & Kelton, pp259
//
//	This is the ``polar'' method.
// 

double Normal::operator() (void)
{
  if (haveCachedNormal == 1)
    {
      haveCachedNormal = 0;
      return(cachedNormal * pStdDev + pMean );
    }
  else
    {
      for(;;)
	{
	  double u1 = pGenerator -> asDouble();
	  double u2 = pGenerator -> asDouble();
	  double v1 = 2 * u1 - 1;
	  double v2 = 2 * u2 - 1;
	  double w = (v1 * v1) + (v2 * v2);

	  //
	  //	We actually generate two IID normal distribution variables.
	  //	We cache the one & return the other.
	  //

	  if (w <= 1)
	    {
	      double y = sqrt( (-2 * log(w)) / w);
	      double x1 = v1 * y;
	      double x2 = v2 * y;

	      haveCachedNormal = 1;
	      cachedNormal = x2;

	      return(x1 * pStdDev + pMean);
	    }
	}
    }
}


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
#ifndef _ACG_h
#define _ACG_h 1 

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

#endif


///////////////////////////////////////////////////////////////////////////////
//
// libg++/ACG.cc
//
///////////////////////////////////////////////////////////////////////////////

// This may look like C code, but it is really -*- C++ -*-
/* 
Copyright (C) 1989 Free Software Foundation

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

#ifdef __GNUG__
#pragma implementation
#endif
#include <assert.h>

//
//	This is an extension of the older implementation of Algorithm M
//	which I previously supplied. The main difference between this
//	version and the old code are:
//
//		+ Andres searched high & low for good constants for
//		  the LCG.
//
//		+ theres more bit chopping going on.
//
//	The following contains his comments.
//
//	agn@UNH.CS.CMU.EDU sez..
//	
//	The generator below is based on 2 well known
//	methods: Linear Congruential (LCGs) and Additive
//	Congruential generators (ACGs).
//	
//	The LCG produces the longest possible sequence
//	of 32 bit random numbers, each being unique in
//	that sequence (it has only 32 bits of state).
//	It suffers from 2 problems: a) Independence
//	isnt great, that is the (n+1)th number is
//	somewhat related to the preceding one, unlike
//	flipping a coin where knowing the past outcomes
//	dont help to predict the next result.  b)
//	Taking parts of a LCG generated number can be
//	quite non-random: for example, looking at only
//	the least significant byte gives a permuted
//	8-bit counter (that has a period length of only
//	256).  The advantage of an LCA is that it is
//	perfectly uniform when run for the entire period
//	length (and very uniform for smaller sequences
//	too, if the parameters are chosen carefully).
//	
//	ACGs have extremly long period lengths and
//	provide good independence.  Unfortunately,
//	uniformity isnt not too great. Furthermore, I
//	didnt find any theoretically analysis of ACGs
//	that addresses uniformity.
//	
//	The RNG given below will return numbers
//	generated by an LCA that are permuted under
//	control of a ACG. 2 permutations take place: the
//	4 bytes of one LCG generated number are
//	subjected to one of 16 permutations selected by
//	4 bits of the ACG. The permutation a such that
//	byte of the result may come from each byte of
//	the LCG number. This effectively destroys the
//	structure within a word. Finally, the sequence
//	of such numbers is permuted within a range of
//	256 numbers. This greatly improves independence.
//	
//
//  Algorithm M as describes in Knuths "Art of Computer Programming",
//	Vol 2. 1969
//  is used with a linear congruential generator (to get a good uniform
//  distribution) that is permuted with a Fibonacci additive congruential
//  generator to get good independence.
//
//  Bit, byte, and word distributions were extensively tested and pass
//  Chi-squared test near perfect scores (>7E8 numbers tested, Uniformity
//  assumption holds with probability > 0.999)
//
//  Run-up tests for on 7E8 numbers confirm independence with
//  probability > 0.97.
//
//  Plotting random points in 2d reveals no apparent structure.
//
//  Autocorrelation on sequences of 5E5 numbers (A(i) = SUM X(n)*X(n-i),
//	i=1..512)
//  results in no obvious structure (A(i) ~ const).
//
//  Except for speed and memory requirements, this generator outperforms
//  random() for all tests. (random() scored rather low on uniformity tests,
//  while independence test differences were less dramatic).
//
//  AGN would like to..
//  thanks to M.Mauldin, H.Walker, J.Saxe and M.Molloy for inspiration & help.
//
//  And I would (DGC) would like to thank Donald Kunth for AGN for letting me
//  use his extensions in this implementation.
//

//
//	Part of the table on page 28 of Knuth, vol II. This allows us
//	to adjust the size of the table at the expense of shorter sequences.
//

static int randomStateTable[][3] = {
{3,7,16}, {4,9, 32}, {3,10, 32}, {1,11, 32}, {1,15,64}, {3,17,128},
{7,18,128}, {3,20,128}, {2,21, 128}, {1,22, 128}, {5,23, 128}, {3,25, 128},
{2,29, 128}, {3,31, 128}, {13,33, 256}, {2,35, 256}, {11,36, 256},
{14,39,256}, {3,41,256}, {9,49,256}, {3,52,256}, {24,55,256}, {7,57, 256},
{19,58,256}, {38,89,512}, {17,95,512}, {6,97,512}, {11,98,512}, {-1,-1,-1} };

//
// spatial permutation table
//	RANDOM_PERM_SIZE must be a power of two
//

#define RANDOM_PERM_SIZE 64
_G_uint32_t randomPermutations[RANDOM_PERM_SIZE] = {
0xffffffff, 0x00000000,  0x00000000,  0x00000000,  // 3210
0x0000ffff, 0x00ff0000,  0x00000000,  0xff000000,  // 2310
0xff0000ff, 0x0000ff00,  0x00000000,  0x00ff0000,  // 3120
0x00ff00ff, 0x00000000,  0xff00ff00,  0x00000000,  // 1230

0xffff0000, 0x000000ff,  0x00000000,  0x0000ff00,  // 3201
0x00000000, 0x00ff00ff,  0x00000000,  0xff00ff00,  // 2301
0xff000000, 0x00000000,  0x000000ff,  0x00ffff00,  // 3102
0x00000000, 0x00000000,  0x00000000,  0xffffffff,  // 2103

0xff00ff00, 0x00000000,  0x00ff00ff,  0x00000000,  // 3012
0x0000ff00, 0x00000000,  0x00ff0000,  0xff0000ff,  // 2013
0x00000000, 0x00000000,  0xffffffff,  0x00000000,  // 1032
0x00000000, 0x0000ff00,  0xffff0000,  0x000000ff,  // 1023

0x00000000, 0xffffffff,  0x00000000,  0x00000000,  // 0321
0x00ffff00, 0xff000000,  0x00000000,  0x000000ff,  // 0213
0x00000000, 0xff000000,  0x0000ffff,  0x00ff0000,  // 0132
0x00000000, 0xff00ff00,  0x00000000,  0x00ff00ff   // 0123
};

//
//	SEED_TABLE_SIZE must be a power of 2
//
#define SEED_TABLE_SIZE 32
static _G_uint32_t seedTable[SEED_TABLE_SIZE] = {
0xbdcc47e5, 0x54aea45d, 0xec0df859, 0xda84637b,
0xc8c6cb4f, 0x35574b01, 0x28260b7d, 0x0d07fdbf,
0x9faaeeb0, 0x613dd169, 0x5ce2d818, 0x85b9e706,
0xab2469db, 0xda02b0dc, 0x45c60d6e, 0xffe49d10,
0x7224fea3, 0xf9684fc9, 0xfc7ee074, 0x326ce92a,
0x366d13b5, 0x17aaa731, 0xeb83a675, 0x7781cb32,
0x4ec7c92d, 0x7f187521, 0x2cf346b4, 0xad13310f,
0xb89cff2b, 0x12164de1, 0xa865168d, 0x32b56cdf
};

//
//	The LCG used to scramble the ACG
//
//
// LC-parameter selection follows recommendations in 
// "Handbook of Mathematical Functions" by Abramowitz & Stegun 10th, edi.
//
// LC_A = 251^2, ~= sqrt(2^32) = 66049
// LC_C = result of a long trial & error series = 3907864577
//

static const _G_uint32_t LC_A = 66049;
static const _G_uint32_t LC_C = 3907864577;
static inline _G_uint32_t LCG(_G_uint32_t x)
{
    return( x * LC_A + LC_C );
}


ACG::ACG(_G_uint32_t seed, int size)
{
    register int l;
    initialSeed = seed;
    
    //
    //	Determine the size of the state table
    //
    
    for (l = 0;
	 randomStateTable[l][0] != -1 && randomStateTable[l][1] < size;
	 l++);
    
    if (randomStateTable[l][1] == -1) {
	l--;
    }

    initialTableEntry = l;
    
    stateSize = randomStateTable[ initialTableEntry ][ 1 ];
    auxSize = randomStateTable[ initialTableEntry ][ 2 ];
    
    //
    //	Allocate the state table & the auxillary table in a single malloc
    //
    
    state = new _G_uint32_t[stateSize + auxSize];
    auxState = &state[stateSize];

    reset();
}

//
//	Initialize the state
//
void
ACG::reset()
{
    register _G_uint32_t u;

    if (initialSeed < SEED_TABLE_SIZE) {
	u = seedTable[ initialSeed ];
    } else {
	u = initialSeed ^ seedTable[ initialSeed & (SEED_TABLE_SIZE-1) ];
    }


    j = randomStateTable[ initialTableEntry ][ 0 ] - 1;
    k = randomStateTable[ initialTableEntry ][ 1 ] - 1;

    register int i;
    for(i = 0; i < stateSize; i++) {
	state[i] = u = LCG(u);
    }
    
    for (i = 0; i < auxSize; i++) {
	auxState[i] = u = LCG(u);
    }
    
    k = u % stateSize;
    int tailBehind = (stateSize - randomStateTable[ initialTableEntry ][ 0 ]);
    j = k - tailBehind;
    if (j < 0) {
	j += stateSize;
    }
    
    lcgRecurr = u;
    
    assert(sizeof(double) == 2 * sizeof(_G_int32_t));
}

ACG::~ACG()
{
    if (state) delete state;
    state = 0;
    // don't delete auxState, it's really an alias for state.
}

//
//	Returns 32 bits of random information.
//

_G_uint32_t
ACG::asLong()
{
    _G_uint32_t result = state[k] + state[j];
    state[k] = result;
    j = (j <= 0) ? (stateSize-1) : (j-1);
    k = (k <= 0) ? (stateSize-1) : (k-1);
    
    short int auxIndex = (result >> 24) & (auxSize - 1);
    register _G_uint32_t auxACG = auxState[auxIndex];
    auxState[auxIndex] = lcgRecurr = LCG(lcgRecurr);
    
    //
    // 3c is a magic number. We are doing four masks here, so we
    // do not want to run off the end of the permutation table.
    // This insures that we have always got four entries left.
    //
    register _G_uint32_t *perm = & randomPermutations[result & 0x3c];
    
    result =  *(perm++) & auxACG;
    result |= *(perm++) & ((auxACG << 24)
			   | ((auxACG >> 8)& 0xffffff));
    result |= *(perm++) & ((auxACG << 16)
			   | ((auxACG >> 16) & 0xffff));
    result |= *(perm++) & ((auxACG <<  8)
			   | ((auxACG >> 24) &   0xff));
    
    return(result);
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int main (void)
{
  ACG acg;
  Normal normal (0, 1, &acg);

  for (int i=0; i<10000; i++)
    {
      const double r1 = normal ();
      const double r2 = normal ();

      const double x = r1 + 0.5 * r2;
      const double y = r2 + 0.5 * r1;

      printf ("%f %f\n", x, y);
    }
}
