package uhh.fbi.tams.mobilerobot.unit;



/**
 * Wird ausgel&ouml;st, wenn der Roboter eine Bewegung beginnen soll,
 * die Bremsen aber noch nicht gel&ouml;st wurden.
 *
 * @author daniel Westhoff
 * @since uhh.fbi.tams.mobilerobot 0.4
 * @version 0.1
 * @see uhh.fbi.tams.mobilerobot.unit.motion.Brakes
 * @see uhh.fbi.tams.mobilerobot.unit.motion.Motion
 */
public class BrakesOnException
    extends MobileRobotException
{
  /** Konstruktor. */
  public BrakesOnException ()
  {
    super ();
  }

  /** Konstruktor. */
  public BrakesOnException (String message)
  {
    super (message);
  }

  /** Konstruktor. */
  public BrakesOnException (Throwable cause)
  {
    super (cause);
  }

  /** Konstruktor. */
  public BrakesOnException (String message, Throwable cause)
  {
    super (message, cause);
  }
}
