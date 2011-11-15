/***************************************************************************
                          controller.h  -  description
                             -------------------
    begin                : Mon Feb 25 2002
    copyright            : (C) 2002 by Axel Schneider, Daniel Westhoff
    email                : axel.schneider@uni-bielefeld.de,
													 daniel.westhoff@uni-bielefeld.de
 ***************************************************************************/


#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "picontroller.h"

/**
  *@author Axel Schneider, Daniel Westhoff
  */

class Controller {



public:
	virtual ~Controller();
  /**
		the constructor with the parameters for the two PI controllers
		the parameter k is for the proportion of the outputs of the two controllers
		the distance of the robot's wheels are 2 * la
	 */
   Controller(const double kr_phi,
							const double tr_phi,
							const double kr_dist,
							const double tr_dist,
							const double rWheels,
							const double la);
  /**
		executes one control cycle for the angular velocity
		<pre>
		phi-desired is the target angle for the robot
		phi-Now is the actual angle of the robot
		vTransWheelMax is the maximum translatory speed of a wheel
		omegaR,L are the resulting angular velocities of the wheels
		</pre>
	 */
  void stepOmega(const double phiDesired,
								const double phiNow,
								const double vTransWheelMax,
								double &omegaR,
								double &omegaL);
  /** 
		executes one control cycle for the velocity
		<pre>
		x-desired is the target position in the translatory dof of the robot
		x-Now is the actual position in the translatory dof of the robot
		vTransWheelMax is the maximum translatory speed of a wheel
		omegaR,L are the resulting angular velocities of the wheels
		</pre>
	 */
  void stepVTrans(const double xDesired,
									const double xNow,
									const double vTransWheelMax,
									double &omegaR,
									double &omegaL);
  /** 
		calculates the difference angle between the robot and a desired goal point
	 */
  double calcDifferenceAngle(	const double xRobot,
															const double yRobot,
															const double phiRobot,
															const double xGoal,
															const double yGoal);


public: // Public attributes



private: // Private methods
	Controller();
  /**
		calculates the distance between the desired values and the actual values
	 */
  double calcDist(const double xDesired,
									const double yDesired,
									const double xNow,
									const double yNow);
  /** 
		delivers the abslolute value of the variable x
	 */
  double fabs(const double x);



private: // Private attributes
  /**
		the rotation controller
	 */
  PIController omegaControl;
  /**
		the velocity controller
	 */
  PIController vTransControl;
  /**
		holds the output of the omega controller (correcting variable)
	 */
  double _omega;
  /**
		holds the output of the velocity controller (correcting variable)
	 */
  double _vTrans;
  /** 
		the distance from the actual robot position to the goal point
		as input for the omega controller
	 */
  double _distanceToGoal;
  /**
		the maximum translatory velocity of the robot
	 */
  double _vTransWheelMax;
  /**
		the radius of the robot's wheels
	 */
  double _rWheels;
  /**
		2* la is the distance between the centres of the two drive wheels
	 */
  double _la;
  /** 
		the angular velocity of the right wheel
	 */
  double _omegaR;
  /**
		the angular velocity of the left wheel
	 */
  double _omegaL;
  /** 
		maximum  rotational speed of the single wheel calculated from _vTransMax
	 */
  double _omegaWheelMax;
  /** 
		storage for the desired angle of the robot
	 */
  double _phiDesired;
  /** 
		the old phi desired
	 */
  double _phiDesiredOld;
  /**
		storage for the desired x position in the translatory dof of the robot
	 */
  double _xDesired;
  /**
		the old x desired
	 */
  double _xDesiredOld;

};

#endif
