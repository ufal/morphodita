all:
	@echo "make [html | txt | pdf | web | manual]"

html: morphodita.html
txt: morphodita.txt morphodita_install.txt
pdf: morphodita.pdf

morphodita.html: morphodita.t2t *.t2t
	./txt2tags -t html -C not_web.conf $<

morphodita.txt: morphodita.t2t *.t2t
	./txt2tags -t txt -C not_web.conf --no-toc --no-enum-title $<

morphodita_install.txt: morphodita_install.t2t
	./txt2tags -t txt -C not_web.conf -C strip_level.conf --no-toc --no-enum-title $<

morphodita.tex: morphodita.t2t *.t2t
	./txt2tags -t tex -C not_web.conf $<

morphodita.pdf: morphodita.tex *.sty
	pdflatex $<
	pdflatex $<

web: morphodita.t2t morphodita_install.t2t morphodita_user.t2t morphodita_api_tutorial.t2t morphodita_api_reference.t2t
	./txt2tags -t html -C web.conf -o web.html morphodita.t2t
	./txt2tags -t html -C web.conf -C strip_level.conf -o web_install.html morphodita_install.t2t
	./txt2tags -t html -C web.conf -C strip_level.conf -o web_users_guide.html morphodita_user.t2t
	./txt2tags -t html -C web.conf -C strip_level.conf -o web_api_tutorial.html morphodita_api_tutorial.t2t
	./txt2tags -t html -C web.conf -C strip_level.conf -o web_api_reference.html morphodita_api_reference.t2t

manual: html txt
	cp morphodita.txt ../MANUAL
	cp morphodita.html ../MANUAL.html
	cp morphodita_install.txt ../INSTALL

INSTALL: morphodita_install.t2t

.PHONY: clean veryclean
clean:
	rm -f *.aux *.dvi *.html *.log *.pdf *.out *.tex *.toc *.txt
