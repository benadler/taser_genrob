package uhh.fbi.tams.mobilerobot.unit;

import java.io.Serializable;

/**
 * Durch eine Folge von {@link uhh.fbi.tams.mobilerobot.unit.Point2D}
 * beschriebenes Polygon. Die Reihenfolge der Punkte im gespeicherten
 * Array gibt die Reihenfolge der Punkte des Polygons an, d.h. es sind
 * konvexe, konkave und degenerierte Polygone mit sich &uuml;berschneidenden
 * Linien m&ouml;glich. Das Polygon ist geschlossen, d. h. der letzte Punkt
 * ist gedanklich mit dem ersten Punkt zu verbinden.
 *
 * @see uhh.fbi.tams.mobilerobot.unit.Point2D
 *
 * @author Daniel Westhoff
 * @version 0.1
 */
public class Polygon
    implements Serializable
{

  /** Punkte des Polygon. */
  private final Point2D[] points;

  /**
   * <b>Konstruktor nicht implementiert.</b>
   */
  private Polygon ()
  {
    points = null;
  }

  /**
   * Konstruktor erzeugt ein Polygon aus den &uuml;bergebenen 2D-Punkten.
   *
   * @param points Array mit 2D-Punkten deren Reihenfolge das Polygon
   *               beschreibt.
   */
  public Polygon (Point2D[] points)
  {
    this.points = points;
  }

  /**
   * Gibt die 2D-Punkte
   *
   * @return Point2D[] Array mit 2D-Punkten deren Reihenfolge das Polygon
   *                   beschreibt.
   */
  public final Point2D[] getPoints ()
  {
    return points;
  }

  /**
   * Polygon als String zur&uuml;ck geben.
   */
  public String toString ()
  {
    StringBuffer sb = new StringBuffer ("(");

    for (int i=0; i<points.length-1; ++i)
    {
      sb.append (points[i].toString());
      sb.append (",");
    }

    sb.append (points[points.length-1]);
    sb.append (")");

    return sb.toString();
  }
}
