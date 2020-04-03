#include "value.h"
#include "api.h"
#include "context.h"

#include <stdio.h>
#ifdef _WIN32
#	include <windows.h>
#else
#	include <stdlib.h>
#	include <dlfcn.h>
#	define HANDLE void *
#	define HMODULE void *
#	define LoadLibrary(name) dlopen(name, RTLD_LAZY)
#	define GetProcAddress(h,name) dlsym(h,name)
#	define FreeLibrary(h) dlclose(h)
#endif

typedef void *(*c_prim)();

typedef struct lib {
	const char *name;
	HANDLE h;
	struct lib *next;
} lib;

static _context *dll_context = NULL;

EXTERN void *dll_primitive_resolve( vm *vm, const char *name, int nargs ) {
	lib *l;
	char *pos = strchr(name,'_');
	char buf[100];
	c_prim func;
	if( dll_context == NULL )
		dll_context = context_new();
	l = (lib*)context_get_data(dll_context);
	if( pos == NULL || nargs > 0xFF || nargs < -1 )
		return NULL;
	*pos = 0;
	while( l != NULL ) {
		if( strcmp(l->name,name) == 0 )
			break;
		l = l->next;
	}
	if( l == NULL ) {
#		ifdef CUSTOM_DLL
		const char *std = CUSTOM_DLL;
		HANDLE h = (strcmp(name,"std")==0)?LoadLibrary(std):LoadLibrary(name);
#		else
		HANDLE h = LoadLibrary(name);
#		endif
		if( h == NULL )
			return NULL;
		l = malloc(sizeof(lib));
		l->name = (const char*)strdup(name);
		l->h = h;
		l->next = (lib*)context_get_data(dll_context);
		context_set_data(dll_context,l);
	}
	*pos = '_';
	if( strlen(name) > 90 )
		return NULL;
	if( nargs == -1 )
		sprintf(buf,"%s__MULT",name);
	else
		sprintf(buf,"%s__%d",name,nargs);
	func = (c_prim)GetProcAddress((HMODULE)l->h,buf);
	if( func == NULL )
		return NULL;
	return func();
}

EXTERN void dll_primitive_free() {
	lib *l = (lib*)context_get_data(dll_context);
	while( l != NULL ) {
		lib *ltmp = l->next;
		FreeLibrary(l->h);
		free((char*)l->name);
		free(l);
		l = ltmp;
	}
	context_set_data(dll_context,NULL);
}
