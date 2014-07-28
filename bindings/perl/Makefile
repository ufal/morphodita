include ../common/Makefile.common

ifdef LINUX
  PERL_MORPHODITA_LIB=$(call lib,../../src/libmorphodita.dyn)
else ifdef WIN
  PERL_MORPHODITA_LIB=$(call lib,../../src/libmorphodita)
  PERL_C_FLAGS=-DMORPHODITA_STATIC
else ifdef OSX
  PERL_MORPHODITA_LIB=$(call lib,../../src/libmorphodita)
  PERL_C_FLAGS=-Wno-reserved-user-defined-literal
  PERL_LD_FLAGS=-Wl,-undefined -Wl,dynamic_lookup
endif
PERL_C_FLAGS+=-w

TARGETS = $(call dynlib,auto/Ufal/MorphoDiTa/MorphoDiTa)

all: $(TARGETS)

include ../common/Makefile.rules

$(call dynlib,auto/Ufal/MorphoDiTa/MorphoDiTa): morphodita_perl.cpp $(PERL_MORPHODITA_LIB)
	@$(if $(PERL_INCLUDE),,$(error PERL_INCLUDE must be set to the directory containing perl.h))
	@$(call mkdir,$(dir $@))
	@$(call touch,$(dir $@)MorphoDiTa.bs)
	$(CXX) -I../../src/include -I$(PERL_INCLUDE) $(DYN_C_FLAGS) $(DYN_LD_FLAGS) $(PERL_C_FLAGS) $(PERL_LD_FLAGS) $(call version_script,morphodita_perl.map) $^ -o $@

morphodita_perl.cpp: morphodita_perl.i ../common/morphodita.i
	@$(call mkdir,Ufal)
	swig $(SWIG_FLAGS) -perl -outdir Ufal -o $@ $<

.PHONY: clean
clean:
	@$(call rmdir,Ufal auto) $(call rm,morphodita_perl.cpp)
