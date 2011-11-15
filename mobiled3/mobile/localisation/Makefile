#
# mobile/localisation/Makefile
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

static: $(LIBDIR)/liblocalisation.a

shared: $(LIBDIR)/liblocalisation.so

#######

LIBSRCS = localisation.cpp ekf.cpp systemmodel.cpp measurementmodel.cpp \
	genbasesystemmodel.cpp genbasemeasurementmodel.cpp

$(LIBDIR)/liblocalisation.a: $(patsubst %.cpp, $(OBJDIR)/%.o, $(LIBSRCS))
	$(RM) $@
	$(AR) $@ $^

$(LIBDIR)/liblocalisation.so: $(patsubst %.cpp, $(OBJDIR)/%.so, $(LIBSRCS))
	$(CXX) -shared -Wl,-soname,liblocalisation.so $^ -o $@
