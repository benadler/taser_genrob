import java.lang.*;
import java.io.Serializable;
import java.util.ArrayList;

import org.roblet.Roblet;
import org.roblet.Robot;

import genRob.genControl.client.Client;
import genRob.genControl.client.Slot;
import genRob.genControl.client.Sieve;
import genRob.genControl.client.Server;
import genRob.genControl.client.ServerListener;
import genRob.genMap.unit.basic.Line2;
import genRob.genMap.unit.basic.Lines2;
// import genRob.genMap.unit.basic.HLine;
// import genRob.genMap.unit.basic.HLines;

// check signatures using: javap -s -classpath . RobletObstacles

// WARNING!
// This roblet DIFFERS FROM THE OTHERS! Read the comment in ensureSlotValid()!

public class RobletObstacles implements Roblet,Serializable
{
	private static Slot slot;
	private static RobletObstacles roblet;
	private final Line2[] linesToProcess;
	private final boolean addLines;
	private final int timeout;

	public synchronized static void add(final int[] x, final int[] y, final int timeout) throws Exception
	{
		// create an array of lines from the given parameters.
		final Line2[] tempLineArray = new Line2[x.length];
		for(int i=0; i<x.length; i++)
		{
			tempLineArray[i] = new Line2(x[i]-10, y[i]-10, x[i]+10, y[i]+10);
// 			System.out.println("RobletObstacles::add(): Adding new obstacle at " + x[i] + "/" + y[i] + ".");
		}

		roblet = new RobletObstacles(tempLineArray, true, timeout);

		ensureSlotValid();

		try
		{
			slot.run(roblet);
		}
		catch(Exception e)
		{
			System.out.println("RobletObstacles::add(): Exception while trying to run roblet, resetting slot.");
			slot = null;
			e.printStackTrace();
		}
	}


	public synchronized static void remove(final int[] x, final int[] y) throws Exception
	{
		// create an array of lines from the given parameters.
		final Line2[] tempLineArray = new Line2[x.length];
		for(int i=0; i<x.length; i++)
		{
			tempLineArray[i] = new Line2(x[i]-10, y[i]-10, x[i]+10, y[i]+10);
			System.out.println("RobletObstacles::add(): Removing obstacle at " + x[i] + "/" + y[i] + ".");
		}

		roblet = new RobletObstacles(tempLineArray, false, 0);

		ensureSlotValid();

		try
		{
			slot.run(roblet);
		}
		catch(Exception e)
		{
			System.out.println("RobletObstacles::add(): Exception while trying to run roblet, resetting slot.");
			slot = null;
			e.printStackTrace();
		}
	}

	static void ensureSlotValid() throws Exception
	{
		// This roblet does NOT reuse a server's slot, because after adding obstacles, it remains running
		// on the genmap-server, sleeps, and then removes the obstacles after the timeout has, err, timed
		// out :) If we reused the slot before the timeout, the previous (sleeping) roblet would be killed
		// for the new roblet - the old obstacles would never be deleted, resulting in stale objects in
		// the map.

		// Find a server and assign the slot to it.
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
					System.out.println("RobletObstacles::add(): Exception while trying to get server slot.");
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
					Lines2.class,
				}
			}
		);

		// After adding the listener, wait for a server to come up and run our roblet. When this is
		// done, notify() will be called above and we can exit.
		System.out.println("RobletObstacles::add(): waiting for a genMap-server.");

		synchronized(roblet){roblet.wait();}

		slot = handler._slot;
		sieve.close();
	}

	RobletObstacles(Line2[] linesToProcess, boolean addLines, final int timeout)
	{
		this.linesToProcess = linesToProcess;
		this.timeout = timeout;

		// if true, the given lines will be added. If false, they'll be removed.
		this.addLines = addLines;

		System.out.println("RobletObstacles::RobletObstacles(): number of lines in linesToProcess: " + linesToProcess.length);
	}

	public Object execute(Robot robot)
	{
		final Lines2 lines = (Lines2)robot.getUnit(Lines2.class);

		if(lines == null) throw new RuntimeException("RobletObstacles::execute(): unit Lines not found.");

		Line2[] allLines = lines.list();
// 		System.out.println("RobletObstacles::execute(): mapserver has " + allLines.length + " lines:");

// 		for(int i=0; i<allLines.length; i++)
// 			System.out.println(allLines[i]);

		try
		{
			if(addLines)
			{
				Thread thread = new Thread()
				{
					public void run()
					{
						try
						{
							// add the given lines.
							for(int i=0; i<linesToProcess.length; i++)
								lines.add(linesToProcess[i]);

							System.out.println(System.currentTimeMillis() / 1000 + " RobletObstacles::execute(): " + linesToProcess.length + " lines added, waiting " + timeout / 1000 + " seconds.");

							Thread.sleep(timeout);

							// add the given lines.
							for(int i=0; i<linesToProcess.length; i++)
								lines.remove(linesToProcess[i]);

							System.out.println(System.currentTimeMillis() / 1000 + " RobletObstacles::execute(): " + linesToProcess.length + " lines timed out, removed.");
						}
						catch(Exception e)
						{
							e.printStackTrace();
						}
					}
				};

                		thread.start();
			}
			else
			{
				// delete the given lines.
				for(int i=0; i<linesToProcess.length; i++)
					lines.remove(linesToProcess[i]);
				System.out.println("lines removed.");
			}
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}

		return null;
	}
}

