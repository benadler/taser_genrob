package uhh.fbi.tams.mobilerobot.unit;

/**
 * Oberklasse aller Ausnahmen im Paket <tt>uhh.fbi.tams.mobilerobot</tt>.
 *
 * @since uhh.fbi.tams.mobilerobot 0.4
 * @author Daniel Westhoff
 * @version 0.1
 */
public class MobileRobotException
    extends Exception
{
  /** Konstruktor. */
  public MobileRobotException ()
  {
    super ();
  }

  /** Konstruktor. */
  public MobileRobotException (String message)
  {
    super (message);
  }

  /** Konstruktor. */
  public MobileRobotException (Throwable cause)
  {
    super (cause);
  }

  /** Konstruktor. */
  public MobileRobotException (String message, Throwable cause)
  {
    super (message, cause);
  }
}
