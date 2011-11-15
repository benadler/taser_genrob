package uhh.fbi.tams.mobilerobot.unit.motion;

import uhh.fbi.tams.mobilerobot.unit.Pose;
import uhh.fbi.tams.mobilerobot.unit.ProtocolException;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import org.roblet.Unit;

/**
 * Diese Einheit gibt die Ziel-Pose zur&uuml;ck, wenn der Roboter gerade eine
 * Bewegung ausf&uuml;hrt.
 *
 * @author Daniel Westhoff
 * @version 0.2
 * @since uhh.fbi.tams.mobilerobot 0.3
 * @see Motion#isCompleted()
 */
public interface Destination
    extends Unit
{
    /**
     * Liefert die Pose zu der sich der Roboter bewegt.
     *
     * @return Pose Pose zu der sich der Roboter bewegt, <tt>null</tt>,
     *              wenn der Roboter keine Bewegung ausf&uuml;hrt.
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
     *
     * @see Motion#isCompleted()
     */
    public Pose getDestination ()
        throws SocketCommunicationException,
               ProtocolException,
               UnknownCommandException;
}
