package uhh.fbi.tams.mobilerobot.unit;

import java.io.Serializable;


/**
 * Eine Landmarke ist ein vom Roboter &uuml;ber die Sensorik identifizierbares
 * Merkmal, z.B. ein Reflektorstreifen f&uuml;r einen Laserscanner. Eine Landmarke
 * hat ein Identifikationsnummer, eine über eine Koordinate festgelegte
 * Position in einer Karte und eine Blickrichtung aus der die Landmarke
 * erfasst werden kann.
 *
 * @author Daniel Westhoff
 * @version 0.1
 */
public class Landmark
    implements Serializable
{
  /** Eindeutige Identifikationsnummer */
  public int ID;

  /** 2D-Position der Landmarke (in [mm])*/
  public Coord position;

  /** Orientierung/Blickrichting der Landmarke (in [Robiant])*/
  public int angle;

  /** Symetrischer Bereich um die Orientierung, in dem die Marke zu sehen ist
   *  (in [Robiant])
   */
  public int range;

  /** Konstruktor. */
  public Landmark (int rID,
                   Coord rPosition,
                   int rAngle,
                   int rRange)
  {
    ID = rID;
    position = rPosition;
    angle = rAngle;
    range = rRange;
  }


  /** Gibt einen String mit Informationen &uuml;ber die Landmarke zur&uuml;ck.*/
  public String toString ()
  {
    return "(ID: "
        + ID
        + "; Position: "
        + position
        + "; Angle: "
        + Robiant.robiant2degree (angle)
        + "\u00B0; Range: "
        + Robiant.robiant2degree (range)
        + "\u00B0)";
  }


  /**
   * Liefert <tt>true</tt> zur&uuml;ck, wenn Position, Blickrichtung und
   * &Ouml;ffnungswinkel mit der &uuml;bergebenen Landmarke &uuml;bereinstimmen.
   */
  public boolean equals (Object obj)
  {
    if (!(obj instanceof Landmark))
    {
      return false;
    }

    Landmark mark = (Landmark) obj;

    return (position.x == mark.position.x)
        && (position.y == mark.position.y)
        && (angle == mark.angle)
        && (range == mark.range);
  }


  /** HashCode */
  public int hashCode ()
  {
    return position.hashCode ();
  }

}
