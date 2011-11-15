#
#
#

all: libs

include ../Makevars

BINTARGETS = argsTest $(BINDIR)/tee2

ifeq ($(WITHRCCL), yes)
BINTARGETS += 
endif

static: $(LIBDIR)/OBJS.util

shared: $(LIBDIR)/SHOBJS.util

libs: $(OBJDIR) $(LIBDIR) $(LIBS)
	sync

bins: $(OBJDIR) $(BINDIR) $(BINTARGETS)
	sync

install:

clean:
	$(RM) *~ \#*\# *.err gmon.out

veryclean: clean
	$(RM) -R $(OBJDIR)
	$(RM) $(BINTARGETS)

distclean: clean
	$(RM) -R obj.*
	$(RM) $(BINTARGETS)

#######

LIBOBJS = args.o flag2int.o inetutils.o linlist.o memstack.o \
	status.o strfxns.o

ifneq ($(MYARCH), i386-qnx4.25)
LIBOBJS += normal.o
endif

$(LIBDIR)/OBJS.util: $(patsubst %, $(OBJDIR)/%, $(LIBOBJS))
	echo $(patsubst %, util/%, $^) > $@

$(LIBDIR)/SHOBJS.util: $(patsubst %.o, $(OBJDIR)/%.so, $(LIBOBJS))
	echo $(patsubst %, util/%, $^) > $@

#######

argsTest: $(OBJDIR)/argsTest.o $(LIBDIR)/libtesche.a
	$(LD) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BINDIR)/tee2: $(OBJDIR)/tee2.o
	$(LD) $^ -o $@
