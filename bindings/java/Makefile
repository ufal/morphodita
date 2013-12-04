include ../Makefile.common

TARGETS = $(call dynlib,libmorphodita_java)

all: $(TARGETS)

include ../Makefile.rules

$(call dynlib,libmorphodita_java): morphodita_java.cpp $(call lib,../../src/libmorphodita.dyn)
	@$(if $(JAVA_JNI),,$(error JAVA_JNI must be set to the directory containing jni.h))
	$(CXX) -I../../src/include -I$(JAVA_JNI) $(DYN_C_FLAGS) $(DYN_LD_FLAGS) -fno-strict-aliasing -Wl,--version-script=morphodita_java.map $^ -o $@

morphodita_java.cpp: morphodita_java.i
	mkdir -p cz/cuni/mff/ufal/morphodita
	swig $(SWIG_FLAGS) -java -package cz.cuni.mff.ufal.morphodita -outdir cz/cuni/mff/ufal/morphodita -o $@ $<

.PHONY: clean
clean:
	rm -rf $(TARGETS) cz/ *.cpp
