/* ************************************************************************ */
/*																			*/
/*	Honsen Runtime															*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hsconfig.h"
#include "../common/wpairbtree.h"

WPairBTree<char *,int> config((WPairBTree<char *,int>::COMPARATOR)strcmp);

void read_config(const char *path) {
	char buf[100];
	int pos = 0, kpos = -1;
	FILE *f = fopen("config","rb");
	if( f == NULL ) {
		f = fopen(path,"rb");
		if( f == NULL )
			return;
	}
	while( fread(buf+pos,1,1,f) == 1 ) {
		switch( buf[pos] ) {
		case '\r':
		case '\n':
			if( kpos != -1 ) {
				buf[pos] = 0;
				config.Add(strdup(buf),atoi(buf+kpos+1));
			}
			kpos = -1;
			pos = -1;
			break;
		case '=':
			buf[pos] = 0;
			kpos = pos;
			break;
		}
		pos++;
		if( pos == 100 ) {
			fclose(f);
			return;
		}
	}
	fclose(f);
}

void free_config() {
	config.DeleteKeyOnly();
}

int get_config(const char *key) {
	return config.Find((char*)key);
}

void set_config( const char *key, int value ) {
	config.Add(strdup(key),value);
}

/* ************************************************************************ */
