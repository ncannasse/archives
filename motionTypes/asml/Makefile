FILES=checker.ml expr.ml lexer.mll main.ml unix/oSDep.ml parser.ml typer.ml types.ml 
LIBS=-I /usr/lib/ocaml/3.07/extlib extLib.cmxa

all:
	ocamake -pp camlp4o -opt $(FILES) -o asml $(LIBS)

install:
	-cp -f asml ~/bin
	-mkdir ~/local/lib/asml
	-cp -rf std/*.as ~/local/lib/asml

