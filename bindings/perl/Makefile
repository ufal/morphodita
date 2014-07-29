include ../common/Makefile.common

ifdef OSX
  BINDING_C_FLAGS+=-Wno-reserved-user-defined-literal
endif
BINDING_C_FLAGS+=-w
BINDING_MODULE=$(call dynlib,auto/Ufal/MorphoDiTa/MorphoDiTa)

all: $(BINDING_MODULE)

include ../common/Makefile.rules

$(BINDING_MODULE): morphodita_perl.cpp $(BINDING_MORPHODITA_LIB)
	@$(if $(PERL_INCLUDE),,$(error PERL_INCLUDE must be set to the directory containing perl.h))
	@$(call mkdir,$(dir $@))
	@$(call touch,$(dir $@)MorphoDiTa.bs)
	$(CXX) -I../../src/include -I$(PERL_INCLUDE) $(DYN_C_FLAGS) $(DYN_LD_FLAGS) $(BINDING_C_FLAGS) $(BINDING_LD_FLAGS) $(call version_script,morphodita_perl.map) $^ -o $@

morphodita_perl.cpp: morphodita_perl.i ../common/morphodita.i
	@$(call mkdir,Ufal)
	swig $(SWIG_FLAGS) -perl -outdir Ufal -o $@ $<

.PHONY: clean
clean:
	@$(call rmdir,Ufal auto) $(call rm,morphodita_perl.cpp)
