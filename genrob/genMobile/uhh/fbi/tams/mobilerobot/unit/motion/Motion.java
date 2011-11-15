package uhh.fbi.tams.mobilerobot.unit.motion;

import uhh.fbi.tams.mobilerobot.unit.InternalPathplannerException;
import uhh.fbi.tams.mobilerobot.unit.Pose;
import uhh.fbi.tams.mobilerobot.unit.ProtocolException;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import uhh.fbi.tams.mobilerobot.unit.BrakesOnException;
import uhh.fbi.tams.mobilerobot.unit.AccessException;
import org.roblet.Unit;

/**
 * Diese Einheit bietet Zugriff auf die Bewegungssteuerung des Roboters.
 *
 * @see Brakes
 * @see uhh.fbi.tams.mobilerobot.unit.Access
 * @since uhh.fbi.tams.mobilerobot 0.1
 * @author Daniel Westhoff
 * @version 0.2
 */
public interface Motion
    extends Unit
{

  /**
   * Der Roboter f&auml;hrt auf die &uuml;bergebene Pose. Pfadplanung
   * &uuml;bernimmt die Steuerung auf dem Roboter.
   *
   * @param pose Pose die der Roboter anfahren soll.
   *
   * @see uhh.fbi.tams.mobilerobot.unit.Access
   * @see uhh.fbi.tams.mobilerobot.unit.motion.Brakes
   *
   * @throws AccessException Wird ausgel&ouml;st, wenn das Roblet&reg;
   *                         keine Zugriffsbrechtigung besitzt (siehe
   *                         {@link uhh.fbi.tams.mobilerobot.unit.Access}).
   * @throws BrakesOnException Tritt auf, wenn der Roboter bewegt
   *                           werden sollte, ohne das vorher die Bremsen
   *                           mit der Einheit {@link Brakes} gel&ouml;st
   *                           wurden.
   * @throws InternalPathplannerException Wird ausgel&ouml;st, wenn die
   *                                      Robotersteuerung <tt>mobiled</tt>
   *                                      einen Fehler in der internen
   *                                      Pfadplannung feststellt. Die
   *                                      Ausnahme ist dann eine der
   *                                      abgeleiteten Ausnahmen {@link
   *                                      uhh.fbi.tams.mobilerobot.unit.GoalInObstacleException},
   *                                      {@link uhh.fbi.tams.mobilerobot.unit.StartInObstacleException}
   *                                      oder {@link uhh.fbi.tams.mobilerobot.unit.PathNotFoundException}.
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
  public void move (Pose pose)
      throws AccessException,
      BrakesOnException,
      InternalPathplannerException,
      SocketCommunicationException,
      ProtocolException,
      UnknownCommandException;

  /**
   * Der Roboter h&auml;lt die gerade laufende Bewegung an. Dies geschieht
   * (meistens) durch runterrampen der Geschwindigkeit und es kann daher
   * einen kleinen Moment dauern bis der Roboter wirklich steht. Der
   * Befehl setzt ein Flag in der Robotersteuerung, dass diese stoppen soll,
   * und kehrt sofort wieder zur&uuml;ck.
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
  public void stop ()
      throws AccessException,
      SocketCommunicationException,
      ProtocolException,
      UnknownCommandException;

  /**
   * Der Roboter rotiert, bis seine Orientierung mit dem &uuml;bergebenen
   * Winkel &uuml;bereinstimmt.
   *
   * @param orientation Zielorientierung in [Robiant].
   *
   * @see uhh.fbi.tams.mobilerobot.unit.Access
   * @see uhh.fbi.tams.mobilerobot.unit.motion.Brakes
   *
   * @throws AccessException Wird ausgel&ouml;st, wenn das Roblet&reg;
   *                         keine Zugriffsbrechtigung besitzt (siehe
   *                         {@link uhh.fbi.tams.mobilerobot.unit.Access}).
   * @throws BrakesOnException Tritt auf, wenn der Roboter bewegt
   *                           werden sollte, ohne das vorher die Bremsen
   *                           mit der Einheit {@link Brakes} gel&ouml;st
   *                           wurden.
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
  public void rotate (int orientation)
      throws AccessException,
      BrakesOnException,
      SocketCommunicationException,
      ProtocolException,
      UnknownCommandException;

  /**
   * Der Roboter dreht, bis seine Orientierung mit dem &uuml;bergebenen
   * Winkel &uuml;bereinstimmt, <font color="red"><b> wobei die
   * Kollisionsvermeidung ausgeschaltet ist</b></font>.
   *
   * @param orientation Zielorientierung in [Robiant].
   *
   * @see uhh.fbi.tams.mobilerobot.unit.Access
   * @see uhh.fbi.tams.mobilerobot.unit.motion.Brakes
   *
   * @throws AccessException Wird ausgel&ouml;st, wenn das Roblet&reg;
   *                         keine Zugriffsbrechtigung besitzt (siehe
   *                         {@link uhh.fbi.tams.mobilerobot.unit.Access}).
   * @throws BrakesOnException Tritt auf, wenn der Roboter bewegt
   *                           werden sollte, ohne das vorher die Bremsen
   *                           mit der Einheit {@link Brakes} gel&ouml;st
   *                           wurden.
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
  public void turn (int orientation)
      throws AccessException,
      BrakesOnException,
      SocketCommunicationException,
      ProtocolException,
      UnknownCommandException;

  /**
   * Roboter f&auml;hrt geradeaus auf die &uuml;bergebene Pose, ohne sich
   * am Ende auf die mit der Pose &uuml;bergebene Orientierung zu drehen.
   *
   * @param pose Pose zu der der Roboter fahren soll.
   *
   * @see uhh.fbi.tams.mobilerobot.unit.Access
   * @see uhh.fbi.tams.mobilerobot.unit.motion.Brakes
   *
   * @throws AccessException Wird ausgel&ouml;st, wenn das Roblet&reg;
   *                         keine Zugriffsbrechtigung besitzt (siehe
   *                         {@link uhh.fbi.tams.mobilerobot.unit.Access}).
   * @throws BrakesOnException Tritt auf, wenn der Roboter bewegt
   *                           werden sollte, ohne das vorher die Bremsen
   *                           mit der Einheit {@link Brakes} gel&ouml;st
   *                           wurden.
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
  public void translate (Pose pose)
      throws AccessException,
      BrakesOnException,
      SocketCommunicationException,
      ProtocolException,
      UnknownCommandException;

  /**
   * Roboter f&auml;hrt die &uuml;bergebene Distanz vorw&auml;rts geradeaus,
   * <font color="red"><b>die Kollisionsvermeidung ist dabei ausgeschaltet</b></font>!
   * Dieser Befehl ist daf&uuml;r gedacht kleine Strecken zu fahren, um z.B. an
   * eine Ladestation anzudocken, die in den Kollisionsbereich der
   * Laserscanner hineinragt. Besser {@link #move(Pose)},
   * {@link #translate(Pose)} oder {@link #rotate(int)} verwenden.
   *
   * @param distance Distanz in [mm]
   *                 <b>(Negative Werte drehen die Fahrtrichtung um)</b>.
   *
   * @see uhh.fbi.tams.mobilerobot.unit.Access
   * @see uhh.fbi.tams.mobilerobot.unit.motion.Brakes
   *
   * @throws AccessException Wird ausgel&ouml;st, wenn das Roblet&reg;
   *                         keine Zugriffsbrechtigung besitzt (siehe
   *                         {@link uhh.fbi.tams.mobilerobot.unit.Access}).
   * @throws BrakesOnException Tritt auf, wenn der Roboter bewegt
   *                           werden sollte, ohne das vorher die Bremsen
   *                           mit der Einheit {@link Brakes} gel&ouml;st
   *                           wurden.
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
  public void forward (int distance)
      throws AccessException,
      BrakesOnException,
      SocketCommunicationException,
      ProtocolException,
      UnknownCommandException;

  /**
   * Roboter f&auml;hrt die &uuml;bergebene Distanz r&uuml;ckw&auml;rts
   * geradeaus, <font color="red"><b>die Kollisionsvermeidung ist dabei
   * ausgeschaltet</b></font>! Dieser Befehl ist daf&uuml;r gedacht kleine Strecken zu
   * fahren, um z.B. an eine Ladestation anzudocken, die in den
   * Kollisionsbereich der Laserscanner hineinragt. Besser
   * {@link #move(Pose)}, {@link #translate(Pose)} oder {@link #rotate(int)}
   * verwenden.
   *
   * @param distance Distanz in [mm]
   *                 <b>(Negative Werte drehen die Fahrtrichtung um)</b>.
   *
   * @see uhh.fbi.tams.mobilerobot.unit.Access
   * @see uhh.fbi.tams.mobilerobot.unit.motion.Brakes
   *
   * @throws AccessException Wird ausgel&ouml;st, wenn das Roblet&reg;
   *                         keine Zugriffsbrechtigung besitzt (siehe
   *                         {@link uhh.fbi.tams.mobilerobot.unit.Access}).
   * @throws BrakesOnException Tritt auf, wenn der Roboter bewegt
   *                           werden sollte, ohne das vorher die Bremsen
   *                           mit der Einheit {@link Brakes} gel&ouml;st
   *                           wurden.
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
  public void backward (int distance)
      throws AccessException,
      BrakesOnException,
      SocketCommunicationException,
      ProtocolException,
      UnknownCommandException;

  /**
   * Fragt ab, ob eine Bewegung ausgef&uuml;hrt wird.
   *
   * @return <tt>true</tt>, wenn keine Bewegung mehr ausgef&uuml;hrt wird,
   *         sonst <tt>false</tt>.
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
  public boolean isCompleted ()
      throws SocketCommunicationException,
      ProtocolException,
      UnknownCommandException;

  /**
   * Wartet, solange noch eine Bewegung gefahren wird.
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
  public void waitForCompleted ()
      throws SocketCommunicationException,
      ProtocolException,
      UnknownCommandException;
}
