package uhh.fbi.tams.mobilerobot.unit;


/**
 * Diese Ausnahme wird ausgel&ouml;st, wenn ein fehlerhaftes Telegramm
 * empfangen wurde. Fehler k&ouml;nnen z.B. im Status-Byte oder in der Checksumme
 * auftreten.
 *
 * <p><b>Anmerkung:</b> Wenn eine solche Ausnahme auftritt, sollte dem
 * Anwender ein Hinweis auf den Fehler angezeigt werden. Der Roboter
 * kann aber trotzdem weiter verwendet werden. Es sollte versucht werden den
 * fehlgeschlagenen Befehl noch einmal abzusetzen.
 */
public class ProtocolException
    extends CommunicationException
{

  /**
   * Initialisiert eine neue Instanz ohne eine Botschaft.
   */
  public ProtocolException ()
  {
  }


  /**
   * Initialisiert eine neue Instanz mit einer Botschaft.
   * @param strMessage    die Botschaft
   */
  public ProtocolException (String strMessage)
  {
    super (strMessage);
  }


  /**
   * Initialisiert eine neue Instanz mit Anla&szlig;.
   * @param rThrowable    Anla�ausnahme
   */
  public ProtocolException (Throwable rThrowable)
  {
    super (rThrowable);
  }


  /**
   * Initialisiert eine neue Instanz mit Botschaft und mit Anla&szlig;.
   * @param strMessage    Botschaft
   * @param rThrowable    Anla�ausnahme
   */
  public ProtocolException (String strMessage, Throwable rThrowable)
  {
    super (strMessage, rThrowable);
  }

}
