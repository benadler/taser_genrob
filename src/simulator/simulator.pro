######################################################################
# Automatically generated by qmake (2.01a) Wed Oct 17 17:26:01 2007
######################################################################

TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += . ../ ../mobiled/
CONFIG += qt
QT += opengl network

# the splines have some fucked-up dependencies, because they're a motion, need localization, need...
# for platine-linux26
INCLUDEPATH += /opt/sun-jdk-1.6.0.03/include/ /opt/sun-jdk-1.6.0.03/include/linux/
LIBPATH += /opt/sun-jdk-1.6.0.03/lib/ /opt/sun-jdk-1.6.0.03/jre/lib/amd64/server/ /opt/sun-jre-bin-1.6.0.24/lib/amd64/server/


# for tams61-linux24
INCLUDEPATH += /usr/lib/java/include/ /usr/lib/java/include/linux/
LIBPATH += /usr/lib/java/lib/ /usr/lib/java/jre/lib/i386/server/

# for tams61-linux26
INCLUDEPATH += /usr/lib/jvm/java-1.6.0-sun-1.6.0/include/ /usr/lib/jvm/java-1.6.0-sun-1.6.0/include/linux/
LIBPATH += /usr/lib/jvm/java-1.6.0-sun-1.6.0/lib /usr/lib/jvm/java-1.6.0-sun-1.6.0/jre/lib/i386/server/ /usr/lib64/jvm/java-1.6.0-sun-1.6.0/jre/lib/amd64/server/

LIBS += -ljvm

# Input

FORMS += robotcontrol.ui

HEADERS +=	simulator.h \
		logwidget.h \
		robotcontrols.h \
		robotview.h \
		robotscene.h \
		../packet.h \
		../logger.h \
		../configuration.h \
		../conversion.h \
		../pose.h \
		robotitem.h \
		../motion.h \
		../polynom.h \
		../spline.h \
		../splinecubic.h \
		../splinehermite.h \
		../splinecosinus.h \
		../lasermark.h \
		../mobiled/java.h \
		../mobiled/localization.h \
		../mobiled/correspondence.h

SOURCES +=	simulator.cpp \
		logwidget.cpp \
		robotcontrols.cpp \
		robotview.cpp \
		robotscene.cpp \
		../packet.cpp \
		../logger.cpp \
		../configuration.cpp \
		../conversion.cpp \
		../pose.cpp \
		robotitem.cpp \
		../motion.cpp \
		../polynom.cpp \
		../spline.cpp \
		../splinecubic.cpp \
		../splinehermite.cpp \
		../splinecosinus.cpp \
		../lasermark.cpp \
		../mobiled/java.cpp \
		../mobiled/localization.cpp \
		../mobiled/correspondence.cpp