#!/bin/sh

cp=".:../org.roblet.jar:../genRob.genControl.client.jar:../genRob.genMap.unit.jar"

javac  -encoding UTF-8  -classpath $cp  *.java
