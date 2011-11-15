#
#
#

all: libs

include ../Makevars

static: $(LIBDIR)/OBJS.math

shared: $(LIBDIR)/SHOBJS.math

libs: $(OBJDIR) $(LIBDIR) $(LIBS)
	sync

BINTARGETS = matSizeTest matLUSpeedTest matLUQualTest matLUMemLeakTest

bins: $(patsubst %, $(BINDIR)/%, $(BINTARGETS))
	sync

clean:
	$(RM) *.o *~ \#*\# gmon.out

veryclean: clean
	$(RM) -R $(OBJDIR)

distclean: clean
	$(RM) -R obj.* $(TARGETS)

#######

LIBOBJS = mat.o matLU.o matManip.o matSize.o vec.o

$(LIBDIR)/OBJS.math: $(patsubst %.o, $(OBJDIR)/%.o, $(LIBOBJS))
	echo $(patsubst %, math/%, $^) > $@

$(LIBDIR)/SHOBJS.math: $(patsubst %.o, $(OBJDIR)/%.so, $(LIBOBJS))
	echo $(patsubst %, math/%, $^) > $@

#######

$(BINDIR)/matSizeTest: $(OBJDIR)/matSizeTest.o $(LIBDIR)/libtesche.a
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BINDIR)/matLUSpeedTest: $(OBJDIR)/matLUSpeedTest.o $(LIBDIR)/libtesche.a
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BINDIR)/matLUQualTest: $(OBJDIR)/matLUQualTest.o $(LIBDIR)/libtesche.a
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BINDIR)/matLUMemLeakTest: $(OBJDIR)/matLUMemLeakTest.o $(LIBDIR)/libtesche.a
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@
