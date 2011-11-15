package uhh.fbi.tams.mobilerobot;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

import uhh.fbi.tams.mobilerobot.unit.*;

/**
 * Socket-Verbindung für das Bin&auml;rprotokoll zur Robotersteuerung
 * <tt>mobiled</tt>.
 *
 * @since uhh.fbi.tams.mobilerobot 0.1
 * @author Hagen Stanek, Daniel Westhoff
 * @version 0.2
 */
class BinarySocket
{
  /** Log. */
  private ModuleLog out;

  /** Socket-Verbindung. */
  private Socket socket;

  /** Output-Stream. */
  private OutputStream outputStream;

  /** Input-Stream. */
  private InputStream inputStream;


  /**
   * Konstruktor.
   *
   * @param host Rechner auf dem die Robotersteuerung <tt>mobiled</tt>
   *             l&auml;uft.
   * @param port Port der Robotersteuerung <tt>mobiled</tt>.
   * @param out Logging.
   *
   * @throws MobileRobotException Wenn ein Fehler beim Aufbau der Verbindung
   *                              zur Robotersteuerung <tt>mobiled</tt>
   *                              aufgetreten ist.
   * */
  BinarySocket (String host,
                int port,
                ModuleLog out)
      throws MobileRobotException
  {
    this.out = out;

    try
    {
      socket = new Socket (host, port);
    }
    catch (IOException e)
    {
      throw new MobileRobotException
          ("W\u00E4rend das Modul initialisiert wurde, "
           + "ist ein Fehler aufgetreten: Verbindung mit"
           + "der Robotersteuerung 'mobiled' auf "
           + host
           + ":"
           + port
           + " ist fehlgeschlagen.",
           e);
    }

    try
    {
      outputStream = socket.getOutputStream ();
      inputStream = socket.getInputStream ();
    }
    catch (Exception e)
    {
      throw new MobileRobotException
          ("W\u00E4rend das Modul initialisiert wurde, "
           + "ist ein Fehler aufgetreten: Streams \u00FCber die "
           + "Socket-Verbindung konnten nicht initialisiert werden.",
           e);
    }
  }

  /** Socket schließen. */
  void close ()
  {
    try
    {
      socket.close ();
    }
    catch (java.io.IOException e)
    {
      if (out.module)
      {
        out.module (this, e);
      }
    }
  }


  /**
   * Anfrage an die Robotersteuerung <tt>mobiled</tt> senden und
   * Antwort empfangen.
   */
  synchronized TelegramReply send (TelegramRequest request)
      throws SocketCommunicationException
  {
    // send
    request.writeTo (outputStream);

    // receive
    TelegramReply reply = new TelegramReply (out);
    reply.readFrom (inputStream);

    return reply;
  }

}
