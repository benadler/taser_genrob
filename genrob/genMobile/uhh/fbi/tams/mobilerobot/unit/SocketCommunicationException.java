package uhh.fbi.tams.mobilerobot.unit;

/**
 * Ausnahme, die geworfen wird, wenn Fehler bei der Kommunikation
 * mit der Robotersteuerung <tt>mobiled</tt> aufgetreten sind.
 *
 * <p><b>Anmerkung:</b> Wenn eine solche Ausnahme auftritt, sollte der Roboter
 * nicht weiter benutzt werden, da Roblet&reg-Server und Robotersteuerung
 * <tt>mobiled</tt> nicht richtig zusammenarbeiten. Vermutlich ist die
 * Verbindung zur Robotersteuerung abgebrochen und die Steuerung muss neu
 * gestartet werden. Dem Nutzer ist ein entsprechender Hinweis anzuzeigen.
 */
public class SocketCommunicationException
    extends CommunicationException
{

  /**
   * Initialisiert eine neue Instanz ohne eine Botschaft.
   */
  public SocketCommunicationException ()
  {
  }


  /**
   * Initialisiert eine neue Instanz mit einer Botschaft.
   * @param strMessage    die Botschaft
   */
  public SocketCommunicationException (String strMessage)
  {
    super (strMessage);
  }


  /**
   * Initialisiert eine neue Instanz mit Anla&szlig;.
   * @param rThrowable    Anla&szlig;ausnahme
   */
  public SocketCommunicationException (Throwable rThrowable)
  {
    super (rThrowable);
  }


  /**
   * Initialisiert eine neue Instanz mit Botschaft und mit Anla&szlig;.
   * @param strMessage    Botschaft
   * @param rThrowable    Anla&szlig;ausnahme
   */
  public SocketCommunicationException (String strMessage, Throwable rThrowable)
  {
    super (strMessage, rThrowable);
  }

}
