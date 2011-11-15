/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Mon Feb 25 16:27:14 CET 2002
    copyright            : (C) 2002 by Axel Schneider, Daniel Westhoff
    email                : axel.schneider@uni-bielefeld.de, daniel.westhoff@uni-bielefeld.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream.h>
#include <stdlib.h>

#include "picontroller.h"
#include "pistandardcontroller.h"

int main(int argc, char *argv[])
{
  cout << "Hello, World!" << endl;
	PIStandardController a();
  PIController controller(0.2, 0.05);
	double y;

	for (int a = 0; a <= 1000000; a++)
	{
		y = controller.step(1.0, 0.1);
		cout << y << endl;	
	}

  return EXIT_SUCCESS;
}
