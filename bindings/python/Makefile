include ../common/Makefile.common

ifdef LINUX
  PYTHON_MODULE=$(call dynlib,ufal_morphodita)
  PYTHON_MORPHODITA_LIB=$(call lib,../../src/libmorphodita.dyn)
else ifdef WIN
  PYTHON_MODULE=$(call dynlib,ufal_morphodita)
  PYTHON_MORPHODITA_LIB=$(call lib,../../src/libmorphodita)
  PYTHON_C_FLAGS=-DMORPHODITA_STATIC
else ifdef OSX
  PYTHON_MODULE=ufal_morphodita.so
  PYTHON_MORPHODITA_LIB=$(call lib,../../src/libmorphodita)
  PYTHON_LD_FLAGS=-Wl,-undefined -Wl,dynamic_lookup
endif
PYTHON_C_FLAGS+=-w

TARGETS = $(PYTHON_MODULE)

all: $(TARGETS)

include ../common/Makefile.rules

$(PYTHON_MODULE): morphodita_python.cpp $(PYTHON_MORPHODITA_LIB)
	@$(if $(PYTHON_INCLUDE),,$(error PYTHON_INCLUDE must be set to the directory containing Python.h))
	$(CXX) -I../../src/include -I$(PYTHON_INCLUDE) $(DYN_C_FLAGS) $(DYN_LD_FLAGS) $(PYTHON_C_FLAGS) $(PYTHON_LD_FLAGS) $(call version_script,morphodita_python.map) $^ -o $@

morphodita_python.cpp: morphodita_python.i ../common/morphodita.i
	@$(call mkdir,ufal)
	@$(call touch,ufal/__init__.py)
	swig $(SWIG_FLAGS) -python -outdir ufal -o $@ -interface ufal_morphodita -builtin $<

.PHONY: clean
clean:
	@$(call rmdir,ufal) $(call rm,$(TARGETS) morphodita_python.cpp)
