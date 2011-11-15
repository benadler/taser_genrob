package uhh.fbi.tams.mobilerobot.unit;

/**
 * Diese Ausnahme wird in Methoden ausgel&ouml;st, die den Roboter steuern.
 * Solche Methoden k&ouml;nnen nur aufgerufen werden, wenn vorher &uuml;ber die
 * Einheit {@link Access} ein Roblet&reg; Zugriff auf die Steuerung des
 * Roboters erhalten hat.
 *
 * <p><b>Hinweis:</b> Es sollte dem Benutzer angezeigt werden, wenn der
 * Zugriff auf eine Methode zur Steuerung des Roboters diese Ausnahme
 * ausl&ouml;st, da eventuell ein anderes Roblet mit
 * {@link Access#forceAccess()} den Zugriff &uuml;bernommen hat.
 *
 * @see Access
 * @author Daniel Westhoff
 * @version 0.1
 * @since uhh.fbi.tams.mobilerobot 0.4
 */
public class AccessException
    extends MobileRobotException
{
  /** Konstruktor. */
  public AccessException ()
  {
    super ();
  }

  /** Konstruktor. */
  public AccessException (String message)
  {
    super (message);
  }

  /** Konstruktor. */
  public AccessException (Throwable cause)
  {
    super (cause);
  }

  /** Konstruktor. */
  public AccessException (String message, Throwable cause)
  {
    super (message, cause);
  }
}
