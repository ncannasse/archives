CFLAGS=-I/usr/include/gc -DCUSTOM_DLL='"/usr/local/apache/libexec/mod_mts.so"' -D__linux__=1 -DFAST_INTERP

all:
	gcc $(CFLAGS) -c *.c
	ar -cru mtsvm.a *.o

clean:
	-rm *.o
	-rm *.a
