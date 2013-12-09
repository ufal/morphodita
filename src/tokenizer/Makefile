all: czech_tokenizer.cpp

%.cpp: %.rl
	ragel $< -G2 -o $@ && sed '1d; /^#line [0-9]/d' -i $@
