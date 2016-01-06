include ../src/Makefile.builtem
include t2t_docsys/Makefile.include

.PHONY: all doc web install
all: doc
doc: morphodita.html morphodita.txt morphodita_readme.txt morphodita_readme_md.md morphodita_install.txt
doc: morphodita_bindings_csharp_readme.txt morphodita_bindings_java_readme.txt morphodita_bindings_perl_readme.txt morphodita_bindings_python_readme.txt
web: web.html web_install.html web_user.html web_api_tutorial.html web_api_reference.html web_about.html web_online.html
install: doc
	$(call cp,morphodita.txt,../MANUAL)
	$(call cp,morphodita.html,../MANUAL.html)
	$(call cp,morphodita_readme.txt,../README)
	$(call cp,morphodita_readme_md.md,../README.md)
	$(call cp,morphodita_install.txt,../INSTALL)
	$(call cp,morphodita_bindings_csharp_readme.txt,../bindings/README.CS)
	$(call cp,morphodita_bindings_java_readme.txt,../bindings/README.JAVA)
	$(call cp,morphodita_bindings_perl_readme.txt,../bindings/README.PERL)
	$(call cp,morphodita_bindings_python_readme.txt,../bindings/README.PYTHON)

%.html: %.t2t $(wildcard *.t2t)
	$(call t2t_docsys_html,$@,$<,manual $(if $(findstring _,$*),striplevel))

web%html: morphodita%t2t $(wildcard *.t2t)
	$(call t2t_docsys_html,$@,$<,web html_addlevel $(if $(findstring _,$*),striplevel))

%.txt: %.t2t $(wildcard *.t2t)
	$(call t2t_docsys_txt,$@,$<,manual $(if $(findstring _,$*),striplevel))

%.md: %.t2t $(wildcard *.t2t)
	$(call t2t_docsys_md,$@,$<,manual $(if $(findstring _,$*),striplevel))

%.pdf: %.t2t $(wildcard *.t2t)
	$(call t2t_docsys_pdf,$@,$<,manual $(if $(findstring _,$*),striplevel))

.PHONY: clean
clean:
	@$(call rm,$(T2T_DOCSYS_OUTPUT_WILDCARDS))
