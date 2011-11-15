package uhh.fbi.tams.mobilerobot.unit;

/**
 * Diese Ausnahme wird ausgel&ouml;st, wenn die interne Pfadplanung der
 * Robotersteuerung <tt>mobiled</tt> feststellt, das der Startpunkt der
 * Pfadsuche in einen Hindernis liegt.
 */
public class StartInObstacleException
    extends InternalPathplannerException
{
  /** Konstruktor. */
  public StartInObstacleException ()
  {
    super ();
  }

  /** Konstruktor. */
  public StartInObstacleException (String message)
  {
    super (message);
  }

  /** Konstruktor. */
  public StartInObstacleException (Throwable cause)
  {
    super (cause);
  }

  /** Konstruktor. */
  public StartInObstacleException (String message, Throwable cause)
  {
    super (message, cause);
  }
}
