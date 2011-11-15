#
#
#

all: libs

include ../Makevars

CPPFLAGS += $(UNDEF)DEBUG

static: $(LIBDIR)/OBJS.tty

shared: $(LIBDIR)/SHOBJS.tty

libs: $(OBJDIR) $(LIBDIR) $(LIBS)
	sync

LOCALBINTARGETS = kbdTest ttysTest mouseTest

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

LIBOBJS = kbd.o mouse.o tty.o ttys.o

$(LIBDIR)/OBJS.tty: $(patsubst %.o, $(OBJDIR)/%.o, $(LIBOBJS))
	echo $(patsubst %, tty/%, $^) > $@

$(LIBDIR)/SHOBJS.tty: $(patsubst %.o, $(OBJDIR)/%.so, $(LIBOBJS))
	echo $(patsubst %, tty/%, $^) > $@

#######

$(LOCALBINDIR)/kbdTest: $(OBJDIR)/kbdTest.o $(LIBDIR)/libtesche.a
	$(LD) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(LOCALBINDIR)/ttysTest: $(OBJDIR)/ttysTest.o $(LIBDIR)/libtesche.a
	$(LD) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(LOCALBINDIR)/mouseTest: $(OBJDIR)/mouseTest.o $(LIBDIR)/libtesche.a
	$(LD) $(LDFLAGS) $^ $(LDLIBS) -o $@
