/***************************************************************************
                          pistandardcontroller.cpp  -  description
                             -------------------
    begin                : Thu Apr 11 2002
    copyright            : (C) 2002 by Axel Schneider, Daniel Westhoff
    email                : axel.schneider@uni-bielefeld.de,
														daniel.westhoff@uni-bielefeld.de
 ***************************************************************************/

#include "pistandardcontroller.h"


PIStandardController::PIStandardController (void)
{
}


PIStandardController::~PIStandardController (void)
{
}


/**
	constructor with parameters 	kr = PI controller proportional gain
																tr = PI controller time constant
 */
 PIStandardController::PIStandardController(const double kr, const double tr){

 	_kr = kr;
	_tr = tr;
	_firstRun = true;
	_tSample = 0.0;
	_iSum = 0.0;
	_yk = 0.0;
	_iFade = 1.0;
	_pFade = 1.0;
}



/** 
	starts one cycle of the PI controller and returns the correcting variable
 */
double PIStandardController::step(const double xDesired, const double xNow){

	//get an initial value for _start and _stop
	if(_firstRun == true)
	{
		gettimeofday(&_start, 0x0);
		_stop = _start;
		_firstRun = false;
	}
	//in "normal" use the time stamp for every new call is collected in _stop
	else
	{
		gettimeofday(&_stop, 0x0);
	}

  //the sample time is calculated, it must be constant and 10 times faster than the process!
	_tSample = (double) ((_stop.tv_sec * 1000000.0) + _stop.tv_usec) -
	  ((_start.tv_sec * 1000000.0) + _start.tv_usec);

	//convert into seconds
	_tSample /= 1000000.0;

	//calculate the control deviation
	_xdk = xDesired - xNow;
	
	//calculate I part
	_iSum += _xdk;

	//PI algorithm
	_yk = _kr * _pFade * _xdk + ((_kr * _iFade * _tSample / _tr) * _iSum);

	_start = _stop;

	return _yk;
}



/**
	sets the gain kr for the PI controller
 */
void PIStandardController::setKr(const double kr){

	_kr = kr;
}



/**
	sets the controller's time constant tr
 */
void PIStandardController::setTr(const double tr){

	_tr = tr;
}



/**
	deletes the sum of the I part
 */
void PIStandardController::deleteSum(void){

	_iSum = 0.0;
}



/**
	sets the fading variable to weigh the I part
 */
void PIStandardController::setIFade(const double iFade){

	_iFade = iFade;
}



/**
	fading for the P part of the controller
 */
void PIStandardController::setPfade(const double pFade){

	_pFade = pFade;
}
