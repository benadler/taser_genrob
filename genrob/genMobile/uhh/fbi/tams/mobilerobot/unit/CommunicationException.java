package uhh.fbi.tams.mobilerobot.unit;

/**
 * Oberklasse aller Ausnahmen, die ausgel&ouml;st werden, wenn eine Fehler
 * bei der Kommunikation mit der Robotersteuerung <tt>mobiled</tt> aufgetreten
 * ist.
 *
 * @since uhh.fbi.tams.mobilerobot 0.7
 * @author Daniel Westhoff
 * @version 0.1
*/
public class CommunicationException
    extends MobileRobotException
{
  /** Konstruktor. */
  public CommunicationException ()
  {
    super ();
  }

  /** Konstruktor. */
  public CommunicationException (String message)
  {
    super (message);
  }

  /** Konstruktor. */
  public CommunicationException (Throwable cause)
  {
    super (cause);
  }

  /** Konstruktor. */
  public CommunicationException (String message, Throwable cause)
  {
    super (message, cause);
  }
}
