include ../common/Makefile.common

BINDING_MODULE=$(call dynlib,auto/Ufal/MorphoDiTa/MorphoDiTa)
BINDING_C_FLAGS+=$(if $(PERL_INCLUDE),,$(error PERL_INCLUDE must be set to the directory containing perl.h))
BINDING_C_FLAGS+=-w $(call include_dir,$(PERL_INCLUDE))
BINDING_C_FLAGS+=$(if $(filter osx-%,$(PLATFORM)),-Wno-reserved-user-defined-literal,)

all: $(BINDING_MODULE)

$(call dynobj,morphodita_perl):C_FLAGS+=$(BINDING_C_FLAGS)
$(BINDING_MODULE): $(call dynobj,morphodita_perl) $(BINDING_MORPHODITA_O)
	@$(call mkdir,$(dir $@))
	@$(call touch,$(dir $@)MorphoDiTa.bs)
	$(call link_dynlib,$@,$^ $(BINDING_C_FLAGS), $(BINDING_LD_FLAGS) $(call version_script,morphodita_perl.map))

morphodita_perl.cpp: morphodita_perl.i ../common/morphodita.i
	@$(call mkdir,Ufal)
	swig $(SWIG_FLAGS) -perl -outdir Ufal -o $@ $<

.PHONY: clean
clean:
	@$(call rm,.build Ufal auto morphodita_perl.cpp)
