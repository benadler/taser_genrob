//
//
//

#ifndef _MOVE_H_
#define _MOVE_H_

#include "./motion.h"
#include "../controller/picontroller.h"


/**
 * @ingroup Motion
 * @brief Diese Bewegung wird aus atomaren Bewegungen wie CROTATION und CTRANSLATION 
 *        oder andern Implementierungen von _CMOTION zusammengesetzt.
 * Diese Klasse ist das Kompositum in dem mehrere Bewegungen zusammengefügt werden.
 * Die einzelnen Bewegungen des Kompositums werden als Teilbewegungen bezeichnet.
 * Sie können wiederum Komposita sein.
 *
 * @section author Author
 *   Torsten Scherer,
 *   Daniel Westhoff (westhoff@informatik.uni-hamburg.de)
 * @section date Copyright
 *   (c) 2000-2006
 */
class CMOVE : protected _CMOTION
{
private:

  // not even implemented
  CMOVE (void);
  CMOVE (const CMOVE &);
  CMOVE &operator= (const CMOVE &);
  bool operator== (const CMOVE &);

protected:

  /** Anzahl der teilbewegungen. */
  int _numMotions;

  /** Nummer der aktuellen Bewegung. */
  int _activeMotion;

  /** Feld mit den Teilbewegungen. */
  _CMOTION **_motion;

  /** Aktuelle ausgeführte Teilbewegung. */
  _CMOTION *_current;

public:

  /**
   * @brief Konstruktor.
   * Erzeugt eine aus mehreren _CMOTION zusammengestzte Bewegung.
   * @param numOfMotions Anzahl der Bewegungen im übergebenen Feld.
   * @param motion Feld mit Bewegungen die ausgeführt werden sollen.
   */
  CMOVE (const int numOfMotions, _CMOTION **motion);

  /** Destruktor. */
  virtual ~CMOVE (void);

  /** _CMOTION */
  virtual bool Step (const CVEC pose,
		     const CVEC velocities,
		     double &transVel,
		     double &rotVel);

};

#endif
