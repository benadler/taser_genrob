package uhh.fbi.tams.mobilerobot.unit;

/**
 * Diese Ausnahme wird ausgel&ouml;st, wenn die interne Pfadplannung der
 * Robotersteuerung <tt>mobiled</tt> keinen Pfad zwischen Start- und
 * Zielpunkt finden konnte.
 */
public class PathNotFoundException
    extends InternalPathplannerException
{
  /** Konstruktor. */
  public PathNotFoundException ()
  {
    super ();
  }

  /** Konstruktor. */
  public PathNotFoundException (String message)
  {
    super (message);
  }

  /** Konstruktor. */
  public PathNotFoundException (Throwable cause)
  {
    super (cause);
  }

  /** Konstruktor. */
  public PathNotFoundException (String message, Throwable cause)
  {
    super (message, cause);
  }
}
