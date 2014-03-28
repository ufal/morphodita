TOKENIZERS=$(patsubst %,tokenizer/%_tokenizer.cpp,czech english generic)

$(TOKENIZERS): %.cpp: %.rl $(wildcard %*.rl) $(wildcard tokenizer/ragel/*.rl)
	ragel $< -T1 -o $@ && sed '1d; /^#line [0-9]/d' -i $@
