TARGET=mod_mts.so
CFLAGS=-DEAPI -I /usr/local/apache/include -I /usr/include/mysql -I ../mtsvm -D__linux__=1 -I pcre -I /usr/local/include/mysql -L /usr/local/lib/mysql -L /usr/local/mysql/lib/mysql
LFLAGS=-lmysqlclient -lgc -lpcre -lz

all: clean
	gcc $(CFLAGS) -c *.c
	gcc -shared $(CFLAGS) -o $(TARGET) *.o $(LFLAGS) ../mtsvm/*.o 

clean:
	rm -rf *.o *.so

