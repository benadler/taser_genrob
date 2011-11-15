package uhh.fbi.tams.mobilerobot.unit.motion;

import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.ProtocolException;
import uhh.fbi.tams.mobilerobot.unit.AccessException;
import org.roblet.Unit;

/**
 * Mit dieser Einheit lassen sich die Bremsen anziehen und l&ouml;sen sowie
 * deren Zustand &uuml;berpr&uuml;fen.
 *
 * @see Motion
 * @see uhh.fbi.tams.mobilerobot.unit.Access
 * @author Daniel Westhoff
 * @since uhh.fbi.tams.mobilerobot 0.4
 * @version 0.1
 */
public interface Brakes
    extends Unit
{
  /**
   * Zieht die Bremse des Roboters an, der Roboter
   * kann dann <b>keine</b> Bewegung ausf&uuml;hren.
   *
   * @see Motion
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
  public void applyBrakes ()
      throws AccessException,
             SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;

  /**
   * L&ouml;st die Bremse des Roboters, der Roboter
   * kann danach Bewegungen ausf&uuml;hren.
   *
   * @see Motion
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
  public void releaseBrakes ()
      throws AccessException,
             SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;

  /**
   * Gibt <tt>true</tt> zur&uuml;ck, wenn die Bremse des Roboters gel&ouml;st
   * ist, sonst <tt>false</tt>.
   *
   * @return <tt>true</tt>, wenn die Bremse gel&ouml;st ist, sonst <tt>false</tt>
   *
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws UnknownCommandException Wird ausgel&ouml;st, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.   */
  public boolean areBrakesReleased ()
      throws SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;
}
