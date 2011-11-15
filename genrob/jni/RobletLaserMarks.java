import java.lang.*;
import java.io.Serializable;

import org.roblet.Roblet;
import org.roblet.Robot;

import genRob.genControl.client.Client;
import genRob.genControl.client.Slot;
import genRob.genControl.client.Sieve;
import genRob.genControl.client.Server;
import genRob.genControl.client.ServerListener;
import genRob.genMap.unit.basic.Reflector;
import genRob.genMap.unit.basic.Reflectors;

// check signatures using: javap -s -classpath . RobletLaserMarks

public class RobletLaserMarks implements Roblet,Serializable
{
	private static Slot slot;

	public synchronized static int[] get() throws Exception
	{
		final RobletLaserMarks roblet = new RobletLaserMarks();

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
						System.out.println("RobletLaserMarks::get(): Exception while trying to get server slot.");
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
						Reflectors.class,
					}
				}
			);

			// After adding the listener, wait for a server to come up and run our roblet. When this is
			// done, notify() will be called above and we can exit.
			System.out.println("RobletLaserMarks::get(): waiting for a genMap-server.");

			synchronized(roblet){roblet.wait();}

			slot = handler._slot;
			sieve.close();
		}

		Reflector[] reflectors = new Reflector[0];

		try
		{
			reflectors = (Reflector[])slot.run(roblet);
		}
		catch(Exception e)
		{
			System.out.println("RobletLaserMarks::get(): Exception while trying to run roblet, resetting slot.");
			slot = null;
			e.printStackTrace();
		}

		// we need two coordinates for each reflector.
		int[] results = new int[reflectors.length * 2];

		for(int i=0;i<reflectors.length;i++)
		{
			results[2*i+0] = reflectors[i].x;
			results[2*i+1] = reflectors[i].y;
		}

		System.out.println("RobletLaserMarks::get(): done, returning " + reflectors.length + " lasermarks.");

		return results;
	}

	RobletLaserMarks()
	{
		System.out.println("RobletLaserMarks::RobletLaserMarks()");
	}

	public Object execute(Robot robot)
	{
		final Reflectors reflectors = (Reflectors)robot.getUnit(Reflectors.class);

		if(reflectors == null) throw new RuntimeException("RobletLaserMarks::execute(): unit Reflectors not found.");

		final Reflector[] reflectorArray = reflectors.list();

		System.out.println("RobletLaserMarks::execute(): " + reflectorArray.length + " Lasermarks found, returning them.");

		return reflectorArray;
	}
}

