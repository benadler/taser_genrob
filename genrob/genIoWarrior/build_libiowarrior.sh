g++ -c -I./ -I/usr/lib/SunJava2-1.4.2/include -I/usr/lib/SunJava2-1.4.2/include/linux -I/usr/src/modules-robot/iowarrior -o jiowarrior.o jiowarrior.c

g++ -o libiowarrior.so -shared  jiowarrior.o
