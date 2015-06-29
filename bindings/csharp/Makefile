include ../common/Makefile.common

ifdef LINUX
  BINDING_MODULE=$(call dynlib,libmorphodita_csharp)
else ifdef WIN
  BINDING_MODULE=$(call dynlib,morphodita_csharp)
  BINDING_LD_FLAGS+=-Wl,--kill-at
else ifdef OSX
  BINDING_MODULE=$(call dynlib,libmorphodita_csharp)
endif
BINDING_C_FLAGS += -fno-strict-aliasing

all: $(BINDING_MODULE)

include ../common/Makefile.rules

$(BINDING_MODULE): morphodita_csharp.cpp $(BINDING_MORPHODITA_LIB)
	$(CXX) -I../../src/include $(DYN_C_FLAGS) $(DYN_LD_FLAGS) $(BINDING_C_FLAGS) $(BINDING_LD_FLAGS) $(call version_script,morphodita_csharp.map) $^ -o $@

morphodita_csharp.cpp: morphodita_csharp.i ../common/morphodita.i
	@$(call mkdir,MorphoDiTa)
	swig $(SWIG_FLAGS) -csharp -namespace Ufal.MorphoDiTa -outdir MorphoDiTa -o $@ $<

.PHONY: clean
clean:
	@$(call rmdir,MorphoDiTa) $(call rm,$(BINDING_MODULE) morphodita_csharp.cpp $(wildcard examples/*.class))
