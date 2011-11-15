@echo off

set  cp=jini-examples.jar;jini-core.jar
set  sp=java.security.policy=browser.policy
set  cb=java.rmi.server.codebase=http://localhost:8080/jini-examples-dl.jar

java  -classpath %cp%  -D%sp%  -D%cb%  com.sun.jini.example.browser.Browser
