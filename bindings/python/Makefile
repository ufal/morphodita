include ../common/Makefile.common

ifdef WIN
  PYTHON_MORPHODITA_LIB=$(call lib,../../src/libmorphodita)
  PYTHON_C_FLAGS=-DMORPHODITA_STATIC
else
  PYTHON_MORPHODITA_LIB=$(call lib,../../src/libmorphodita.dyn)
endif

TARGETS = $(call dynlib,morphodita_python)

all: $(TARGETS)

include ../common/Makefile.rules

$(call dynlib,morphodita_python): morphodita_python.cpp $(PYTHON_MORPHODITA_LIB)
	@$(if $(PYTHON_INCLUDE),,$(error PYTHON_INCLUDE must be set to the directory containing Python.h))
	$(CXX) -I../../src/include -I$(PYTHON_INCLUDE) $(DYN_C_FLAGS) $(DYN_LD_FLAGS) $(PYTHON_C_FLAGS) $(PYTHON_LD_FLAGS) -Wl,--version-script=morphodita_python.map $^ -o $@

morphodita_python.cpp: morphodita_python.i ../common/morphodita.i
	@$(call mkdir,ufal)
	@$(call touch,ufal/__init__.py)
	swig $(SWIG_FLAGS) -python -outdir ufal -o $@ -interface morphodita_python $<

.PHONY: clean
clean:
	@$(call rmdir,ufal) $(call rm,$(TARGETS) $(wildcard *.cpp))
