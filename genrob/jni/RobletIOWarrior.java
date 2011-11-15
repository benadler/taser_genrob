import java.lang.*;
import java.io.Serializable;

import org.roblet.Roblet;
import org.roblet.Robot;

import genRob.genControl.client.Client;
import genRob.genControl.client.Slot;
import genRob.genControl.client.Sieve;
import genRob.genControl.client.Server;
import genRob.genControl.client.ServerListener;

import uhh.fbi.tams.iowarrior.unit.IOwarrior;

// check signatures using: javap -s -classpath . RobletIOWarrior

public class RobletIOWarrior implements Roblet,Serializable
{
	private static Slot slot;
	int port;
	boolean enable;

	public synchronized static void setPort(int port, boolean enable) throws Exception
	{
		System.out.println("RobletIOWarrior::setPort(" + port + " " + enable + ").");
		final RobletIOWarrior roblet = new RobletIOWarrior(port, enable);

		if(slot == null)
		{
			// The slot is not defined, find a server and assign the slot to it.
			Client client = new Client();
			Sieve sieve = new Sieve(client);

			class ServerHandler implements ServerListener
			{
				Slot _slot;
				public void addedServer(Server server)
				{
					try
					{
						_slot = server.getSlot();
					}
					catch(Exception e)
					{
						System.out.println("RobletIOWarrior::setPort(): Exception while trying to get server slot.");
						e.printStackTrace();
					}

					synchronized(roblet){roblet.notify();}
				}
				public void removedServer(Server server)
				{
					//System.out.println("ServerListener::removedServer(): Lost server:" + server);
				}
			}
			ServerHandler handler = new ServerHandler();

			sieve.addListener
			(
				handler,
				new Class[][]	// OR
				{
					new Class[]	// AND
					{
						IOwarrior.class,
					}
				}
			);

			// After adding the listener, wait for a server to come up and run our roblet. When this is
			// done, notify() will be called above and we can exit.
			System.out.println("RobletIOWarrior::setPort(): waiting for a genIoWarrior-server.");

			synchronized(roblet){roblet.wait();}

			slot = handler._slot;
			sieve.close();
		}

		try
		{
			slot.run(roblet);
		}
		catch(Exception e)
		{
			System.out.println("RobletIOWarrior::get(): Exception while trying to run roblet, resetting slot.");
			slot = null;
			e.printStackTrace();
		}

		System.out.println("RobletIOWarrior::get(): done, shutting down.");
	}

	RobletIOWarrior(int port, boolean enable)
	{
		this.port = port;
		this.enable = enable;
		System.out.println("RobletIOWarrior::RobletIOWarrior()");
	}

	public Object execute(Robot robot)
	{
		final IOwarrior iowarrior = (IOwarrior)robot.getUnit(IOwarrior.class);

		if(iowarrior == null) throw new RuntimeException("RobletIOWarrior::execute(): unit IOWarrior not found.");
		
		if(port > (iowarrior.getNumberOfPorts() - 1)) throw new RuntimeException("RobletIOWarrior::execute(): port " + port + "doesn't exist, there are only " + iowarrior.getNumberOfPorts() + " ports.");

		try
		{
			iowarrior.setEnabled(port, enable);
			System.out.println("RobletIOWarrior::execute(): successfully set port number " + port + iowarrior.getLabel(port) + ".");
		}
		catch(Exception e)
		{
			System.out.println("RobletIOWarrior::execute(): Exception while trying to set IOwarrior port " + port + " " + iowarrior.getLabel(port) + ".");
			slot = null;
			e.printStackTrace();
		}

		return null;
	}
}

