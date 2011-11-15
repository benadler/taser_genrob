package uhh.fbi.tams.mobilerobot.unit.sensor;

import uhh.fbi.tams.mobilerobot.unit.ProtocolException;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import org.roblet.Unit;

/**
 * Diese Einheit gibt die Temperaturen des linken und rechten
 * Antriebs des Serviceroboters des AB TAMS zur&uuml;ck.
 *
 * <p>Der <i>rechte Antrieb</i> ist rechts, wenn man den Roboter von hinten
 * betrachtet. Der <i>linke Antrieb</i> ist entsprechend links, wenn man den
 * Roboter von hinten betrachtet.
 *
 * @author Daniel Westhoff
 * @version 0.2
 * @since uhh.fbi.tams.mobilerobot 0.3.3
 */
public interface DriveTemperature
    extends Unit
{
  /**
   * Gibt die Temperatur des linken Antriebs des Roboters zur&uuml;ck.
   *
   * @return Temperatur des linken Antriebs in Grad Celsius.
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
  public double getLeftTemperature ()
      throws SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;

  /**
   * Gibt die Temperatur des rechten Antriebs des Roboters zur&uuml;ck.
   *
   * @return Temperatur des rechten Antriebs in Grad Celsius.
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
  public double getRightTemperature ()
      throws SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;

  /**
   * Gibt die Temperaturen der Antriebe zur&uuml;ck. Der erste Wert im
   * zur&uuml;ckgegebenen Feld entspricht der Temperatur des linken
   * Antriebs, das zweite Feld der Temperatur des rechten Antriebs.
   *
   * @return Temperaturen der Antriebe in Grad Celsius.
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
  public double[] getTemperatures ()
      throws SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;
}
