morpho/english_morpho_guesser.cpp: %.cpp: %.rl
	ragel $< -T0 -o $@ && sed '1d; /^#line [0-9]/d; /^static const int [^ ]*_en_main = [0-9]*;$$/d' -i $@
