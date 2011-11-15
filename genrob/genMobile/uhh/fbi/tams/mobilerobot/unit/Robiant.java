package uhh.fbi.tams.mobilerobot.unit;

/**
 * Statische Methoden um Winkelangaben in Robiant umzurechnen und umgekehrt.
 *
 * <p>Als <emph>Robiant</emph> wird die Darstellung von Winkeln zwischen -180
 * und 180 Grad &uuml;ber den gesamten Bereich eines Integer bezeichnet. Dies ist
 * z.B. in der mobilen Robotik von  Interesse, wenn mit der Orientierung eines
 * Roboters gerechnet wird. Blickt ein Roboter in Richtung 170 Grad und soll er
 * sich um weitere 20 Grad drehen, dann ist die neue Orientierung 190 Grad
 * und mit einer <tt>if</tt>-Abfrage muss dieser Winkel wieder in das
 * Intervall [-180;180] abgebildet werden, d.h. in diesem Fall auf -170 Grad.
 * Nutzt man die Robiant-Dastellung wird der Winkel &uuml;ber die Methoden
 * dieser Klasse in einen Integer umgerechnet. Dann wird nur mit Integern
 * gerechnet ohne auf &Uuml;berläufe zu achten. Erst wenn man wieder einen
 * Winkel ben&ouml;tigt wird vom Integerwert zur&uuml;ckgerechnet.
 *
 * @since uhh.fbi.tams.mobilerobot.unit 0.4
 * @author Hagen Stanek, Daniel Westhoff
 * @version 1.0
 */

public final class Robiant
{
  /** Default-Konstruktor. */
  private Robiant (){}

  /**
   * Rechnet einen Winkel in Grad nach Robiant um.
   *
   * @param degree Winkel in Grad.
   * @return int Winkel in Robiant.
   */
  public static int degree2robiant (double degree)
  {
    return (int) ((degree / -180D) * -2147483648D);
  }

  /**
   * Rechnet einen Winkel in Grad nach Robiant um.
   *
   * @param degree Winkel in Grad.
   * @return int Winkel in Robiant.
   */
  public static int degree2robiant (float degree)
  {
    return (int) ((degree / -180F) * -2.147484E+009F);
  }

  /**
   * Rechnet einen Winkel in Robiant nach Grad um.
   *
   * @param robiant Winkel in Robiant.
   * @return double Winkel in Grad.
   */
  public static double robiant2degree (int robiant)
  {
    return ((double) robiant / -2147483648D) * -180D;
  }

  /**
   * Rechnet einen Winkel in Robiant nach Grad um.
   *
   * @param robiant Winkel in Robiant.
   * @return float Winkel in Grad.
   */
  public static float robiant2degreeAsFloat (int robiant)
  {
    return ((float) robiant / -2.147484E+009F) * -180F;
  }

  /**
   * Rechnet einen Winkel in Rad nach Robiant um.
   *
   * @param rad Winkel in Rad.
   * @return int Winkel in Robiant.
   */
  public static int rad2robiant (double rad)
  {
    return (int) ((rad / -3.1415926535897931D) * -2147483648D);
  }

  /**
   * Rechnet einen Winkel in Rad nach Robiant um.
   *
   * @param rad Winkel in Rad.
   * @return int Winkel in Robiant.
   */
  public static int rad2robiant (float rad)
  {
    return (int) (((double) rad / -3.1415926535897931D) * -2147483648D);
  }

  /**
   * Rechnet einen Winkel in Robiant nach Rad um.
   *
   * @param robiant Winkel in Robiant.
   * @return double Winkel in Rad.
   */
  public static double robiant2rad (int robiant)
  {
    return ((double) robiant / -2147483648D) * -3.1415926535897931D;
  }

  /**
   * Rechnet einen Winkel in Robiant nach Rad um.
   *
   * @param robiant Winkel in Robiant.
   * @return float Winkel in Rad.
   */
  public static float robiant2radAsFloat (int robiant)
  {
    return (float) ((double) ((float) robiant / -2.147484E+009F)
                    * -3.1415926535897931D);
  }

  /**
   * Rechnet von karthesischen Koordinaten <i>(x,y)</i> nach Polarkoordinaten
   * <i>(r,theta)</i>.
   * <b>Vergleiche <a href="http://java.sun.com/j2se/1.4.2/docs/api/java/lang/Math.html#atan2(double, double)">Math.atan2()</a>.</b>
   *
   * @param y <i>x</i>-Koordinate.
   * @param x <i>y</i>-Koordinate.
   * @return int <i>theta</i>
   */
  public static int atan2 (int y, int x)
  {
    return rad2robiant (Math.atan2 (y, x));
  }

  /**
   * Berechnet den Kosinus eines Winkels in Robiant.
   *
   * @param robiant Winkel in Robiant.
   * @return double Kosinus des Winkels.
   */
  public static double cos (int robiant)
  {
    return Math.cos (robiant2rad (robiant));
  }

  /**
   * Berechnet den Sinus eines Winkels in Robiant.
   *
   * @param robiant Winkel in Robiant.
   * @return double Sinus des Winkels.
   */
  public static double sin (int robiant)
  {
    return Math.sin (robiant2rad (robiant));
  }

  /**
   * Berechnet <i>r * cos (a)</i>.
   *
   * @param r
   * @param a Winkel in Robiant.
   * @return int <i>r * cos (a)</i>.
   */
  public static int rcos (int r, int a)
  {
    return (int) ((double) r * Math.cos (robiant2rad (a)));
  }

  /**
   * Berechnet <i>r * sin (a)</i>.
   *
   * @param r
   * @param a Winkel in Robiant.
   * @return int <i>r * sin (a)</i>.
   */
  public static int rsin (int r, int a)
  {
    return (int) ((double) r * Math.sin (robiant2rad (a)));
  }
}
