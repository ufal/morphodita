include ../Makefile.common

ifdef WIN
  JAVA_MORPHODITA_LIB=$(call lib,../../src/libmorphodita)
  JAVA_C_FLAGS=-DMORPHODITA_STATIC -I"$(JAVA_HOME)"/include -I"$(JAVA_HOME)"/include/win32
  JAVA_LD_FLAGS=-Wl,--kill-at
  JAVA_MODULE=$(call dynlib,morphodita_java)
else
  JAVA_MORPHODITA_LIB=$(call lib,../../src/libmorphodita.dyn)
  JAVA_C_FLAGS=-I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux
  JAVA_MODULE=$(call dynlib,libmorphodita_java)
endif

TARGETS = $(JAVA_MODULE)

all: $(TARGETS)

include ../Makefile.rules

$(JAVA_MODULE): morphodita_java.cpp $(JAVA_MORPHODITA_LIB)
	@$(if $(JAVA_HOME),,$(error JAVA_HOME must be set))
	$(CXX) -I../../src/include $(DYN_C_FLAGS) $(DYN_LD_FLAGS) $(JAVA_C_FLAGS) $(JAVA_LD_FLAGS) -fno-strict-aliasing -Wl,--version-script=morphodita_java.map $^ -o $@

morphodita_java.cpp: morphodita_java.i ../morphodita.i
	@$(call mkdir,cz/cuni/mff/ufal/morphodita)
	swig $(SWIG_FLAGS) -java -package cz.cuni.mff.ufal.morphodita -outdir cz/cuni/mff/ufal/morphodita -o $@ $<

.PHONY: clean
clean:
	@$(call rmdir,cz) $(call rm,$(TARGETS) $(wildcard *.cpp))
