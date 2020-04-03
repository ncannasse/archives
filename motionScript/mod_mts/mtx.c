#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <vmcontext.h>

typedef struct bincache {
	int fid;
	time_t tstamp;
	char *file;
	vm *vm;
	struct bincache *next;
} bincache;

static _context *cache = NULL;

static void free_cache( bincache *prev, bincache *c ) {
	if( prev == NULL )
		context_set_data(cache,c->next);
	else
		prev->next = c->next;
	free(c->file);
	free(c);
}

static char *read_mtx( const char *file, int *data_size ) {
	char *data;
	int pos = 0;
	int size;
	int l;
	FILE *f = fopen(file,"rb");
	if( f == NULL )
		return NULL;
	fseek(f,0,SEEK_END);
	size = ftell(f);
	fseek(f,0,SEEK_SET);
	data = malloc(size);
	*data_size = size;
	while( size > 0 ) {
		l = fread(data + pos,1,size,f);
		if( l == -1 ) {
			free(data);
			fclose(f);
			return NULL;
		}
		size -= l;
		pos += l;
	}
	fclose(f);
	return data;
}

vm *load_mtx( vmparams *params, const char *file ) {
	struct stat buf;
	int fid;
	int datasize;
	char *data;
	bincache *prev = NULL;
	bincache *c = context_get_data(cache);

	if( stat(file,&buf) != 0 )
		return NULL;
	
	fid = (int)val_id(file);
	while( c != NULL ) {
		if( c->fid == fid && strcmp(c->file,file) == 0 ) {
			if( c->tstamp == buf.st_mtime )				
				return c->vm;
			break;
		}
		prev = c;
		c = c->next;
	}
	if( cache == NULL )
		cache = context_new();
	if( c == NULL ) {
		c = (bincache*)malloc(sizeof(bincache));
		c->fid = fid;
		c->file = strdup(file);
		c->next = context_get_data(cache);
		context_set_data(cache,c);
		prev = NULL;
	}
	else
		vm_free(c->vm);

	c->tstamp = buf.st_mtime;

	data = read_mtx(file,&datasize);
	if( data == NULL ) {
		free_cache(prev,c);
		return (vm*)-1;
	}

	c->vm = vm_alloc(params);
	vm_init(c->vm,data,datasize);
	free(data);

	if( vm_error(c->vm) ) {
		vm *v = c->vm;
		free_cache(prev,c);
		return v;
	}

	return c->vm;
}
