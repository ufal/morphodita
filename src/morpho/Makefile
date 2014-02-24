morpho/english_morpho_guesser.cpp: %.cpp: %.rl
	ragel $< -F1 -o $@ && sed '1d; /^#line [0-9]/d' -i $@
