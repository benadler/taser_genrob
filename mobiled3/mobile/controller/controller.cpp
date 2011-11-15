/***************************************************************************
                          controller.cpp  -  description
                             -------------------
    begin                : Mon Feb 25 2002
    copyright            : (C) 2002 by Axel Schneider, Daniel Westhoff
    email                : axel.schneider@uni-bielefeld.de,
													 daniel.westhoff@uni-bielefeld.de
 ***************************************************************************/


#include <math.h>
#include <iostream>

#include "controller.h"

Controller::~Controller (void)
{
}



/**
	the constructor with the parameters for the two PI controllers
 */
Controller::Controller(const double kr_phi,
		       const double tr_phi,
		       const double kr_dist,
		       const double tr_dist,
		       const double rWheels,
		       const double la)
:omegaControl(kr_phi, tr_phi), vTransControl(kr_dist, tr_dist){

	_omega = 0.0;
	_omegaR = 0.0;
	_omegaL = 0.0;
	_omegaWheelMax = 0.0;
	_vTrans = 0.0;
	_vTransWheelMax = 0.0;
	_distanceToGoal = 0.0;
	_rWheels = rWheels;
	_la = la;
	_phiDesired = 0.0;
	_phiDesiredOld = 0.0;
	_xDesired = 0.0;
	_xDesiredOld = 0.0;
}



/**
	executes one control cycle for the angular velocity
 */
void Controller::stepOmega(const double phiDesired,
													const double phiNow,
													const double vTransWheelMax,
													double &omegaR,
													double &omegaL){

	double omegaWheel;
	double iFade = 0.0;

	//storing...
	_phiDesired = phiDesired;
	_vTransWheelMax = vTransWheelMax;
	_omegaWheelMax = _vTransWheelMax / _rWheels;


	//don't use too much I when far away from the desired value
	iFade = exp(-200.0 * fabs(_phiDesired - phiNow));
  //cout << "iFade " << iFade << endl;
	//switch on the I part of the controller for small angles only
#if 1
	omegaControl.setIFade(0.0);
#else
	if (fabs(_phiDesired - _phiDesiredOld) < (1.0 * M_PI /180))
		omegaControl.setIFade(iFade);
  else
		omegaControl.setIFade(0.0);
#endif
	omegaControl.setPFade(1.0);



	//get the value for the rotational speed from the rotation controller
	_omega = omegaControl.step(phiDesired, phiNow);



	//clipping of the rotatory part
	omegaWheel = _omega * _la / _rWheels;
	if(omegaWheel > _omegaWheelMax) omegaWheel = _omegaWheelMax;
	if(omegaWheel < -_omegaWheelMax) omegaWheel =-_omegaWheelMax;

	_phiDesiredOld = _phiDesired;
	_omegaR = omegaWheel;
	_omegaL = -omegaWheel;

	omegaR = _omegaR;
	omegaL = _omegaL;
}



/**
	executes one control cycle for the velocity
 */
void Controller::stepVTrans(const double xDesired,
														const double xNow,
														const double vTransWheelMax,
														double &omegaR,
														double &omegaL){

	double omegaWheel;
	double iFade;

	//storing...
	_xDesired = xDesired;
	_vTransWheelMax = vTransWheelMax;
	_omegaWheelMax = (_vTransWheelMax / _rWheels) - fabs(_omegaR);



	//don't use too much I when far away from the desired value
	iFade = exp(-1.0 * fabs(_xDesired - _xDesiredOld));

	vTransControl.setIFade(iFade);
	vTransControl.setPFade(1.0);



	//get the value for the translatory speed from the tanslation velocity controller
	_vTrans = vTransControl.step(xDesired, xNow);



	//clipping of the rotatory part
	omegaWheel = _vTrans / _rWheels;
	if(omegaWheel > _omegaWheelMax) omegaWheel = _omegaWheelMax;
	if(omegaWheel < -_omegaWheelMax) omegaWheel =-_omegaWheelMax;
	


	_xDesiredOld = _xDesired;
	//add the rotatory part, which is on _omegaR,L, to the translatory part
	omegaR = _omegaR + omegaWheel;
	omegaL = _omegaL + omegaWheel;
}



/**
	calculates the distance between the desired values and the actual values
 */
double Controller::calcDist(const double xDesired,
														const double yDesired,
														const double xNow,
														const double yNow){

	return sqrt(((xDesired - xNow)*(xDesired - xNow)) +
							((yDesired - yNow)*(yDesired - yNow)));
}



/**
	delivers the abslolute value of the variable x
 */
double Controller::fabs(const double x){

	
	if(x < 0.0)
		return -x;
	return x;
}



/**
	calculates the difference angle between the robot and a desired goal point
 */
double Controller::calcDifferenceAngle(	const double xRobot,
																				const double yRobot,
																				const double phiRobot,
																				const double xGoal,
																				const double yGoal){

	double numerator = 0.0;
	double denominator = 1.0;
	double deltaX = 0.0;
	double deltaY = 0.0;

	deltaX = xGoal - xRobot;
	deltaY = yGoal - yRobot;

	numerator = (cos(phiRobot) * deltaX) +
							(sin(phiRobot) * deltaY);

	denominator = 1.0 * sqrt((deltaX * deltaX) + (deltaY * deltaY));

	//if robot position = goal position then there is no difference any more
	if (denominator == 0.0)
		return 0.0;

	return acos(numerator / denominator);

}
