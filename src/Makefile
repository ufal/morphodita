include Makefile.options

EXECUTABLES = $(call exe,benchmark_dictionary create_classes csts encode_dictionary morpho_accuracy remap_tags run_morpho run_tagger test_dictionary tagger_accuracy train_tagger)
LIBRARIES = $(call lib,libmorphodita libmorphodita.dyn) $(call dynlib,libmorphodita)
all: $(EXECUTABLES)
lib: $(LIBRARIES)

# dependency and compilation rules
include Makefile.rules

# object files groups
COMMON_O = arch/arch-specific
LZMA_DEC_O = utils/compressor_load utils/lzma/LzmaDec
LZMA_ENC_O = utils/compressor_save utils/lzma/LzmaEnc utils/lzma/LzFind
MORPHO_O = $(LZMA_DEC_O) morpho/czech_morpho morpho/morpho morpho/morpho_statistical_guesser morpho/tag_filter utils/persistent_unordered_map utils/tokenizer/czech_tokenizer utils/utf8
MORPHODITA_O = $(MORPHO_O) tagger/tagger

# libraries
$(call lib,libmorphodita): $(call obj,$(COMMON_O) $(MORPHODITA_O))
	@$(foreach obj,$^,cp $(obj) $(call libobj,morphodita,$(obj));)
	ar -rcs $@ $(call libobj,morphodita,$^)
	@rm -f $(call libobj,morphodita,$^)
$(call lib,libmorphodita.dyn): $(call dynobj,$(COMMON_O) $(MORPHODITA_O))
	@$(foreach obj,$^,cp $(obj) $(call libobj,morphodita,$(obj));)
	ar -rcs $@ $(call libobj,morphodita,$^)
	@rm -f $(call libobj,morphodita,$^)
$(call dynlib,libmorphodita): $(call lib,libmorphodita.dyn)
	$(CXX) -o $@ $(DYN_LD_FLAGS) -Wl,--whole-archive $< -Wl,-no-whole-archive -Wl,--version-script=libmorphodita.map

# executables
$(call exe,benchmark_dictionary): $(call obj,$(COMMON_O) $(MORPHO_O) utils/input)
$(call exe,create_classes): $(call obj,morpho/raw_morpho_dictionary_reader utils/input)
$(call exe,csts): $(call obj,$(COMMON_O) $(MORPHO_O) utils/input)
$(call exe,encode_dictionary): $(call obj,$(COMMON_O) $(LZMA_ENC_O) morpho/czech_morpho_encoder morpho/morpho_prefix_guesser_encoder morpho/morpho_statistical_guesser_encoder utils/input utils/persistent_unordered_map)
$(call exe,morpho_accuracy): $(call obj,$(COMMON_O) $(MORPHO_O) utils/input)
$(call exe,remap_tags): $(call obj,$(COMMON_O) $(MORPHO_O) utils/input)
$(call exe,run_morpho): $(call obj,$(COMMON_O) $(MORPHO_O) utils/input)
$(call exe,run_tagger): $(call obj,$(COMMON_O) $(MORPHODITA_O) utils/input)
$(call exe,test_dictionary): $(call obj,$(COMMON_O) $(MORPHO_O) morpho/raw_morpho_dictionary_reader utils/input)
$(call exe,tagger_accuracy): $(call obj,$(COMMON_O) $(MORPHODITA_O) utils/input)
$(call exe,train_tagger): $(call obj,$(COMMON_O) $(LZMA_ENC_O) $(MORPHODITA_O) utils/input)
$(EXECUTABLES): $(call exe,%): $(call obj,%)
	$(CXX) $^ -o $@ $(LD_FLAGS)

# cleaning
.PHONY: clean

clean:
	rm -rf .objs/ $(LIBRARIES) $(EXECUTABLES) $(if $(WIN),$(call lib,$(call dynlib,libmorphodita)))
