include ../Makefile.common

TARGETS = $(call dynlib,libmorphodita)

all: $(TARGETS)

include ../Makefile.rules

$(call dynlib,libmorphodita): morphodita.c++ $(call lib,../../src/libmorphodita.dyn)
	$(CXX) -I../../src/include -I$(JAVA_JNI) $(DYN_C_FLAGS) $(DYN_LD_FLAGS) -fno-strict-aliasing -Wl,--version-script=libmorphodita.map $^ -o $@

morphodita.c++: ../morphodita.i
	~/.local/bin/swig $(SWIG_FLAGS) -c++ -java $<

.PHONY: clean
clean:
	rm -rf $(TARGETS) *.c++ *.java
