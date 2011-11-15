#
# mobile/controller/Makefile
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

static: $(LIBDIR)/libcontroller.a

shared: $(LIBDIR)/libcontroller.so

#######

LIBSRCS = controller.cpp picontroller.cpp pistandardcontroller.cpp

$(LIBDIR)/libcontroller.a: $(patsubst %.cpp, $(OBJDIR)/%.o, $(LIBSRCS))
	$(RM) $@
	$(AR) $@ $^

$(LIBDIR)/libcontroller.so: $(patsubst %.cpp, $(OBJDIR)/%.so, $(LIBSRCS))
	$(CXX) -shared -Wl,-soname,libcontroller.so $^ -o $@
