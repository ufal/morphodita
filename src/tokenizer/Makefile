all: czech_tokenizer.cpp english_tokenizer.cpp

%.cpp: %.rl %*.rl ragel/*.rl
	ragel $< -T1 -o $@ && sed '1d; /^#line [0-9]/d' -i $@
