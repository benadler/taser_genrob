#
#
#

all: libs

include ../Makevars

CPPFLAGS += -D_REENTRANT

static: $(LIBDIR)/OBJS.thread

shared: $(LIBDIR)/SHOBJS.thread

libs: $(OBJDIR) $(LIBDIR) $(LIBS)
	sync

BINS =

ifeq ($(MYARCH), i586-linux)
BINS = threadTest mutexTest conditionTest fifoTest threadDetachedTest
endif

BINTARGETS = $(patsubst %, $(BINDIR)/%, $(BINS))

bins: $(OBJDIR) $(BINDIR) $(BINDIR) $(BINTARGETS)
	sync

clean:
	$(RM) *~ \#*\# *.err *.orig

veryclean: clean
	$(RM) -R $(OBJDIR) $(BINDIR)

distclean: veryclean
	$(RM) -R bin.* obj.*

#######

LIBOBJS = thread.o mutex.o condition.o fifo.o tracelog.o

ifneq ($(MYARCH), i386-qnx4.25)

#  LIBOBJS += tracelog2.o

endif

$(LIBDIR)/OBJS.thread: $(patsubst %.o, $(OBJDIR)/%.o, $(LIBOBJS))
	echo $(patsubst %, thread/%, $^) > $@

$(LIBDIR)/SHOBJS.thread: $(patsubst %.o, $(OBJDIR)/%.so, $(LIBOBJS))
	echo $(patsubst %, thread/%, $^) -lpthread > $@

#######

$(BINDIR)/threadTest: $(OBJDIR)/threadTest.o $(OBJDIR)/thread.o $(OBJDIR)/tracelog.o
	$(LD) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BINDIR)/mutexTest: $(OBJDIR)/mutexTest.o $(OBJDIR)/mutex.o $(OBJDIR)/thread.o $(OBJDIR)/tracelog.o
	$(LD) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BINDIR)/conditionTest: $(OBJDIR)/conditionTest.o $(OBJDIR)/condition.o $(OBJDIR)/thread.o $(OBJDIR)/tracelog.o
	$(LD) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BINDIR)/fifoTest: $(OBJDIR)/fifoTest.o $(OBJDIR)/fifo.o $(OBJDIR)/condition.o $(OBJDIR)/thread.o $(OBJDIR)/tracelog.o
	$(LD) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BINDIR)/threadDetachedTest: $(OBJDIR)/threadDetachedTest.o \
		$(LIBDIR)/libtesche.a
	$(LD) $(LDFLAGS) $^ $(LDLIBS) -o $@
