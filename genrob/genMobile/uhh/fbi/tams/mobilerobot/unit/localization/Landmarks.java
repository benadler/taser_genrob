package uhh.fbi.tams.mobilerobot.unit.localization;

import uhh.fbi.tams.mobilerobot.unit.Landmark;
import uhh.fbi.tams.mobilerobot.unit.ProtocolException;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import org.roblet.Unit;

/**
 * Viele Lokalisationsalgorithmen f&uuml;r mobile Roboter arbeiten auf Basis
 * von Landmarken. Die Position einer Landmarke kann sich w&auml;hrend des
 * Betriebs &auml;ndern. Dies geschieht, wenn der Lokalisationsalgorithmus
 * aufgrund der Messungen Anpassungen an den initialen Positionen der
 * Landmarken vornehmen darf. Im Laufe des Betriebs konvergieren die
 * relativen Positionen der Landmarken untereinander, die globalen k&ouml;nnen
 * sich daher &auml;ndern (Stichwort: SLAM).
 *
 * @author Daniel Westhoff
 * @version 0.1
 */
public interface Landmarks
    extends Unit
{
  /**
   * Liefert die gerade von der Lokalisation verwendeten Landmarken.
   * (Dies sind alle beim Start des Roboters angegebenen Landmarken,
   * deren Werte aber vom Lokalisationsalgorithmus an die Ergebnisse
   * von Messungen angepasst worden sein k&ouml;nnen, um der tats&auml;chlich
   * in der Umgebung des Roboters vorgefundenen Situation besser
   * zu entsprechen.)
   *
   * @return Landmark[] Liste aller zur Zeit verwendeter Landmarken.
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
  public Landmark[] getAllMarks ()
      throws SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;


  /**
   * Liefert die initial beim Start des Roboters angegebenen
   * Positionen der Landmarken.
   *
   * @return Landmark[] Initiale Liste der Landmarken.
   *
   * @throws SocketCommunicationException Wird ausgel&ouml;st, wenn bei der
   *                                      Socket-Kommunikation mit der
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      ein Fehler aufgetreten ist.
   * @throws ProtocolException Wird ausgel&ouml;st, wenn ein Antwort-Telegramm
   *                           nicht zul&auml;ssig ist.
   * @throws UnknownCommandException Wird ausgel&ouml;t, wenn das Kommando nicht
   *                                 von der Robotersteuerung <tt>mobiled</tt>
   *                                 implementiert wird.
   */
  public Landmark[] getAllMarksInitial ()
      throws SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;

}
