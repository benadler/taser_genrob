import java.lang.*;
import java.util.ArrayList;
import java.io.Serializable;

import org.roblet.Roblet;
import org.roblet.Robot;

import genRob.genControl.client.*;
import genRob.genPath.unit.*;

// check signatures using: javap -s -classpath . RobletPath

public class RobletPath implements Roblet,Serializable
{
	private static Slot slot;
	private int robotRadius;
	private final Position start, end;

	public static int[] getPath(final int radius, final int startx, final int starty, final int endx, final int endy) throws Exception
	{
		Position start = new Position(startx, starty);
		Position end = new Position(endx, endy);

		final RobletPath roblet = new RobletPath(radius, start, end);

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
						System.out.println("RobletPath::getPath(): Exception while trying to get server slot.");
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
						  Planner.class,
					  }
				  }
				);

			// After adding the listener, wait for a server to come up and run our roblet. When this is
			// done, notify() will be called above and we can exit.
			System.out.println("RobletPath::getPath(): waiting for a genPath-server.");

			synchronized(roblet){roblet.wait();}

			slot = handler._slot;
			sieve.close();
		}

		ArrayList<Position> waypoints = new ArrayList<Position>();

		try
		{
			System.out.println("RobletPath::getPath(): processing waypoint request on server...");
			waypoints = (ArrayList<Position>)slot.run(roblet);
		}
		catch(Exception e)
		{
			System.out.println("RobletPath::getPath(): Exception while trying to run roblet, resetting slot.");
			slot = null;
			e.printStackTrace();
		}

		int[] results = new int[waypoints.size() * 2];

		for(int i=0;i<waypoints.size();i++)
		{
			results[2*i+0] = (Integer)waypoints.get(i).x;
			results[2*i+1] = (Integer)waypoints.get(i).y;
		}
		
		System.out.println("RobletPath::getPath(): retrieved " + waypoints.size() + " waypoints, done.");

		return results;
	}

	RobletPath(final int radius, final Position start, final Position end)
	{
		System.out.println("RobletPath::RobletPath()");
		this.start = start;
		this.end = end;
		this.robotRadius = radius;
	}

	public Object execute(Robot robot)
	{
		//System.out.println("RobletPath::execute()");
		final Planner planner = (Planner)robot.getUnit(Planner.class);

		if(planner == null) throw new RuntimeException("Unit Planner not found.");

		System.out.println(System.currentTimeMillis() / 1000 + " RobletPath::execute(): planning path from " + start.toString() + " to " + end.toString() + ".");

		ArrayList<Position> wayPoints = new ArrayList<Position>();

		try
		{
			Path path = planner.plan(robotRadius, start.x, start.y, end.x, end.y);
			PathIterator it = path.iterator();

			// We return a list INCLUDING start and end point. This way, an EMPTY list is only returned in
			// case of an error. We'll use this knowledge to check for errors lateron (when back in c++).
			System.out.println("\nRobletPath::execute(): Path found.");
			wayPoints.add(start);
// 			System.out.println( + start.x + " / " + start.y);
			while(it.hasNext())
			{
				Position p = it.next();
// 				System.out.println( + p.x + " / " + p.y);
				wayPoints.add(p);
			}
// 			System.out.println( + end.x + " / " + end.y);
			wayPoints.add(end);
			System.out.println(System.currentTimeMillis() / 1000 + " RobletPath::execute(): Roblet returning with " + wayPoints.size() + " waypoints.\n");
		}
		catch(Exception e)
		{
			e.printStackTrace();
			System.out.println("Error, no path available! Returning empty list.");
			return new ArrayList<Position>();
		}

		return wayPoints;
	}
}

