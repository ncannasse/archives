OCAMLDIR=c:\ocaml\lib\

all: clean install

clean:
	rm -f osiris.cma osiris.cmxa osiris.cmi osiris.cmo osiris.cmx osiris.obj osiris.lib

install: osiris.cma osiris.cmxa 
	cp -f osiris.cma osiris.cmxa osiris.lib osiris.cmi $(OCAMLDIR)

osiris.cma:
	ocamlc -a osiris.mli osiris.ml -o osiris.cma

osiris.cmxa:
	ocamlopt -a osiris.mli osiris.ml -o osiris.cmxa
