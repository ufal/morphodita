include ../common/Makefile.common

BINDING_MODULE=$(if $(filter osx-%,$(PLATFORM)),ufal_morphodita.so,$(call dynlib,ufal_morphodita))
BINDING_C_FLAGS+=$(if $(PYTHON_INCLUDE),,$(error PYTHON_INCLUDE must be set to the directory containing Python.h))
BINDING_C_FLAGS+=-w $(call include_dir,$(PYTHON_INCLUDE))

all: $(BINDING_MODULE)

$(call dynobj,morphodita_python):C_FLAGS+=$(BINDING_C_FLAGS)
$(BINDING_MODULE): $(call dynobj,morphodita_python) $(BINDING_MORPHODITA_O)
	$(call link_dynlib,$@,$^ $(BINDING_C_FLAGS), $(BINDING_LD_FLAGS) $(call version_script,morphodita_python.map))

morphodita_python.cpp: morphodita_python.i ../common/morphodita.i
	@$(call mkdir,ufal)
	@$(call echo,>ufal/__init__.py)
	swig $(SWIG_FLAGS) -python -outdir ufal -o $@ -interface ufal_morphodita -builtin $<

.PHONY: clean
clean:
	@$(call rm,.build ufal $(call all_dynlib,ufal_morphodita) morphodita_python.cpp)
