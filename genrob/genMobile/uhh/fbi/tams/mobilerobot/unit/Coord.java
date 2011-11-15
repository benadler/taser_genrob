package uhh.fbi.tams.mobilerobot.unit;

import java.io.Serializable;
import uhh.fbi.tams.mobilerobot.*;

/**
 *  Karthesische Koordinate in [mm].
 *
 * @author Daniel Westhoff
 * @version 0.1
 */
public class Coord
    implements Serializable
{
  /** x-Koordinate in [mm]*/
  public int x;

  /** y-Koordinate in [mm]*/
  public int y;

  /** Konstruktor erzeugt neue Koordinate mit x=0 [mm] und y =0 [mm]. */
  public Coord ()
  {
    x = 0;
    y = 0;
  }

  /** Konstruktor erzeugt neue Koordinate mit x=rX und y=rY.
   *
   * @param rX x-Koordinate in [mm].
   * @param rY y Koordinate in [mm].
   */
  public Coord (int rX, int rY)
  {
    x = rX;
    y = rY;
  }

  /**
   * Vergleicht die Koordinate mit einer anderen und gibt <tt>true</tt>
   * zur&uuml;ck, wenn die x- und y-Werte &uuml;bereinstimmen.
   *
   * @param c Koordinate mit der verglichen werden soll
   *
   * @return <tt>true</tt>, wenn x- und y-Werte &uuml;bereinstimmen
   */
  public boolean equals (Coord c)
  {
    return ((this.x == c.x) && (this.y == c.y));
  }

  /** Liefert eine String mit der x- und y-Koordinate zur&uuml;ck. */
  public String toString ()
  {
    return "(x: "
        + x
        + " mm; y: "
        + y
        + " mm)";
  }


  /**
   * Vergleicht die Komponenten mit den Komponenten der &uuml;bergebenen
   * Koordinate.
   */
  public boolean equals (Object obj)
  {
    if (!(obj instanceof Coord))
    {
      return false;
    }

    Coord coord = (Coord) obj;
    return (x == coord.x) && (y == coord.y);
  }


  /**  */
  public int hashCode ()
  {
    return x;
  }

}
