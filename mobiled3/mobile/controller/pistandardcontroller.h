/***************************************************************************
                          pistandardcontroller.h  -  description
                             -------------------
    begin                : Thu Apr 11 2002
    copyright            : (C) 2002 by Axel Schneider, Daniel Westhoff
    email                : axel.schneider@uni-bielefeld.de,
														daniel.westhoff@uni-bielefeld.de
 ***************************************************************************/



#ifndef PISTANDARDCONTROLLER_H
#define PISTANDARDCONTROLLER_H

#include <sys/time.h>


/**
  *@author Axel Schneider, Daniel Westhoff
  */

class PIStandardController {



public:
	virtual ~PIStandardController();
  /**
	<pre>
		constructor with parameters 	kr = PI controller proportional gain
																	tr = PI controller time constant
	</pre>
	 */
  PIStandardController(const double kr, const double tr);
  /** 
		starts one cycle of the PI controller and returns the correcting variable
	 */
  double step(const double xDesired, const double xNow);
  /** 
		sets the controller's time constant tr
	 */
  void setTr(const double tr);
  /** 
		sets the gain kr for the PI controller
	 */
  void setKr(const double kr);
  /** 
deletes the sum of the I part
 */
  void deleteSum(void);
  /** 
sets the fading variable to weigh the I part
 */
  void setIFade(const double iFade);
  /** 
fading for the P part of the controller
 */
  void setPfade(const double pFade);



private: // Private methods
	/**
		no default constructor usage
	 */
   PIStandardController();



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
  /** 
		collects the the values for the integral part
	 */
  double _iSum;
};

#endif
