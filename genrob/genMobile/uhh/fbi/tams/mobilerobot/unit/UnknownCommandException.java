package uhh.fbi.tams.mobilerobot.unit;

/**
 * Diese Ausnahme sollten nur auftreten, wenn die Version
 * der Robotersteuerung <tt>mobiled</tt> und vom Roblet&reg;-Server-Modul
 * <tt>uhh.fbi.tams.mobilerobot</tt> nicht zueinander passen und
 * werden ausgel&ouml;st, wenn ein Kommando in der Robotersteuerung nicht
 * implementiert ist oder ein Kommando dem Roblet&reg;-Server-Modul nicht
 * bekannt ist.
 *
 * <p><b>Anmerkung:</b> Wenn eine solche Ausnahme auftritt, sollte der Roboter
 * nicht weiter benutzt werden, da Roblet&reg-Server und Robotersteuerung
 * <tt>mobiled</tt> nicht richtig zusammenarbeiten. Dem Nutzer ist ein
 * entsprechender Hinweis anzuzeigen.
 */
public class UnknownCommandException
    extends CommunicationException
{

  /**
   * Initialisiert eine neue Instanz ohne eine Botschaft.
   */
  public UnknownCommandException ()
  {
  }


  /**
   * Initialisiert eine neue Instanz mit einer Botschaft.
   * @param strMessage    die Botschaft
   */
  public UnknownCommandException (String strMessage)
  {
    super (strMessage);
  }


  /**
   * Initialisiert eine neue Instanz mit Anla&szlig;.
   * @param rThrowable    Anla&szlig;ausnahme
   */
  public UnknownCommandException (Throwable rThrowable)
  {
    super (rThrowable);
  }


  /**
   * Initialisiert eine neue Instanz mit Botschaft und mit Anla&szlig;.
   * @param strMessage    Botschaft
   * @param rThrowable    Anla&szlig;ausnahme
   */
  public UnknownCommandException (String strMessage, Throwable rThrowable)
  {
    super (strMessage, rThrowable);
  }

}
