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

static: $(LIBDIR)/libmotion.a

shared: $(LIBDIR)/libmotion.so

#######

LIBSRCS = forward.cc motion.cc move.cc rotation.cc translation.cc filter.cc

$(LIBDIR)/libmotion.a: $(patsubst %.cc, $(OBJDIR)/%.o, $(LIBSRCS))
	$(RM) $@
	$(AR) $@ $^

$(LIBDIR)/libmotion.so: $(patsubst %.cc, $(OBJDIR)/%.so, $(LIBSRCS))
	$(CXX) -shared -Wl,-soname,libmotion.so $^ -o $@
