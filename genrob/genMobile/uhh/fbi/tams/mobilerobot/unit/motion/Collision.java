package uhh.fbi.tams.mobilerobot.unit.motion;

import uhh.fbi.tams.mobilerobot.unit.ProtocolException;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import org.roblet.Unit;

/**
 * Diese Einheit liefert Informationen, ob der Roboter durch eine
 * Hindernis blockiert ist.
 *
 * @author Daniel Westhoff
 * @version 0.2
 * @since uhh.fbi.tams.mobilerobot 0.3.5
 */
public interface Collision
    extends Unit
{

  /**
   * Liefert zur&uuml;ck, ob der Roboter blockiert ist und sich daher
   * nicht bewegen kann.
   *
   * @return <tt>true</tt>, wenn der Roboter blockiert ist,
   *         <tt>false</tt> sonst.
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
  public boolean isStalled ()
      throws SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;



  /**
   * Liefert den Radius des Kreises um den Robotermittelpunkt innerhalb
   * dessen ein Collisionsermittlung statfindet. D. h. ein Objekt was
   * innerhalb dieses Radius von den Sensoren des Roboters detektiert
   * wird, f&uuml;hrt dazu, dass der Roboter sofort anh&auml;lt und erst
   * wieder weiterf&auml;hrt, wenn das Objekt entfernt wurde.
   *
   * @return Kollisionsradius in Millimeter [mm]
   */
  public int getCollisionRadius ();

}
