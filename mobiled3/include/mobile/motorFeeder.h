//
//
//

#ifndef _MOTORFEEDER_H_
#define _MOTORFEEDER_H_

#include "thread/condition.h"
#include "./canPlatform.h"

///////////////////////////////////////////////////////////////////////////////////////////
///
/// Thread zur Abfrage des Antriebsmotoren. �ber das <i>Controller Area Network</i>
/// (CAN) k�nnen die Messwerte der Inkrementalgeber in den Antriebsmotoren abgefragt 
/// werden. Au�erdem k�nnen die Radgeschwindigkeiten gesetzt werden. Es gibt weitere 
/// Sensoren, die �ber das CAN mittels dieser Klasse abgefragt werden k�nnen:
/// Temperatursensoren in den Antrieben, Spannung der Batterie, Gyroskop, Werte der
/// Joystick-Steuerung.
///
///////////////////////////////////////////////////////////////////////////////////////////
class CMOTORFEEDER : protected CTHREAD
{

  /////////////////////////////////////////////////////////////////////////////////////////
  /// Hauptklasse der Robotersteuerung <i>mobiled</i>
  /////////////////////////////////////////////////////////////////////////////////////////

  friend class CGENBASE;

private:

  /////////////////////////////////////////////////////////////////////////////////////////
  /// *** nicht implementiert ***
  /////////////////////////////////////////////////////////////////////////////////////////

  CMOTORFEEDER (const CMOTORFEEDER &);

  /////////////////////////////////////////////////////////////////////////////////////////
  /// *** nicht implementiert ***
  /////////////////////////////////////////////////////////////////////////////////////////

  CMOTORFEEDER &operator= (const CMOTORFEEDER &);

  /////////////////////////////////////////////////////////////////////////////////////////
  /// *** nicht implementiert ***
  /////////////////////////////////////////////////////////////////////////////////////////

  bool operator== (const CMOTORFEEDER &);

  /////////////////////////////////////////////////////////////////////////////////////////
  /// gemessene translatorische Geschwindiglkeit des Roboters
  /////////////////////////////////////////////////////////////////////////////////////////
  
  float _transVel;

  /////////////////////////////////////////////////////////////////////////////////////////
  /// gemessene rotatorische Geschwindiglkeit des Roboters
  /////////////////////////////////////////////////////////////////////////////////////////

  float _rotVel;

protected:

  /////////////////////////////////////////////////////////////////////////////////////////
  /// Hauptklasse der Robotersteuerung <i>mobiled</i>.
  /////////////////////////////////////////////////////////////////////////////////////////

  CGENBASE *_genBase;            

  /////////////////////////////////////////////////////////////////////////////////////////
  /// Zugriff auf das <i>Controller Area Network</i> (CAN) des Roboters.
  /////////////////////////////////////////////////////////////////////////////////////////
  
  CCANPLATFORM _canPlatform;         

  /////////////////////////////////////////////////////////////////////////////////////////
  ///
  /////////////////////////////////////////////////////////////////////////////////////////

  CCONDITION _trigger;

  /////////////////////////////////////////////////////////////////////////////////////////
  ///
  /////////////////////////////////////////////////////////////////////////////////////////

  void Trigger (void);

  /////////////////////////////////////////////////////////////////////////////////////////
  /// Hier werden zyklisch die Geschwindigkeiten gesetzt und die Sensoren Abgefragt.
  /////////////////////////////////////////////////////////////////////////////////////////

  virtual void Fxn (void);   

  /////////////////////////////////////////////////////////////////////////////////////////
  /// Sensorwerte.
  /////////////////////////////////////////////////////////////////////////////////////////

  typedef struct
  {
    double interval;
    double posLeft, posRight;
    double deltaPosLeft, deltaPosRight;
    double posGyro;
    double deltaPosGyro;
  } WORLDSTATE;

  /////////////////////////////////////////////////////////////////////////////////////////
  /// Liefert die Sensorwerte.
  /////////////////////////////////////////////////////////////////////////////////////////

  bool GetWorldState (WORLDSTATE *state);

  /////////////////////////////////////////////////////////////////////////////////////////
  /// Joystickwert x.
  /////////////////////////////////////////////////////////////////////////////////////////

  int _joyValX; 

  /////////////////////////////////////////////////////////////////////////////////////////
  /// Joystickwert y.
  /////////////////////////////////////////////////////////////////////////////////////////

  int _joyValY;

public:

  /////////////////////////////////////////////////////////////////////////////////////////
  /// Konstruktor.
  /////////////////////////////////////////////////////////////////////////////////////////

  CMOTORFEEDER (CGENBASE *genBase); 

  /////////////////////////////////////////////////////////////////////////////////////////
  /// Destruktor
  /////////////////////////////////////////////////////////////////////////////////////////

  virtual ~CMOTORFEEDER (void);     

  /////////////////////////////////////////////////////////////////////////////////////////
  /// Setzt die Geschwindigkeiten der Antriebsmotoren.
  /// <b>Anmerkung:</b> Die argumente sind Referenzen, da die Funktion die Geschwindigkeiten
  /// intern in Encoder-Geschwindigkeiten umrechnent. Dieser Schritt beinhaltet eine
  /// Rundung der Werte. Die Werte werden danach wieder zur�ckkonvertiert und zur�ckgegeben.
  /// Die gerundeten Werte werden m�glicherweise f�r eine Softwarekomponente auf h�herer 
  /// Ebene ben�tigt. 
  /////////////////////////////////////////////////////////////////////////////////////////

  void SetWorldVelocity (double &velLeft, double &velRight);

  /////////////////////////////////////////////////////////////////////////////////////////
  /// Liefert die Werte der Joystick-Steuerung.
  /////////////////////////////////////////////////////////////////////////////////////////

  void GetJoyValues (int &valX, int &valY);

  /////////////////////////////////////////////////////////////////////////////////////////
  /// Liefert die Batteriespannung.
  /////////////////////////////////////////////////////////////////////////////////////////

  double GetBatteryVoltage (void);

  /////////////////////////////////////////////////////////////////////////////////////////
  /// Liefert die Temperaturen der Antriebsmotoren.
  /////////////////////////////////////////////////////////////////////////////////////////

  double* GetDriveTemperatures (void);

  /////////////////////////////////////////////////////////////////////////////////////////
  /// Zieht die Bremsen der Antriebsmotoren an oder l�st sie.
  /////////////////////////////////////////////////////////////////////////////////////////

  void SetBrakes (const bool enable);

  /////////////////////////////////////////////////////////////////////////////////////////
  /// Liefert die gemessene translatorische und rotatorische 
  /// Geschwindigkeit des Roboters
  /////////////////////////////////////////////////////////////////////////////////////////

  void GetVelocities (float &transVel, float & rotVel) const;

};

#endif
