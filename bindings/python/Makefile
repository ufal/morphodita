include ../common/Makefile.common

ifdef OSX
  BINDING_MODULE=ufal_morphodita.so
else
  BINDING_MODULE=$(call dynlib,ufal_morphodita)
endif
BINDING_C_FLAGS+=-w

all: $(BINDING_MODULE)

include ../common/Makefile.rules

$(BINDING_MODULE): morphodita_python.cpp $(BINDING_MORPHODITA_LIB)
	@$(if $(PYTHON_INCLUDE),,$(error PYTHON_INCLUDE must be set to the directory containing Python.h))
	$(CXX) -I../../src/include -I$(PYTHON_INCLUDE) $(DYN_C_FLAGS) $(DYN_LD_FLAGS) $(BINDING_C_FLAGS) $(BINDING_LD_FLAGS) $(call version_script,morphodita_python.map) $^ -o $@

morphodita_python.cpp: morphodita_python.i ../common/morphodita.i
	@$(call mkdir,ufal)
	@$(call touch,ufal/__init__.py)
	swig $(SWIG_FLAGS) -python -outdir ufal -o $@ -interface ufal_morphodita -builtin $<

.PHONY: clean
clean:
	@$(call rmdir,ufal) $(call rm,$(BINDING_MODULE) morphodita_python.cpp)
