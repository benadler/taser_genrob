package uhh.fbi.tams.mobilerobot.unit;

import java.io.Serializable;
import uhh.fbi.tams.mobilerobot.*;


/**
 * Zweidimensionaler Punkt, beschrieben durch zwei
 * Integer-Koordinaten x und y.
 *
 * @author Daniel Westhoff
 * @version 0.1
 */
public class Point2D
    implements Serializable
{
  public int x = 0;

  public int y = 0;

  /**
   * Dieser Konstruktor erstellt einen 2D-Punkt mit den Koordinaten
   * x=0 und y=0;
   */
  public Point2D ()
  {
  }

  /**
   * Dieser Konstruktor erstellt einen 2D-Punkt mit den &uuml;bergebenen
   * Koordinaten.
   *
   * @param x int x-Koordinate
   * @param y int y-Koordinate
   */
  public Point2D (int x, int y)
  {
    this.x = x;
    this.y = y;
  }

  /**
   * Copy-Konstruktor.
   *
   * @param point 2D-Point
   */
  public Point2D (Point2D point)
  {
    this.x = point.x;
    this.y = point.y;
  }

  /**
   * Vergleiche mit einem anderen Punkt.
   *
   * @param point 2D-Punkt
   * @return boolean Wahr, wenn die x- <b>und</b> y-Koordinaten &uuml;bereinstimmen.
   */
  public boolean equals (Point2D point)
  {
    return (this.x == point.x) && (this.y == point.y);
  }

  /**
   * Gibt die Koordinaten des Punktes als String zur&uuml;ck.
   *
   * @return String Koordinaten des Punktes.
   */
  public String toString ()
  {
    return "(x=" + this.x + ",y=" + this.y + ")";
  }
}
