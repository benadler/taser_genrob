@echo off

set  cp=genRob.genControl.jar;genRob.genMap.jar
set  dp=genRob.genControl.port=6000
set  ec=genRob.genMap.ModuleImpl

java  -classpath %cp%  -D%dp%  genRob.genControl.Main  %ec%
