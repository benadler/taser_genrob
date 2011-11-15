package uhh.fbi.tams.mobilerobot.unit;

/**
 * Diese Ausnahme wird ausgel&ouml;st, wenn die interne Pfadplanung der
 * Robotersteuerung <tt>mobiled</tt> feststellt, das der Zielpunkt der
 * Pfadsuche in einen Hindernis liegt.
 */
public class GoalInObstacleException
    extends InternalPathplannerException
{
  /** Konstruktor. */
  public GoalInObstacleException ()
  {
    super ();
  }

  /** Konstruktor. */
  public GoalInObstacleException (String message)
  {
    super (message);
  }

  /** Konstruktor. */
  public GoalInObstacleException (Throwable cause)
  {
    super (cause);
  }

  /** Konstruktor. */
  public GoalInObstacleException (String message, Throwable cause)
  {
    super (message, cause);
  }
}
