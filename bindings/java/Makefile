include ../common/Makefile.common

ifdef LINUX
  BINDING_MODULE=$(call dynlib,libmorphodita_java)
  BINDING_C_FLAGS+=-I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux
else ifdef WIN
  BINDING_MODULE=$(call dynlib,morphodita_java)
  BINDING_C_FLAGS+=-I"$(JAVA_HOME)"/include -I"$(JAVA_HOME)"/include/win32
  BINDING_LD_FLAGS+=-Wl,--kill-at
else ifdef OSX
  BINDING_MODULE=$(call dynlib,libmorphodita_java)
  BINDING_C_FLAGS+=-I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/darwin $(patsubst %/Home,-I%/Headers,$(filter %/Home,$(JAVA_HOME)))
endif
BINDING_C_FLAGS += -fno-strict-aliasing

all: $(BINDING_MODULE) morphodita.jar

include ../common/Makefile.rules

$(BINDING_MODULE): morphodita_java.cpp $(BINDING_MORPHODITA_LIB)
	@$(if $(JAVA_HOME),,$(error JAVA_HOME must be set))
	$(CXX) -I../../src/include $(DYN_C_FLAGS) $(DYN_LD_FLAGS) $(BINDING_C_FLAGS) $(BINDING_LD_FLAGS) $(call version_script,morphodita_java.map) $^ -o $@

morphodita_java.cpp: morphodita_java.i ../common/morphodita.i
	@$(call mkdir,cz/cuni/mff/ufal/morphodita)
	swig $(SWIG_FLAGS) -java -package cz.cuni.mff.ufal.morphodita -outdir cz/cuni/mff/ufal/morphodita -o $@ $<

morphodita.jar: morphodita_java.cpp
	javac -source 1.4 -Xlint:-options $(wildcard cz/cuni/mff/ufal/morphodita/*.java)
	jar cf $@ $(patsubst %.java,%.class,$(wildcard cz/cuni/mff/ufal/morphodita/*.java))

.PHONY: clean
clean:
	@$(call rmdir,cz) $(call rm,$(BINDING_MODULE) morphodita.jar morphodita_java.cpp $(wildcard examples/*.class))
