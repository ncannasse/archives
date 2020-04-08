OBJS = mtypes.vcproj
LIBS = xml-light.+ extLib.+ unix.+ str.+ extc.+
MTFILES = flash7\*.mt lib\*.mt

native:
	ocamake -opt -epp -pp camlp4o type.ml expr.ml parser.ml lexer.mll
	ocamake -epp -o mtypes.cmxa -lp -linkall $(OBJS) -n main.ml -n parser.ml -n expr.ml -n lexer.ml -n lexer.mll -n type.ml type.cmx expr.cmx lexer.cmx parser.cmx
	ocamake -epp -o mtypes.exe $(LIBS:+=cmxa) mtypes.cmxa main.ml
	
byte:
	ocamake -epp -pp camlp4o -o mtypes.cma -lp -linkall $(OBJS) -n main.ml
	ocamake -lp -custom -epp -o mtypes.exe $(LIBS:+=cma) mtypes.cma main.ml

clean:
	ocamake -clean $(OBJS)
	ocamake -clean -opt $(OBJS)
	
rebuild: clean native

rebuild_byte: clean byte

docgen:
	@cd flash7
	..\mtypes.exe -doc ../doc -doc-exec xalan -doc-style ../genDoc/style2.xsl *.mt -v
	@cd ../lib
	..\mtypes.exe -doc ../doc -doc-exec xalan -doc-style ../genDoc/style2.xsl -cp flash7 *.mt -v
