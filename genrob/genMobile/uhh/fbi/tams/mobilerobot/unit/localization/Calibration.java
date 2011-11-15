package uhh.fbi.tams.mobilerobot.unit.localization;

import uhh.fbi.tams.mobilerobot.unit.Pose;
import uhh.fbi.tams.mobilerobot.unit.ProtocolException;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import uhh.fbi.tams.mobilerobot.unit.AccessException;
import org.roblet.Unit;

/**
 * Erlaubt das Setzen der Pose (Position in [mm] und Orientierungin [Robiant])
 * des Roboters innerhalb der Lokalisation.
 *
 * @see uhh.fbi.tams.mobilerobot.unit.Access
 * @author Daniel Westhoff
 * @version 0.2
 */
public interface Calibration
    extends Unit
{
  /**
   * Setzt in der Lokalisation den Roboter auf die &uuml;bergeben Pose.
   *
   * @param pose Pose neue Pose ([mm],[mm],[Robiant])
   *
   * @see uhh.fbi.tams.mobilerobot.unit.Access
   *
   * @throws AccessException Wird ausgel&ouml;st, wenn das Roblet&reg;
   *                         keine Zugriffsbrechtigung besitzt (siehe
   *                         {@link uhh.fbi.tams.mobilerobot.unit.Access}).
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
  public void setPose (Pose pose)
      throws AccessException,
             SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;
}
