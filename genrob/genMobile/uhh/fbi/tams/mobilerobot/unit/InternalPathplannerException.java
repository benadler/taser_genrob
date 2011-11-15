package uhh.fbi.tams.mobilerobot.unit;

/**
 * Oberklasse der Ausnahmen, die ausgel&ouml;st werden, wenn die interne
 * Pfadplannung der Robotersteuerung <tt>mobiled</tt> einen Fehler festgestellt
 * hat.
 *
 * @see PathNotFoundException
 * @see StartInObstacleException
 * @see GoalInObstacleException
 */
public class InternalPathplannerException
    extends MobileRobotException
{
  /** Konstruktor. */
  public InternalPathplannerException ()
  {
    super ();
  }

  /** Konstruktor. */
  public InternalPathplannerException (String message)
  {
    super (message);
  }

  /** Konstruktor. */
  public InternalPathplannerException (Throwable cause)
  {
    super (cause);
  }

  /** Konstruktor. */
  public InternalPathplannerException (String message, Throwable cause)
  {
    super (message, cause);
  }
}
