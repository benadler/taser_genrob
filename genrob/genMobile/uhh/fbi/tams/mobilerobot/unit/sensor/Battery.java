package uhh.fbi.tams.mobilerobot.unit.sensor;

import uhh.fbi.tams.mobilerobot.unit.ProtocolException;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import org.roblet.Unit;

/**
 * Diese Einheit gibt die momentane Spannung der Batterien
 * des Roboters zur&uuml;ck.
 *
 * @author Daniel Westhoff
 * @version 0.2
 * @since uhh.fbi.tams.mobilerobot 0.3.1
 */
public interface Battery
    extends Unit
{
  /**
   * Liefert die momentane Spannung der Batterien des Roboter.
   *
   * @return Spannung in Volt.
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
  public double getVoltage ()
      throws SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;

  /**
   * Liefert den Wert der Batteriespannung, der angibt, wann die Batterie
   * wieder aufgeladen werden sollte. Ein Programm sollte darauf hinweisen,
   * den Roboter wieder aufzuladen.
   *
   * @return Spannungswert unter dem der Roboter wieder aufgeladen werden
   *         sollte.
   */
  public double getLowVoltage ();

  /**
   * Liefert die Minimal zul&auml;ssige Spannung f&uuml;r den Roboter. Ein Programm
   * sollte eine Warnung ausgeben, wenn die Spannung des Roboters unter
   * diesen Wert f&auml;llt. Der Roboter sollte <b>sofort</b> an die
   * Stromversorgung angeschlossen werden.
   *
   * @return Minimal zul&auml;ssiger Wert f&uuml;r die Batteriespannung.
   */
  public double getMinVoltage ();

}
