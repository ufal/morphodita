all:
	@echo "make [html | pdf | txt | web | manual]"

html pdf tex txt:%:morphodita.%

morphodita.html morphodita.tex morphodita.txt: morphodita.%: morphodita.t2t *.t2t
	./txt2tags -t $* -C not_web.conf $<

morphodita.pdf: morphodita.tex *.sty
	pdflatex $<
	pdflatex $<

web: morphodita.t2t morphodita_user.t2t morphodita_api_tutorial.t2t morphodita_api_reference.t2t
	./txt2tags -t html -C web.conf -o web.html morphodita.t2t
	./txt2tags -t html -C web.conf -C web_subpage.conf -o web_users_guide.html morphodita_user.t2t
	./txt2tags -t html -C web.conf -C web_subpage.conf -o web_api_tutorial.html morphodita_api_tutorial.t2t
	./txt2tags -t html -C web.conf -C web_subpage.conf -o web_api_reference.html morphodita_api_reference.t2t

manual: html pdf txt
	$(foreach fmt,$^,cp morphodita.$(fmt) ../MANUAL.$(fmt); )

.PHONY: clean
clean:
	rm -f *.aux *.dvi *.html *.log *.pdf *.out *.tex *.toc *.txt
