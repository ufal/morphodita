# This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
#
# Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
# Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

include ../common/Makefile.common

BINDING_MODULE=$(call dynlib,$(if $(filter win-%,$(PLATFORM)),morphodita_java,libmorphodita_java))
BINDING_C_FLAGS+=$(if $(JAVA_HOME),,$(error JAVA_HOME must be set))
BINDING_C_FLAGS+=$(if $(filter %-gcc %-clang,$(PLATFORM)),-fno-strict-aliasing)
ifneq ($(filter linux-%,$(PLATFORM)),)
  BINDING_C_FLAGS+=$(call include_dir,$(JAVA_HOME)/include $(JAVA_HOME)/include/linux)
else ifneq ($(filter win-%,$(PLATFORM)),)
  BINDING_C_FLAGS+=$(call include_dir,"$(JAVA_HOME)"/include "$(JAVA_HOME)"/include/win32)
  ifeq ($(PLATFORM),win-gcc)
    BINDING_LD_FLAGS+=-Wl,--kill-at
  endif
else ifneq ($(filter osx-%,$(PLATFORM)),)
  BINDING_C_FLAGS+=$(call include_dir,$(JAVA_HOME)/include $(JAVA_HOME)/include/darwin $(patsubst %/Home,%/Headers,$(filter %/Home,$(JAVA_HOME))))
endif

all: $(BINDING_MODULE) morphodita.jar

$(call dynobj,morphodita_java):C_FLAGS+=$(BINDING_C_FLAGS)
$(BINDING_MODULE): $(call dynobj,morphodita_java) $(BINDING_MORPHODITA_OBJECTS)
	$(call link_dynlib,$@,$^ $(BINDING_C_FLAGS), $(BINDING_LD_FLAGS) $(call version_script,morphodita_java.map))

morphodita_java.cpp: morphodita_java.i ../common/morphodita.i
	@$(call mkdir,cz/cuni/mff/ufal/morphodita)
	swig $(SWIG_FLAGS) -java -package cz.cuni.mff.ufal.morphodita -outdir $(call platform_name,cz/cuni/mff/ufal/morphodita) -o $@ $<

morphodita.jar: morphodita_java.cpp
	javac -source 1.4 -Xlint:-options $(wildcard cz/cuni/mff/ufal/morphodita/*.java)
	jar cf $@ $(patsubst %.java,%.class,$(wildcard cz/cuni/mff/ufal/morphodita/*.java))

.PHONY: clean
clean:
	@$(call rm,.build cz $(call all_dynlib,morphodita_java libmorphodita_java) morphodita.jar morphodita_java.cpp $(wildcard examples/*.class))
