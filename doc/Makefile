include t2t_docsys/Makefile.include

all: doc
html: morphodita.html
txt: morphodita.txt
pdf: morphodita.pdf
web: web.html web_install.html web_user.html web_api_tutorial.html web_api_reference.html web_about.html web_online.html
doc: morphodita.html morphodita.txt morphodita_readme.txt morphodita_readme_md.md morphodita_install.txt
doc: morphodita_bindings_csharp_readme.txt morphodita_bindings_java_readme.txt morphodita_bindings_perl_readme.txt morphodita_bindings_python_readme.txt
install: doc
	cp morphodita.txt ../MANUAL
	cp morphodita.html ../MANUAL.html
	cp morphodita_readme.txt ../README
	cp morphodita_readme_md.md ../README.md
	cp morphodita_install.txt ../INSTALL
	cp morphodita_bindings_csharp_readme.txt ../bindings/README.CS
	cp morphodita_bindings_java_readme.txt ../bindings/README.JAVA
	cp morphodita_bindings_perl_readme.txt ../bindings/README.PERL
	cp morphodita_bindings_python_readme.txt ../bindings/README.PYTHON

morphodita%html: morphodita%t2t $(wildcard *.t2t)
	$(call t2t_docsys_html,$@,$<,manual $(if $(filter-out .,$*),striplevel))

web%html: morphodita%t2t $(wildcard *.t2t)
	$(call t2t_docsys_html,$@,$<,web html_addlevel $(if $(filter-out .,$*),striplevel))

morphodita%txt: morphodita%t2t $(wildcard *.t2t)
	$(call t2t_docsys_txt,$@,$<,manual $(if $(filter-out .,$*),striplevel))

morphodita%md: morphodita%t2t $(wildcard *.t2t)
	$(call t2t_docsys_md,$@,$<,manual $(if $(filter-out .,$*),striplevel))

morphodita%pdf: morphodita%t2t $(wildcard *.t2t)
	$(call t2t_docsys_pdf,$@,$<,manual $(if $(filter-out .,$*),striplevel))

.PHONY: clean
clean:
	@rm -f $(T2T_DOCSYS_OUTPUT_WILDCARDS)
