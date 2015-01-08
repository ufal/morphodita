include t2t_docsys/Makefile.include

all:
	@echo "make [html | txt | pdf | web | manual]"

html: morphodita.html
txt: morphodita.txt
pdf: morphodita.pdf
web: web.html web_install.html web_user.html web_api_tutorial.html web_api_reference.html web_about.html web_online.html
manual: morphodita.html morphodita.txt morphodita_readme.txt morphodita_install.txt
manual: morphodita_bindings_java_readme.txt morphodita_bindings_perl_readme.txt morphodita_bindings_python_readme.txt
	cp morphodita.txt ../MANUAL
	cp morphodita.html ../MANUAL.html
	cp morphodita_readme.txt ../README
	cp morphodita_install.txt ../INSTALL
	cp morphodita_bindings_java_readme.txt ../bindings/README.JAVA
	cp morphodita_bindings_perl_readme.txt ../bindings/README.PERL
	cp morphodita_bindings_python_readme.txt ../bindings/README.PYTHON

morphodita%html: morphodita%t2t $(wildcard *.t2t)
	$(call t2t_docsys_html,$@,$<,manual $(if $(filter-out .,$*),striplevel))

web%html: morphodita%t2t $(wildcard *.t2t)
	$(call t2t_docsys_html,$@,$<,web html_addlevel $(if $(filter-out .,$*),striplevel))

morphodita%txt: morphodita%t2t $(wildcard *.t2t)
	$(call t2t_docsys_txt,$@,$<,manual $(if $(filter-out .,$*),striplevel))

morphodita%pdf: morphodita%t2t $(wildcard *.t2t)
	$(call t2t_docsys_pdf,$@,$<,manual $(if $(filter-out .,$*),striplevel))

.PHONY: clean
clean:
	@rm -f $(wildcard $(t2t_docsys_output_wildcards))
