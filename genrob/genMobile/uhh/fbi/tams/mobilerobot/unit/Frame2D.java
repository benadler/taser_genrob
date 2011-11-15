package uhh.fbi.tams.mobilerobot.unit;

import java.io.Serializable;

/**
 * Ein <tt>Frame2D</tt> bezeichnet einen Punkt und eine Orientierung
 * im 2D-Raum.
 */
public class Frame2D
    implements Serializable
{
  /** x-koordinate in  [mm]. */
  public int x;

  /** y-Koordinate in [mm]. */
  public int y;

  /** Orientierung in [Robiant]. */
  public int theta;

  /** Konstruktor. */
  public Frame2D (int x, int y, int theta)
  {
    this.x = x;
    this.y = y;
    this.theta = theta;
  }

  /**
   * Vergleicht zwei Frames miteinander und gibt <tt>true</tt> zur&uuml;ck,
   * wenn sowohl x, y als auch die Orientierung gleich sind.
   *
   * @param object Objekt, dass mit diesem Frame verglichen werden soll.
   * @return <tt>true</tt>, wenn die beiden Frames gleich sind, sonst
   *         <tt>false</tt>.
   */
  public boolean equals (Object object)
  {
    // Ist der Typ des Objektes gleich?
    if (!(object.getClass() != getClass()))
    {
      return false;
    }

    // Wenn ja, dann casten...
    Frame2D frame = (Frame2D) object;

    // ...und vergleichen
    if ((x == frame.x) && (y == frame.y) && (theta == frame.theta))
    {
      return true;
    }
    else
    {
      return false;
    }
  }


  /**
   * Gibt den Frame als Zeichenkette zur&uuml;ck.
   *
   * <p><b>Achtung:</b> Die Orientierung wird intern in {@link Robiant}
   * gespeichert, für die Ausgabe wird die Orientierung aber in <i>Grad</i>
   * umgerechnet und ausgegeben.
   *
   * @return Zeichenkette die den Frame wiedergibt.
   */
  public String toString ()
  {
    StringBuffer sb = new StringBuffer ("(");
    sb.append (x);
    sb.append (" mm, ");
    sb.append (y);
    sb.append (" mm, ");
    sb.append (Robiant.robiant2degree(theta));
    sb.append ("\u00B0)");

    return sb.toString();
  }
}
