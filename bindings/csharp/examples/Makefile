TARGETS = RunMorphoCli.exe RunTagger.exe

all: $(TARGETS)

%.exe: %.cs
	mcs $< $(wildcard ../Ufal/MorphoDiTa/*)

.PHONY: clean
clean:
	rm -rf $(TARGETS)
