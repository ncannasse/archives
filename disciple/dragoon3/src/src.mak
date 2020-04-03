THIS=src.dsp
CC=ocamake
CFLAGS=
MODULES=$(VCME_ML)
INTERFACES=$(VCME_MLI)
LIBS= stdlib+ dynlink+ xml-light+ mysql+ unix+

all:
	@$(CC) $(CFLAGS) -epp -lp -linkall -o dragoon3.exe $(LIBS:+=.cma) boot.ml
	@$(CC) $(CFLAGS) -epp -a -o core.cma core.ml* log.ml* run.ml*
	@$(CC) $(CFLAGS) $(THIS) -a -o dragoon3.cma -n boot.ml -n core.ml -n run.ml -n log.ml m3d.dll

clean:
	@$(CC) $(CFLAGS) -epp -lp -linkall -o dragoon3.exe $(LIBS:+=.cma) boot.ml -clean
	@$(CC) $(CFLAGS) -epp -a -o core.cma core.ml* log.ml* run.ml* -clean
	@$(CC) $(CFLAGS) $(THIS) -a -o dragoon3.cma -n boot.ml -n core.ml -n run.ml -n log.ml m3d.dll -clean
