# This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
#
# Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
# Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

include Makefile.builtem
include Makefile.include
include rest_server/microrestd/Makefile.include

EXECUTABLES = $(call exe,encode_derivator encode_dictionary run_morpho_analyze run_morpho_cli run_morpho_generate run_tagger run_tokenizer tagger_accuracy train_guesser train_tagger)
SERVER = $(call exe,rest_server/morphodita_server)
TOOLS = $(call exe,tools/benchmark_dictionary tools/csts tools/fill_lemmas tools/morpho_accuracy tools/remap_tags tools/strip_combining_marks tools/test_dictionary)
LIBRARIES = $(call lib,libmorphodita)

.PHONY: all exe server tools lib full
all: exe
exe: $(EXECUTABLES)
server: $(SERVER)
tools: $(TOOLS)
lib: $(LIBRARIES)
full: exe server tools lib

# libraries
$(call lib,libmorphodita): $(call obj,$(MORPHODITA_OBJECTS))
	$(call link_lib,$@,$^)

C_FLAGS += $(call include_dir,.)
# executables
$(call exe,encode_derivator): $(call obj,derivator/derivator_dictionary_encoder utils/compressor_save $(MORPHODITA_OBJECTS))
$(call exe,encode_dictionary): $(call obj,morpho/czech_morpho_encoder morpho/english_morpho_encoder morpho/english_morpho_guesser_encoder morpho/external_morpho_encoder morpho/generic_morpho_encoder morpho/morpho_prefix_guesser_encoder morpho/morpho_statistical_guesser_encoder morpho/raw_morpho_dictionary_reader unilib/version utils/compressor_save version/version)
$(call exe,run_morpho_analyze): $(call obj,$(MORPHODITA_OBJECTS))
$(call exe,run_morpho_cli): $(call obj,$(MORPHODITA_OBJECTS))
$(call exe,run_morpho_generate): $(call obj,$(MORPHODITA_OBJECTS))
$(call exe,run_tagger): $(call obj,$(MORPHODITA_OBJECTS))
$(call exe,run_tokenizer): $(call obj,$(MORPHODITA_OBJECTS))
$(call exe,tagger_accuracy): $(call obj,$(MORPHODITA_OBJECTS))
$(call exe,train_guesser): $(call obj,morpho/morpho_statistical_guesser_trainer utils/compressor_save unilib/unicode unilib/utf8 unilib/version version/version)
$(call exe,train_tagger): $(call obj,$(MORPHODITA_OBJECTS) utils/compressor_save)
$(call exe,rest_server/morphodita_server): LD_FLAGS+=$(call use_library,$(if $(filter win-%,$(PLATFORM)),$(MICRORESTD_LIBRARIES_WIN),$(MICRORESTD_LIBRARIES_POSIX)))
$(call exe,rest_server/morphodita_server): $(call obj,$(MORPHODITA_OBJECTS) rest_server/morphodita_service $(addprefix rest_server/microrestd/,$(MICRORESTD_OBJECTS) $(MICRORESTD_PUGIXML_OBJECTS)))
$(call exe,tools/benchmark_dictionary): $(call obj,$(MORPHODITA_OBJECTS))
$(call exe,tools/csts): $(call obj,$(MORPHODITA_OBJECTS))
$(call exe,tools/fill_lemmas): $(call obj,$(MORPHODITA_OBJECTS))
$(call exe,tools/morpho_accuracy): $(call obj,$(MORPHODITA_OBJECTS))
$(call exe,tools/remap_tags): $(call obj,$(MORPHODITA_OBJECTS))
$(call exe,tools/strip_combining_marks): $(call obj,unilib/utf8 unilib/unistrip unilib/version version/version)
$(call exe,tools/test_dictionary): $(call obj,$(MORPHODITA_OBJECTS) morpho/raw_morpho_dictionary_reader)
$(EXECUTABLES) $(SERVER) $(TOOLS):$(call exe,%): $$(call obj,% utils/options)
	$(call link_exe,$@,$^,$(call win_subsystem,console))

# cleaning
.PHONY: clean
clean:
	@$(call rm,.build $(call all_exe,$(EXECUTABLES) $(SERVER) $(TOOLS)) $(call all_lib,$(LIBRARIES)))

# dump library sources
.PHONY: lib_sources
lib_sources:
	@echo $(patsubst %,%.cpp,$(MORPHODITA_OBJECTS))
