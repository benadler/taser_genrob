//
// canVoltageTest.cc
// (C) by Torsten Scherer (TeSche)
// <itschere@techfak.uni-bielefeld.de>
//

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "mobile/canPlatform.h"

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

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

int main (int argc, char* argv[])
{

  struct timeval start, now, elapsed;

  CCANPLATFORM platform;

  gettimeofday (&start, 0x0);

  int raw;

  double volt;

  //

  if (argc != 2) 
    {
      std::cout << "  usage: canVoltageTest_HH log_file" << std::endl;
      exit (1);
    }


  std::string filename (argv[1]);

  FILE *plot;
  plot = popen ("gnuplot", "w");

  std::ofstream datafile (filename.c_str(),
			  std::ios::out | std::ios::trunc);

  //

  while (true)
    {

      raw = 0;

      volt = 0;
      
      for (int i=0; i<100; i++)
	{
	  usleep (100000);
      
	  raw += platform.GetBatteryVoltageRaw ();
	 
	}

      raw /= 100;

      gettimeofday (&now, 0x0);

      timersub (&now, &start, &elapsed);

      printf ( "%06i.%06i raw: %5i  volt: %2.3f\n",  
      	       elapsed.tv_sec,
      	       elapsed.tv_usec,
      	       raw,
	       platform.ConvertBatteryVoltage (raw));

      datafile << elapsed.tv_sec
	       << "."
	       << elapsed.tv_usec
	       << " "
	       << raw 
	       << std::endl;
      
      std::string str = "plot '" + filename + "' u ($1):($2) w l\n";
	
      fprintf (plot, str.c_str());
      fflush (plot);
    
    }

  return 0;
}
