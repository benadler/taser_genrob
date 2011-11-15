package uhh.fbi.tams.mobilerobot.unit.sensor;

import uhh.fbi.tams.mobilerobot.unit.Frame2D;
import uhh.fbi.tams.mobilerobot.unit.Laserscan;
import uhh.fbi.tams.mobilerobot.unit.ProtocolException;
import uhh.fbi.tams.mobilerobot.unit.SocketCommunicationException;
import uhh.fbi.tams.mobilerobot.unit.UnknownCommandException;
import org.roblet.Unit;

/**
 * Zugriff auf die Laserscanner des Roboters.
 *
 * @author Daniel Westhoff
 * @version 0.2
 */
public interface Laserscanner
    extends Unit
{
  /**
   * Gibt die Anzahl der verf&uuml;gbaren Laserscanner zur&uuml;ck.
   *
   * @return Anzahl der Laserscanner des Roboters.
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
  public int getNumScanners ()
      throws SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;

  /**
   * Liefert die Position (in [mm]) und Orientierung (in [Robiant])
   * des Scanners in Robotercoordinaten.
   *
   * @param scanner Scanner, der abgefragt werden soll.
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
  public Frame2D getScannerPosition (int scanner)
      throws SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;

  /**
   * Liefert die Abstandsmessungen des Laserscanners beginnend
   * bei 0 Grad bis 181 Grad. Das zur&uuml;ckgegebene Feld sollte
   * 361 Eintr&auml;ge haben, d.h. jede Messung erfolgt mit einem abstand
   * von 0.5 Grad.
   *
   * @param scanner Scanner, der abgefragt werden soll.
   *
   * @return Abstandsmessungen des Laserscanenrs in [<i>mm</i>].
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
  public int[] getScanRadial (int scanner)
      throws SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;

  /**
   * Liefert den aktuellen Scan in Laserscanner-Koordinaten mit dem Ursprung
   * im Laserscanner.
   *
   * @param scanner Scanner, der abgefragt werden soll.
   *
   * @return {@link Laserscan}.
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
  public Laserscan getScanScanner (int scanner)
      throws SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;


  /**
   * Liefert den aktuellen Scan in Roboterkoordinaten mit Ursprung im
   * Robotermittelpunkt.
   *
   * @param scanner Scanner, der abgefragt werden soll.
   *
   * @return {@link Laserscan}.
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
  public Laserscan getScanPlatform (int scanner)
      throws SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;

  /**
   * Liefert den aktuellen Scan in Weltkoordinaten.
   *
   * @param scanner Scanner, der abgefragt werden soll.
   *
   * @return {@link Laserscan}.
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
  public Laserscan getScanWorld (int scanner)
      throws SocketCommunicationException,
             ProtocolException,
             UnknownCommandException;

}
