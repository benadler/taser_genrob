#
# mobile/Makefile
#

all: libs bins

include ../Makevars

static: $(LIBDIR)/OBJS.mobile

shared: $(LIBDIR)/SHOBJS.mobile

libs: $(OBJDIR) $(LIBDIR) $(LIBS)
	make -C localisation libs
	make -C localization libs
	make -C motion libs
	make -C controller libs
	make -C pathplanner libs
	sync

BINS = mobiled 

##########################################

ifeq ($(WITHRCCL), yes)

  CPPFLAGS += -I/vol/tirccl/include -DWITH_RCCL
  LDFLAGS += -L/vol/tirccl/lib

  ifneq ($(SIMONLY), yes)

    LDLIBS += -lfts3

  endif

endif

##########################################

BINTARGETS = $(patsubst %, $(BINDIR)/%, $(BINS))

bins: $(OBJDIR) $(BINDIR) $(BINTARGETS)
	make -C localisation bins
	make -C localization bins
	make -C motion bins
	make -C controller bins
	make -C pathplanner bins
	sync

install:

clean:
	$(RM) *~ \#*\# *.err gmon.out core
	$(MAKE) -C motion clean
	$(MAKE) -C localisation clean
	$(MAKE) -C localization clean
	$(MAKE) -C controller clean
	$(MAKE) -C pathplanner clean

veryclean: clean
	$(RM) -R $(OBJDIR) $(LOCALBINDIR)
	$(MAKE) -C motion veryclean
	$(MAKE) -C localisation veryclean
	$(MAKE) -C localization veryclean
	$(MAKE) -C controller veryclean
	$(MAKE) -C pathplanner clean

distclean: veryclean
	$(RM) -R obj.* bin.*
	$(MAKE) -C motion distclean
	$(MAKE) -C localisation distclean
	$(MAKE) -C localization distclean
	$(MAKE) -C controller distclean
	$(MAKE) -C pathplanner veryclean

#######

LIBOBJS = mobile.o mobileExt.o telegram.o

$(LIBDIR)/OBJS.mobile: $(patsubst %.o, $(OBJDIR)/%.o, $(LIBOBJS))
	echo $(patsubst %, mobile/%, $^) > $@

$(LIBDIR)/SHOBJS.mobile: $(patsubst %.o, $(OBJDIR)/%.so, $(LIBOBJS))
	echo $(patsubst %, mobile/%, $^) > $@

####### mobiled

SRCS = main.cc spawner.cc canDongle.cc canPlatform.cc client.cc clientProtocol.cc \
	genBase.cc laser.cc laserFeeder.cc lpt.cc \
	motorFeeder.cc periodicTimer.cc radialscan.cc rs422.cc \
	sicklaser.cc voltageDisplayThread.cc \
	rawlaser.cc sickrawlaser.cc sicksimlaser.cc \
	map.cc

OBJS = $(patsubst %.cc, $(OBJDIR)/%.o, $(SRCS))

$(BINDIR)/mobiled: laserFeeder.h $(OBJS) \
		$(LIBDIR)/libtesche.a \
		$(LIBDIR)/libmotion.a \
		$(LIBDIR)/liblocalisation.a \
		$(LIBDIR)/liblocalization.a \
		$(LIBDIR)/libcontroller.a \
		$(LIBDIR)/libpathplanner.a 
	$(LD) $(LDFLAGS) $(OBJS) $(LDLIBS) \
		-lcontroller \
		$(LIBDIR)/libmotion.a \
		$(LIBDIR)/liblocalisation.a \
		$(LIBDIR)/liblocalization.a \
		$(LIBDIR)/libcontroller.a \
		$(LIBDIR)/libpathplanner.a \
		$(LIBDIR)/libtesche.a \
		-o $@


