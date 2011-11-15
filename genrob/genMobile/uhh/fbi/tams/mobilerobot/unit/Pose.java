package uhh.fbi.tams.mobilerobot.unit;

import java.io.Serializable;
import uhh.fbi.tams.mobilerobot.*;



/**
 * Die Pose eines Roboters beinhaltet desen karthesische Koordinaten in Bezug
 * auf eine Weltkoordinatensystem sowie die Orientierung des Roboters. Die
 * Orientierung ist der  Winkel zwischen x-Achse des Weltkoordinatensystems
 * und Fahrtrichtung des Roboters.
 *
 * @author Daniel Westhoff
 * @version 0.1
 */

public class Pose
    implements Serializable
{
  /** x-Koordinate der Position des Roboters im Weltkoordinatensystem in [mm]. */
  public int x;

  /** y-Koordinte der Position des Roboters im Weltkoordinatensystem in [mm]. */
  public int y;

  /** Orientierung des Roboters in [Robiant]. */
  public int theta;

  /**  Default-Konstruktor. Setzt alle Werte auf 0. */
  public Pose ()
  {
    x = 0;
    y = 0;
    theta = 0;
  }


  /**
   * Erzeugt neue Pose mit den &uuml;bergebenen Parametern.
   *
   * @param xCoord int x-Koordinate in [mm]
   * @param yCoord int y-Koordinate in [mm]
   * @param orientation int Orientierung in [Robiant]
   */
  public Pose (int xCoord,
               int yCoord,
               int orientation)
  {
    x = xCoord;
    y = yCoord;
    theta = orientation;
  }

  /**
   * Gibt einen String mit der x-Koordinate, der y-Koordinate und
   * der Orientierung zur&uuml;ck.
   */
  public String toString ()
  {
    return "(x: "
        + x
        + " mm; y: "
        + y
        + " mm; theta: "
        + Robiant.robiant2degree (theta)
        + "\u00B0)";
  }


  /**
   * Vergleichen mit einer anderen Pose. Sind x-Koordinate,
   * y-Koordinate und Orientierung &uuml;bereinstimmend mit dem
   * &uuml;bergebenen Punkt wird <tt>true</tt> zurück gegeben.
   */
  public boolean equals (Object obj)
  {
    if (!(obj instanceof Pose))
    {
      return false;
    }

    Pose pose = (Pose) obj;

    return (x == pose.x)
        && (y == pose.y)
        && (theta == pose.theta);
  }


  /** HashCode */
  public int hashCode ()
  {
    return x;
  }

}
