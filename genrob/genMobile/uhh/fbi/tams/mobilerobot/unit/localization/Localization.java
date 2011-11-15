package uhh.fbi.tams.mobilerobot.unit.localization;

import uhh.fbi.tams.mobilerobot.unit.Pose;
import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.ProtocolException;
import org.roblet.Unit;

/**
 * Zugriff auf die Selbst-Lokalisierung des Roboters.
 *
 * @since uhh.fbi.tams.mobilerobot 0.1
 * @author Daniel Westhoff
 * @version 0.2
 */
public interface Localization
    extends Unit
{
  /**
   * Gibt die aktuelle Sch&auml;tzung der Pose des Roboters zur&uuml;ck.
   * Einheiten: [mm] und [Robiant].
   *
   * @return Aktuelle Pose des Roboters.
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
  public Pose getPose ()
      throws SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;
}
