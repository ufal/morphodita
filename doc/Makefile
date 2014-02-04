all:
	@echo "make [html | txt | pdf | web | manual]"

html: morphodita.html
txt: morphodita.txt morphodita_install.txt
pdf: morphodita.pdf
web: web.html web_install.html web_user.html web_api_tutorial.html web_api_reference.html web_about.html
manual: morphodita.html morphodita.txt morphodita_readme.txt morphodita_install.txt
	cp morphodita.txt ../MANUAL
	cp morphodita.html ../MANUAL.html
	cp morphodita_readme.txt ../README
	cp morphodita_install.txt ../INSTALL

morphodita.html: morphodita.t2t *.t2t
	./txt2tags -t html -C not_web.conf $<

morphodita_%.html: morphodita_%.t2t *.t2t
	./txt2tags -t html -C not_web.conf -C strip_level.conf --no-toc --no-enum-title -o- $< | sed '/^<H2>=*<\/H2>$$/d' >$@

web.html: morphodita.t2t *.t2t
	./txt2tags -t html -C web.conf -o web.html morphodita.t2t

web_%.html: morphodita_%.t2t *.t2t
	./txt2tags -t html -C web.conf -C strip_level.conf -o- $< | sed '/^<H2>=*<\/H2>$$/d' >$@

morphodita.txt: morphodita.t2t *.t2t
	./txt2tags -t txt -C not_web.conf --no-toc --no-enum-title -o- $< | sed '/^$$/{3d;$$d}' >$@

morphodita_%.txt: morphodita_%.t2t *.t2t
	./txt2tags -t txt -C not_web.conf -C strip_level.conf --no-toc --no-enum-title -o- $< | sed '/^$$/{3d;$$d}' >$@

morphodita.tex: morphodita.t2t *.t2t
	./txt2tags -t tex -C not_web.conf $<

morphodita.pdf: morphodita.tex *.sty
	pdflatex $<
	pdflatex $<

.PHONY: clean
clean:
	rm -f *.aux *.dvi *.html *.log *.pdf *.out *.tex *.toc *.txt
