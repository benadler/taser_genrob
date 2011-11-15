package uhh.fbi.tams.mobilerobot.unit.motion;

import uhh.fbi.tams.mobilerobot.unit.ProtocolException;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import uhh.fbi.tams.mobilerobot.unit.AccessException;
import org.roblet.Unit;

/**
 * Diese Unit erlaubt das Ver&auml;ndern der Fahrgeschwindigkeit des Roboters
 * &uuml;ber eine Skalierung. Ist die Skalierung gleich eins darf der Roboter
 * mit maximaler Geschwindigkeit fahren. Es gibt unterschiedliche
 * Skalierungsfaktoren f&uuml;r die beiden vom Roboter ausf&uuml;hrbaren
 * Bewegungen Rotation und Translation.
 *
 * @see uhh.fbi.tams.mobilerobot.unit.Access
 * @author Daniel Westhoff
 * @version 0.1
 */
public interface Velocity
    extends Unit
{
  /**
   * Gibt die aktuelle Skalierung f&uuml;r die Translation zur&uuml;ck.
   *
   * @return Aktuelle Skalierung.
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
  public float getTranslationScale ()
      throws SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;

  /**
   * Gibt die aktuelle Skalierung f&uuml;r die Rotation zur&uuml;ck.
   *
   * @return Aktuelle Skalierung.
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
  public float getRotationScale ()
      throws SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;

  /**
   * Setzt eine neue Skalierung f&uuml;r die Translation.
   *
   * @param scale Neue Skalierung (0.0 < scale <= 1.0).
   *
   * @throws AccessException Wird ausgel&ouml;st, wenn das Roblet&reg;
   *                         keine Zugriffsbrechtigung besitzt (siehe
   *                         {@link uhh.fbi.tams.mobilerobot.unit.Access})
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
  public void setTranslationScale (float scale)
      throws AccessException,
             SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;

  /**
   * Setzt eine neue Skalierung f&uuml;r die Rotation.
   *
   * @param scale Neue Skalierung (0.0 < scale <= 1.0).
   *
   * @see uhh.fbi.tams.mobilerobot.unit.Access
   *
   * @throws AccessException Wird ausgel&ouml;st, wenn das Roblet&reg;
   *                         keine Zugriffsbrechtigung besitzt (siehe
   *                         {@link uhh.fbi.tams.mobilerobot.unit.Access})
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
  public void setRotationScale (float scale)
      throws AccessException,
             SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;

  /**
   * Ver&auml;ndert die Skalierung um den &uuml;bergebenen Wert.
   *
   * @param changeScale Faktor mit dem die aktuelle Skalierung multipliziert wird.
   *
   * @see uhh.fbi.tams.mobilerobot.unit.Access
   *
   * @throws AccessException Wird ausgel&ouml;st, wenn das Roblet&reg;
   *                         keine Zugriffsbrechtigung besitzt (siehe
   *                         {@link uhh.fbi.tams.mobilerobot.unit.Access})
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
  public void modifyTranslationScale (float changeScale)
      throws AccessException,
             SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;

  /**
   * Ver&auml;ndert die Skalierung um den &uuml;bergebenen Wert.
   *
   * @param changeScale Faktor mit dem die aktuelle Skalierung multipliziert wird.
   *
   * @see uhh.fbi.tams.mobilerobot.unit.Access
   *
   * @throws AccessException Wird ausgel&ouml;st, wenn das Roblet&reg;
   *                         keine Zugriffsbrechtigung besitzt (siehe
   *                         {@link uhh.fbi.tams.mobilerobot.unit.Access})
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
  public void modifyRotationScale (float changeScale)
      throws AccessException,
             SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;
}
