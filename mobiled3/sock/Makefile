#
#
#

all: libs

include ../Makevars

CPPFLAGS += $(UNDEF)DEBUG

static: $(LIBDIR)/OBJS.sock

shared: $(LIBDIR)/SHOBJS.sock

libs: $(OBJDIR) $(LIBDIR) $(LIBS)
	sync

#LOCALBINTARGETS = multicastTest

bins: $(OBJDIR) $(LOCALBINDIR) $(patsubst %, $(LOCALBINDIR)/%, $(LOCALBINTARGETS))
	sync

install:

clean:
	$(RM) *~ \#*\# *.err gmon.out

veryclean: clean
	$(RM) -R $(OBJDIR) $(LOCALBINDIR)

distclean: clean
	$(RM) -R obj.* bin.*

#######

LIBSRCS = daemon.cpp datagramsock.cpp sock.cpp \
	streamsock.cpp # multicastsock.cpp

$(LIBDIR)/OBJS.sock: $(patsubst %.cpp, $(OBJDIR)/%.o, $(LIBSRCS))
	echo $(patsubst %, sock/%, $^) > $@

$(LIBDIR)/SHOBJS.sock: $(patsubst %.cpp, $(OBJDIR)/%.so, $(LIBSRCS))
	echo $(patsubst %, sock/%, $^) > $@

#######

$(LOCALBINDIR)/multicastTest: $(OBJDIR)/multicastTest.o $(LIBDIR)/libtesche.a
	$(LD) $(LDFLAGS) $^ $(LDLIBS) -o $@
