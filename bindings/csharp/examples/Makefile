TARGETS = RunMorphoCli.exe RunTagger.exe

all: $(TARGETS)

%.exe: %.cs
	mcs -r:System.Xml $< $(wildcard ../Ufal/MorphoDiTa/*)

.PHONY: clean
clean:
	rm -rf $(TARGETS)
