# This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
#
# Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
# Mathematics and Physics, Charles University in Prague, Czech Republic.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

include Makefile.builtem
include Makefile.lib

EXECUTABLES = $(call exe,encode_dictionary run_morpho_analyze run_morpho_cli run_morpho_generate run_tagger run_tokenizer tagger_accuracy train_tagger)
LIBRARIES = $(call lib,libmorphodita)
TOOLS = $(call exe,tools/benchmark_dictionary tools/csts tools/fill_lemmas tools/morpho_accuracy tools/remap_tags tools/strip_combining_marks tools/test_dictionary)

.PHONY: all exe lib tools full
all: exe
exe: $(EXECUTABLES)
lib: $(LIBRARIES)
tools: $(TOOLS)
full: exe lib tools

# libraries
$(call lib,libmorphodita): $(call obj,$(MORPHODITA_O))
	$(call link_lib,$@,$^)

C_FLAGS += $(call include_dir,.)
# executables
$(call exe,encode_dictionary): $(call obj,$(LZMA_ENC_O) morpho/czech_morpho_encoder morpho/english_morpho_encoder morpho/english_morpho_guesser_encoder morpho/external_morpho_encoder morpho/generic_morpho_encoder morpho/morpho_prefix_guesser_encoder morpho/morpho_statistical_guesser_encoder morpho/raw_morpho_dictionary_reader utils/input utils/parse_options version/version)
$(call exe,run_morpho_analyze): $(call obj,$(MORPHODITA_O) utils/input utils/parse_options)
$(call exe,run_morpho_cli): $(call obj,$(MORPHODITA_O) utils/input utils/parse_options)
$(call exe,run_morpho_generate): $(call obj,$(MORPHODITA_O) utils/input utils/parse_options)
$(call exe,run_tagger): $(call obj,$(MORPHODITA_O) utils/input utils/parse_options)
$(call exe,run_tokenizer): $(call obj,$(MORPHODITA_O) utils/input utils/parse_options)
$(call exe,tagger_accuracy): $(call obj,$(MORPHODITA_O) utils/input utils/parse_options)
$(call exe,train_tagger): $(call obj,$(LZMA_ENC_O) $(MORPHODITA_O) utils/input utils/parse_options)
$(call exe,tools/benchmark_dictionary): $(call obj,$(MORPHO_O) utils/input)
$(call exe,tools/csts): $(call obj,$(MORPHO_O) utils/input)
$(call exe,tools/fill_lemmas): $(call obj,$(MORPHO_O) utils/input)
$(call exe,tools/morpho_accuracy): $(call obj,$(MORPHO_O) utils/input)
$(call exe,tools/remap_tags): $(call obj,$(MORPHO_O) utils/input)
$(call exe,tools/strip_combining_marks): $(call obj,unilib/utf8 unilib/unistrip utils/input utils/parse_options version/version)
$(call exe,tools/test_dictionary): $(call obj,$(MORPHO_O) morpho/raw_morpho_dictionary_reader utils/input)
$(EXECUTABLES) $(TOOLS):$(call exe,%): $$(call obj,% utils/iostreams)
	$(call link_exe,$@,$^,$(call win_subsystem,console))

# cleaning
.PHONY: clean
clean:
	@$(call rm,.build $(call all_exe,$(EXECUTABLES) $(TOOLS)) $(call all_lib,$(LIBRARIES)))

# dump library sources
.PHONY: lib_sources
lib_sources:
	@echo $(patsubst %,%.cpp,$(MORPHODITA_O))
