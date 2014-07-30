include Makefile.options

EXECUTABLES = $(call exe,encode_dictionary run_morpho_analyze run_morpho_cli run_morpho_generate run_tagger run_tokenizer tagger_accuracy train_tagger)
DYNAMIC_LIBRARIES = $(call dynlib,libmorphodita)
STATIC_LIBRARIES = $(call lib,libmorphodita libmorphodita.dyn)
TOOLS = $(call exe,tools/benchmark_dictionary tools/csts tools/fill_lemmas tools/morpho_accuracy tools/remap_tags tools/strip_combining_marks tools/test_dictionary)
default: exe
exe: $(EXECUTABLES)
lib: $(DYNAMIC_LIBRARIES)
a: $(STATIC_LIBRARIES)
tools: $(TOOLS)
all: exe lib a tools

# dependency and compilation rules
include Makefile.rules

# object files groups
LZMA_DEC_O = utils/compressor_load utils/lzma/LzmaDec
LZMA_ENC_O = utils/compressor_save utils/lzma/LzmaEnc utils/lzma/LzFind
MORPHO_O = $(LZMA_DEC_O) morpho/czech_morpho morpho/english_morpho morpho/english_morpho_guesser morpho/external_morpho morpho/generic_morpho morpho/morpho morpho/morpho_statistical_guesser morpho/tag_filter tagset_converter/identity_tagset_converter tagset_converter/pdt_to_conll2009_tagset_converter tagset_converter/strip_lemma_comment_tagset_converter tagset_converter/strip_lemma_id_tagset_converter tagset_converter/tagset_converter tokenizer/czech_tokenizer tokenizer/english_tokenizer tokenizer/generic_tokenizer tokenizer/tokenizer tokenizer/utf8_tokenizer tokenizer/vertical_tokenizer unilib/unicode unilib/utf8 version/version
MORPHODITA_O = $(MORPHO_O) tagger/tagger

# libraries
$(call lib,libmorphodita): $(call obj,$(MORPHODITA_O))
	ar -rcs $@ $^
$(call lib,libmorphodita.dyn): $(call dynobj,$(MORPHODITA_O))
	ar -rcs $@ $^
$(call dynlib,libmorphodita): $(call dynobj,$(MORPHODITA_O))
	$(CXX) -o $@ $(DYN_LD_FLAGS) $^ $(call version_script,libmorphodita.map) $(call create_implib,libmorphodita)

# executables
$(call exe,encode_dictionary): $(call obj,$(LZMA_ENC_O) morpho/czech_morpho_encoder morpho/english_morpho_encoder morpho/english_morpho_guesser_encoder morpho/external_morpho_encoder morpho/generic_morpho_encoder morpho/morpho_prefix_guesser_encoder morpho/morpho_statistical_guesser_encoder morpho/raw_morpho_dictionary_reader utils/input utils/parse_options version/version)
$(call exe,run_morpho_analyze): $(call obj,$(MORPHODITA_O) utils/input utils/output utils/parse_options)
$(call exe,run_morpho_cli): $(call obj,$(MORPHODITA_O) utils/input utils/parse_options)
$(call exe,run_morpho_generate): $(call obj,$(MORPHODITA_O) utils/input utils/parse_options)
$(call exe,run_tagger): $(call obj,$(MORPHODITA_O) utils/input utils/output utils/parse_options)
$(call exe,run_tokenizer): $(call obj,$(MORPHODITA_O) utils/input utils/output utils/parse_options)
$(call exe,tagger_accuracy): $(call obj,$(MORPHODITA_O) utils/input utils/parse_options)
$(call exe,train_tagger): $(call obj,$(LZMA_ENC_O) $(MORPHODITA_O) utils/input utils/parse_options)
$(call exe,tools/benchmark_dictionary): $(call obj,$(MORPHO_O) utils/input)
$(call exe,tools/csts): $(call obj,$(MORPHO_O) utils/input)
$(call exe,tools/fill_lemmas): $(call obj,$(MORPHO_O) utils/input)
$(call exe,tools/morpho_accuracy): $(call obj,$(MORPHO_O) utils/input)
$(call exe,tools/remap_tags): $(call obj,$(MORPHO_O) utils/input)
$(call exe,tools/strip_combining_marks): $(call obj,unilib/utf8 unilib/unistrip utils/input utils/parse_options version/version)
$(call exe,tools/test_dictionary): $(call obj,$(MORPHO_O) morpho/raw_morpho_dictionary_reader utils/input)
$(EXECUTABLES) $(TOOLS): $(call exe,%): $(call obj,%)
	$(CXX) $^ -o $@ $(LD_FLAGS)

# cleaning
.PHONY: clean

clean:
	@$(call rmdir,.objs) $(call rm,$(EXECUTABLES) $(DYNAMIC_LIBRARIES) $(STATIC_LIBRARIES) $(TOOLS) $(call implib,libmorphodita))

# dump library sources
lib_sources:
	@echo $(patsubst %,%.cpp,$(MORPHODITA_O))
