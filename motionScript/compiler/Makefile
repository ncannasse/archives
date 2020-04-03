all:
	ocamllex lexer.mll 
	ocamlopt -pp camlp4o types.ml tools.ml bytecode.ml binary.ml lexer.ml parser.ml linker.ml compile.ml main.ml -o mts.exe
	mts.exe std.mts math.mts

clean:
	rm -rf *.o *.obj *.cmo *.cmi *.cmx lexer.ml mts mts.exe std.mto math.mto
