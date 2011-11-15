#!/bin/bash

javac -Xlint:unchecked -classpath ../genControl/org.roblet.jar:../genControl/genRob.genControl.client.jar:../genMap/genRob.genMap.unit.jar RobletObstacles.java

javac -Xlint:unchecked -classpath ../genControl/org.roblet.jar:../genControl/genRob.genControl.client.jar:../genMap/genRob.genMap.unit.jar RobletLaserMarks.java

javac -Xlint:unchecked -classpath ../genControl/org.roblet.jar:../genControl/genRob.genControl.client.jar:../genPath/genRob.genPath.unit.jar RobletPath.java

javac -Xlint:unchecked -classpath ../genControl/org.roblet.jar:../genControl/genRob.genControl.client.jar:../genIoWarrior/uhh.fbi.tams.iowarrior.unit.jar RobletIOWarrior.java
