tokenizer/czech_tokenizer.cpp tokenizer/english_tokenizer.cpp: %.cpp: %.rl $(wildcard %*.rl) $(wildcard tokenizer/ragel/*.rl)
	ragel $< -T1 -o $@ && sed '1d; /^#line [0-9]/d' -i $@
