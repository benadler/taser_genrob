package uhh.fbi.tams.mobilerobot.unit.sensor;

import uhh.fbi.tams.mobilerobot.unit.ProtocolException;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import org.roblet.Unit;

/**
 * Mit dieser Einheit k&ouml;nnen die Messwerte der Odometrie auf dem
 * Roboter gespeichert werden. Das Speichern kann &uuml;ber die entsprechenden
 * Methodenaufrufe gestartet und gestoppt werden.
 *
 * @author Daniel Westhoff
 * @version 0.1
 * @since uhh.fbi.tams.mobilerobot 0.2
 */
public interface OdometryLog
    extends Unit
{
  /**
   * Startet das Speichern der Messwerte der Odometrie.
   *
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws UnknownCommandException Wird ausgel&ouml;st, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   */
  public void startLogging ()
      throws SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;

  /**
   * Stoppt das Speichern der Messwerte der Odometrie.
   *
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws UnknownCommandException Wird ausgel&ouml;st, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   */
  public void stopLogging ()
      throws SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;
}
