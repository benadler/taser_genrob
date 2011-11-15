package uhh.fbi.tams.mobilerobot.client;

import java.io.Serializable;

/** Ausnahme, die beim Zugriff auf den mobilen Roboter ausgel&ouml;st werden kann. */
public class ClientException
    extends Exception
{
  /** Konstruktor. */
  public ClientException ()
  {
    super ();
  }

  /** Konstruktor. */
  public ClientException (String message)
  {
    super (message);
  }

  /** Konstruktor. */
  public ClientException (String message, Throwable cause)
  {
    super (message, cause);
  }

  /** Konstruktor. */
  public ClientException (Throwable cause)
  {
    super (cause);
  }
}
