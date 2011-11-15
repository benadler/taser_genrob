@echo off

set  cp=genRob.genControl.jar;genRob.genPath.jar
set  dp=genRob.genControl.port=7000
set  ec=genRob.genPath.ModuleImpl
set  da0=genRob.genPath.algorithm.0=uhh.fbi.tams.pathplanner.Planner

java  -classpath %cp%  -D%dp%  -D%da0%  genRob.genControl.Main  %ec%
