include ../common/Makefile.common

BINDING_MODULE=$(call dynlib,$(if $(filter win-%,$(PLATFORM)),morphodita_csharp,libmorphodita_csharp))
ifeq ($(PLATFORM),win-gcc)
  BINDING_LD_FLAGS+=-Wl,--kill-at
endif

all: $(BINDING_MODULE)

$(call dynobj,morphodita_csharp):C_FLAGS+=$(BINDING_C_FLAGS)
$(BINDING_MODULE): $(call dynobj,morphodita_csharp) $(BINDING_MORPHODITA_O)
	$(call link_dynlib,$@,$^ $(BINDING_C_FLAGS), $(BINDING_LD_FLAGS) $(call version_script,morphodita_csharp.map))

morphodita_csharp.cpp: morphodita_csharp.i ../common/morphodita.i
	@$(call mkdir,Ufal/MorphoDiTa)
	swig $(SWIG_FLAGS) -csharp -namespace Ufal.MorphoDiTa -outdir $(call platform_name,Ufal/MorphoDiTa) -o $@ $<

.PHONY: clean
clean:
	@$(call rm,.build Ufal $(call all_dynlib,$(BINDING_MODULE)) morphodita_csharp.cpp $(wildcard examples/*.class))
