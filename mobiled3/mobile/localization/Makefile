#
# mobile/localization/Makefile
#

include ../../Makevars

CPPFLAGS += -I../../include

libs: $(OBJDIR) $(LIBDIR) $(LIBS)

bins:

clean:
	$(RM) *~ \#*\# *.err gmon.out core

veryclean: clean
	$(RM) -R $(OBJDIR) $(LOCALBINDIR)

distclean: veryclean
	$(RM) -R obj.* bin.*

#######

libs: $(OBJDIR) $(LIBDIR) $(LIBS)

static: $(LIBDIR)/liblocalization.a

shared: $(LIBDIR)/liblocalization.so

#######

LIBSRCS = localization.cc correspondence.cc pose.cc

$(LIBDIR)/liblocalization.a: $(patsubst %.cc, $(OBJDIR)/%.o, $(LIBSRCS))
	$(RM) $@
	$(AR) $@ $^

$(LIBDIR)/liblocalization.so: $(patsubst %.cc, $(OBJDIR)/%.so, $(LIBSRCS))
	$(CXX) -shared -Wl,-soname,liblocalization.so $^ -o $@
