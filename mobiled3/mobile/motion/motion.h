//
//
//


/** @defgroup Motion Motion: Steuerung der mobilen Plattform.
 *  Klassen zur Steuerung der mobilen Plattform.
 *  @{
 */

#ifndef _MOTION_H_
#define _MOTION_H_

#include "math/vec.h"

/**
 * @brief Abstrakte Oberklasse für alle Bewegungen die der Roboter ausführen kann. 
 * 
 * Bewegungen des Roboters werden nach dem Prinzip des Entwurfsmusters <b>Komposition</b>.
 * Die Klasse _CMOTION entspricht dabei der <b>Komponente</b>. Atomare Bewegungen wie
 * CROTATION oder CTRANSLATION sind von _CMOTION abgeleitet. 
 * CMOVE ist ebenfalls von _CMOTION abgeleitet, stellt aber das 
 * <b>Kompositum</b> des Entwurfsmusters dar.  In CMOVE wird eine Bewegung aus 
 * mehreren atomaren Bewegungen zusammengebaut.
 *
 * @image html Kompositum.png
 * @image latex Kompositum
 *
 * Anhand der aktuellen Pose des Roboters werden die Sollgeschwindigkeiten
 * für die Rotation und Translation berechnet.
 *
 * @section author Author
 *   Torsten Scherer,
 *   Daniel Westhoff (westhoff@informatik.uni-hamburg.de)
 * @section date Copyright
 *   (c) 2000-2006
 */
class _CMOTION
{

public:

  typedef enum
    {
      MOTION_ROTATEANGLE,
      MOTION_ROTATEPOINT,
      MOTION_TRANSLATE,
      MOTION_MOVE,
      MOTION_FORWARD
    } MOTIONTYPE;

private:

  /** NOT IMPLEMENTED */
  _CMOTION (void);

  /** NOT IMPLEMENTED */
  _CMOTION (const _CMOTION &);

  /** NOT IMPLEMENTED */
  _CMOTION &operator= (const _CMOTION &);

  /** NOT IMPLEMENTED */
  bool operator== (const _CMOTION &);

protected:

  /** Art der Bewegung. */
  MOTIONTYPE _type;

  /** Translationsgeschwindigkeit. */
  double _transVel;

  /** Rotationsgeschwindigkeit. */
  double _rotVel;

  bool _velocitiesChanged;

  volatile bool _motorStopped;

  bool _abort;

  //

  _CMOTION (const MOTIONTYPE type);

public:

  /** Destruktor. */
  virtual ~_CMOTION (void);

  /** 
   * @brief Diese Methode wird aufgerufen um die Sollgeschwindigkeiten in
   *        Abhängigkeit der übergebenen Pose und Geschwindigkeit des 
   *        Roboters zu berechnen. 
   *
   * @param[in] pose Die Pose des Roboters.
   * @param[in] velocities Die Geschwindigkeiten des Roboters.
   * @param[out] transVel Translatorische Sollgeschwindigkeit für diesen Schritt.
   * @param[out] rotVel Rotatorische Sollgeschwindigkeit für diesen Schritt.
   * @return <b>true</b>, wenn die Bewegung abgeschlossen ist.
   */
  virtual bool Step (const CVEC pose, 
		     const CVEC velocities,
		     double &transVel, 
		     double &rotVel) = 0x0;

  
  //
  MOTIONTYPE GetType (void) const
    {
      return _type;
    }


  //
  // this one is used to write back the velocities as clipped by the collision
  // avoidance. the next Step() method is supposed to consider them.
  //

  void SetVelocities (const double transVel, const double rotVel)
  {
    _velocitiesChanged = true;
    _transVel = transVel;
    _rotVel = rotVel;
  }

  //
  // this one is used to write back a flag that the velocities have been
  // clipped to zero by rounding effects in the motor conversion.
  //

  void SetMotorStopped (void)
    {
      _motorStopped = true;
    }

  bool GetMotorStopped (void) const
    {
      return _motorStopped;
    }

  //
  //
  //

  void Abort (void)
    {
      _abort = true;
    }

};

#endif

/** @} */ // end of group Motion
