/***************************************************************************
                          picontroller.h  -  description
                             -------------------
    begin                : Mon Feb 25 2002
    copyright            : (C) 2002 by Axel Schneider, Daniel Westhoff
    email                : axel.schneider@uni-bielefeld.de,
													 daniel.westhoff@uni-bielefeld.de
 ***************************************************************************/


#ifndef PICONTROLLER_H
#define PICONTROLLER_H

#include <sys/time.h>

/**
  *@author Axel Schneider, Daniel Westhoff
  */

class PIController {



public:
	virtual ~PIController();
  /** 
	<pre>
		constructor with parameters 	kr = PI controller proportional gain
																	tr = PI controller time constant
	</pre>
	 */
   PIController(const double kr, const double tr);
  /** 
		starts one cycle of the PI controller and returns the correcting variable
	 */
  double step(const double xDesired, const double xNow);
  /** 
		sets the fading variable iFade
	 */
  void setIFade(const double iFade);
  /** 
		sets the gain kr for the PI controller
	 */
  void setKr(const double kr);
  /** 
		sets the controller's time constant _tr
	 */
  void setTr(const double tr);
  /** 
		fading for the P part of the controller
	 */
  void setPFade(const double pFade);
	


private: // Private methods
	/**
		no default constructor usage
	 */
   PIController();



private: // Private attributes
  /** 
		PI controller proportional gain
	 */
  double _kr;
  /**
		PI controller time constant
	 */
  double _tr;
  /**
		the sample time of the controller, it should be constant!!
	 */
  double _tSample;
  /**
		time stamp for start
	 */
  timeval _start;
  /** 
		time stamp for stop
	 */
  timeval _stop;
  /** 
		the latest control deviation
	 */
  double _xdk;
  /**
		the control deviation of the last cycle
	 */
  double _xdkOld;
  /**
		the latest correcting variable (output of the controller)
	 */
  double _yk;
  /**
		flag to indicate the first call of member function step
	 */
  bool _firstRun;
  /** 
		fading variable for the P part of the controller
	 */
  double _pFade;
  /** 
		fading variable to activate the I-part of the PI controller
	 */
  double _iFade;
};

#endif
