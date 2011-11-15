@echo off

set  cp=.;..\genRob.genControl.client.jar;..\genRob.genMap.unit.jar

java  -classpath %cp%  %1  localhost:6000  %2  %3  %4
