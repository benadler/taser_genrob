#!/bin/sh

cp=jini-examples.jar:jini-core.jar
sp=java.security.policy=browser.policy
cb=java.rmi.server.codebase=http://localhost:8080/jini-examples-dl.jar

java  -classpath $cp  -D$sp  -D$cb  com.sun.jini.example.browser.Browser