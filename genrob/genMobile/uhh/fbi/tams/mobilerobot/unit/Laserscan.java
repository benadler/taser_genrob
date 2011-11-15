package uhh.fbi.tams.mobilerobot.unit;

import java.io.Serializable;

/**
 * Diese Klasse repr&auml;sentiert die Messpunkte eines Laserscans in
 * karthesischen Koordinaten.
 *
 * @since uhh.fbi.tams.mobilerobot 0.4
 * @version 0.2
 * @author Daniel Westhoff
 */
public class Laserscan
    implements Serializable
{
  /** Messpunkte. */
  private final Point2D[] scan;

  /** Positionen der Lasermarken. */
  private final Point2D[] marks;

  /**
   * Konstruktor.
   *
   * @param scan Liste der Messpunkte in karthesischen Koordinaten.
   */
  public Laserscan (Point2D[] scan)
  {
    if (scan == null)
    {
     this.scan = new Point2D [0];
    }
    else
    {
      this.scan = scan;
    }

    this.marks = new Point2D [0];
  }

  /**
   * Konstruktor.
   *
   * @param scan Liste der Messpunkte in karthesischen Koordinaten.
   * @param marks Liste der detektierten Markenpositionen in karthesischen
   *              Koordinaten.
   */
  public Laserscan (Point2D[] scan, Point2D[] marks)
  {
    this.scan = scan;

    if ((marks == null) || (marks.length == 0))
    {
      this.marks = new Point2D [0];
    }
    else
    {
      this.marks = marks;
    }
  }

  /**
   * Liefert die Messpunkte des Laserscans. Wurden keine Messwerte aufgenommen,
   * hat die Liste die L&auml;nge 0.
   *
   * @return Liste der Messpunkte in karthesischen Koordinaten.
   */
  public Point2D[] getScan ()
  {
    return scan;
  }

  /**
   * Liefert die Positionen der Reflectormarken des Scans. Wurden keine
   * Marken detektiert ist die L&auml;nge der Liste gleich 0.
   *
   * @return Liste mit den Positionen der detektierten Marken in karthesischen
   *         Koordinaten.
   */
  public Point2D[] getMarks ()
  {
    return marks;
  }
}
