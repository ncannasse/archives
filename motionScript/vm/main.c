#if defined(WIN32) && defined(NDEBUG)
	extern long _ftol( double f );
	long _ftol2( double f ) { return _ftol(f); };
#endif

#ifdef MTSVM_MAIN

#include <stdio.h>
#include <malloc.h>
#include "api.h"
#ifdef _DEBUG
#	include <crtdbg.h>
#endif

extern void *dll_primitive_resolve( vm *vm, const char *name, int nargs );
extern void dll_primitive_free();

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

int main( int argc, char *argv[] ) {
	const char *filename;
	vm *v;
	void *data;
	int datasize;
	vmparams params;

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_DELAY_FREE_MEM_DF );
#endif

	if( argc < 2 ) {
		printf("Usage : mtsvm <file.mtx>\n");
		return 1;
	}

	filename = argv[1];

	params.loader = NULL;
	params.printer = NULL;
	params.resolve = dll_primitive_resolve;
	params.custom = NULL;

	vm_global_init();
	v = vm_alloc(&params);
	data = read_mtx(filename,&datasize);

	if( data == NULL ) {
		printf("File not found %s\n",filename);
		return 1;
	}

	vm_init(v,data,datasize);
	free(data);

	if( vm_error(v) ) {
		printf("Error #%d while loading %s\n",vm_error(v),filename);
		vm_free(v);
		return 2;
	}

	vm_execute(v);

	if( vm_error(v) ) {
		printf("Execution aborted : error #%d\n",vm_error(v));
		vm_free(v);
		return 3;
	}

	vm_free(v);
	dll_primitive_free();
	vm_global_free();
	return 0;
}

#endif
