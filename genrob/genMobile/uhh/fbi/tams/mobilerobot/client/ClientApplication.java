package uhh.fbi.tams.mobilerobot.client;

import java.rmi.Remote;
import java.rmi.RemoteException;

/** Schnittstelle zur Client-Anwendung, die vom Roblet aufgerufen wird. */
interface ClientApplication
    extends Remote
{
  /** Ping. */
  public void ping ()
      throws RemoteException;
}
