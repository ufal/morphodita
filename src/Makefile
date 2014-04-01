include Makefile.options

EXECUTABLES = $(call exe,benchmark_dictionary csts encode_dictionary fill_lemmas morpho_accuracy remap_tags run_morpho_analyze run_morpho_cli run_morpho_generate run_tagger run_tokenizer test_dictionary tagger_accuracy train_tagger)
LIBRARIES = $(call lib,libmorphodita libmorphodita.dyn) $(call dynlib,libmorphodita)
all: exe
exe: $(EXECUTABLES)
lib: $(LIBRARIES)

# dependency and compilation rules
include Makefile.rules

# object files groups
LZMA_DEC_O = utils/compressor_load utils/lzma/LzmaDec
LZMA_ENC_O = utils/compressor_save utils/lzma/LzmaEnc utils/lzma/LzFind
MORPHO_O = $(LZMA_DEC_O) morpho/czech_morpho morpho/english_morpho morpho/english_morpho_guesser morpho/generic_morpho morpho/morpho morpho/morpho_statistical_guesser morpho/tag_filter tagset_converter/identity_tagset_converter tagset_converter/pdt_to_conll2009_tagset_converter tagset_converter/tagset_converter tokenizer/czech_tokenizer tokenizer/english_tokenizer tokenizer/generic_tokenizer tokenizer/tokenizer tokenizer/utf8_tokenizer tokenizer/vertical_tokenizer utils/persistent_unordered_map utils/utf8 version/version
MORPHODITA_O = $(MORPHO_O) tagger/tagger

# libraries
$(call lib,libmorphodita): $(call obj,$(MORPHODITA_O))
	ar -rcs $@ $^
$(call lib,libmorphodita.dyn): $(call dynobj,$(MORPHODITA_O))
	ar -rcs $@ $^
$(call dynlib,libmorphodita): $(call lib,libmorphodita.dyn)
	$(CXX) -o $@ $(DYN_LD_FLAGS) -Wl,--whole-archive $< -Wl,-no-whole-archive -Wl,--version-script=libmorphodita.map $(call create_implib,libmorphodita)

# executables
$(call exe,benchmark_dictionary): $(call obj,$(MORPHO_O) utils/input)
$(call exe,csts): $(call obj,$(MORPHO_O) utils/input)
$(call exe,encode_dictionary): $(call obj,$(LZMA_ENC_O) morpho/czech_morpho_encoder morpho/english_morpho_encoder morpho/english_morpho_guesser_encoder morpho/generic_morpho_encoder morpho/morpho_prefix_guesser_encoder morpho/morpho_statistical_guesser_encoder morpho/raw_morpho_dictionary_reader utils/input utils/persistent_unordered_map)
$(call exe,fill_lemmas): $(call obj,$(MORPHO_O) utils/input)
$(call exe,morpho_accuracy): $(call obj,$(MORPHO_O) utils/input)
$(call exe,remap_tags): $(call obj,$(MORPHO_O) utils/input)
$(call exe,run_morpho_analyze): $(call obj,$(MORPHO_O) utils/input utils/output utils/parse_options)
$(call exe,run_morpho_cli): $(call obj,$(MORPHO_O) utils/input)
$(call exe,run_morpho_generate): $(call obj,$(MORPHO_O) utils/input utils/parse_options)
$(call exe,run_tagger): $(call obj,$(MORPHODITA_O) utils/input utils/output utils/parse_options)
$(call exe,run_tokenizer): $(call obj,$(MORPHODITA_O) utils/input utils/output utils/parse_options)
$(call exe,test_dictionary): $(call obj,$(MORPHO_O) morpho/raw_morpho_dictionary_reader utils/input)
$(call exe,tagger_accuracy): $(call obj,$(MORPHODITA_O) utils/input)
$(call exe,train_tagger): $(call obj,$(LZMA_ENC_O) $(MORPHODITA_O) utils/input)
$(EXECUTABLES): $(call exe,%): $(call obj,%)
	$(CXX) $^ -o $@ $(LD_FLAGS)

# cleaning
.PHONY: clean

clean:
	@$(call rmdir,.objs) $(call rm,$(LIBRARIES) $(EXECUTABLES) $(call implib,libmorphodita))

# dump library sources
lib_sources:
	@echo $(patsubst %,%.cpp,$(MORPHODITA_O))
