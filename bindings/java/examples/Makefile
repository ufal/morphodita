TARGETS = RunMorphoCli.class RunTagger.class RunTokenizer.class

all: $(TARGETS)

%.class: %.java
	javac -cp .. $<

%: %.class
	LD_LIBRARY_PATH=.. java -cp .:.. $* $(ARGS)

.PHONY: clean
clean:
	rm -rf $(TARGETS)
