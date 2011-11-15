//
// multi dimensional loop
//
// `n' nested loops, with `n' being variable at run time
//

  int index[maxDim];
  int indexBest[maxDim];

  index[0] = -1;

  for (int i=1; i<maxDim; i++)
    {
      index[i] = 0;
    }

  while (42)
    {
      // increment

      int digit = 0;

      while (++index[digit] >= _maxDim)
	{
	  index[digit] = 0;

	  if (++digit == maxDim)
	    {
	      break;
	    }
	}

      if (digit == maxDim)
	{
	  break;
	}

      // print

      //for (int i=0; i<maxDim; i++) printf ("%i ", index[i]); printf ("\n");

      // compare

      bool legal = true;

      for (int i=0; i<maxDim-1; i++ && legal)
	{
	  for (int j=i+1; j<maxDim; j++ && legal)
	    {
	      if ((i != j) && (index[i] == index[j]))
		{
		  legal = false;
		}
	    }
	}

      if (!legal)
	{
	  continue;
	}

      // check this combination

#if 0

      // minimal distance between centers of gravity of two classes

      double cg[_numObjects][maxDim];

      for (int o1=0; o1<_numObjects; o1++)
	{
	  for (int i=0; i<maxDim; i++)
	    {
	      cg[o1][i] = 0.0;
	    }

#warning "this could be further optimized"

	  for (int i1=0; i1<_object[o1]->_numImages; i1++)
	    {
	      Vector *bild = _object[o1]->_image[i1];

	      for (int i=0; i<maxDim; i++)
		{
		  cg[o1][i] += (*bild)[index[i]];
		}
	    }

	  for (int i=0; i<maxDim; i++)
	    {
	      cg[o1][i] /= _object[o1]->_numImages;
	    }
	}

      double distMin = 1e37;
      static double globDistMax = 0.0;

      for (int o1=0; o1<_numObjects; o1++)
	{
	  for (int o2=0; o2<_numObjects; o2++)
	    {
	      if (o1 != o2)
		{
		  double dist = 0.0;

		  for (int i=0; i<maxDim; i++)
		    {
		      dist += sqr (cg[o1][i] - cg[o2][i]);
		    }

		  dist = sqrt (dist);

		  if (dist < distMin)
		    {
		      distMin = dist;
		    }
		}
	    }
	}

      if (distMin > globDistMax)
	{
	  globDistMax = distMin;

	  indexBest = index;

	  for (int i=0; i<maxDim; i++) printf ("%i ", index[i]);
	  ::printf ("-> %f\n", globDistMax);
	}

#else

      // minimal distance between two points of two classes

      static double globDistMax = 0.0;

      double distMin = 1e37;

      for (int o1=0; o1<_numObjects; o1++)
	{
	  for (int i1=0; i1<_object[o1]->_numImages; i1++)
	    {
	      Vector *bild1 = (Vector *)_object[o1]->_image[i1];

	      for (int o2=0; o2<_numObjects; o2++)
		{
		  if (o1 != o2)
		    {
		      for (int i2=0; i2<_object[o2]->_numImages; i2++)
			{
			  Vector *bild2 = (Vector *)_object[o2]->_image[i2];
			  double dist = 0.0;

			  for (int i=0; i<maxDim; i++)
			    {
			      dist += sqr ((*bild1)[index[i]] - (*bild2)[index[i]]);
			    }

			  dist = sqrt (dist);

			  if (dist < distMin)
			    {
			      distMin = dist;
			    }
			}
		    }
		}
	    }
	}

      if (distMin > globDistMax)
	{
	  globDistMax = distMin;

	  indexBest = index;

	  for (int i=0; i<maxDim; i++) printf ("%i ", index[i]);
	  ::printf ("-> %f\n", globDistMax);
	}

#endif

    }
