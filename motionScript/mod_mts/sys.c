#include <value.h>
#include <stdio.h>
#ifdef _WIN32
#	include <windows.h>
#endif
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "context.h"

typedef struct filelist {
	char *fpath;	
	struct filelist *next;
} filelist;

typedef struct {
	value name;
	FILE *io;
	bool tmp;
} fio;

#define val_file(o)	((fio*)val_odata(o))

extern value get_cwd();

DEFINE_CLASS(std_sys,file);

static filelist *tmpfiles = NULL;

static void file_gc_add( const char *fname ) {
	filelist *f = (filelist*)malloc(sizeof(filelist));
	f->fpath = strdup(fname);
	f->next = tmpfiles;
	tmpfiles = f;
}

void file_gc_run( int timeout ) {
	filelist *f = tmpfiles;
	filelist *prev = NULL;
	int now = time(NULL);
	while( f != NULL ) {
		struct stat inf;
		if( stat(f->fpath,&inf) != 0 || inf.st_atime + timeout < now ) {
			filelist *tmp = f->next;
			if( prev == NULL )
				tmpfiles = f->next;
			else
				prev->next = f->next;
			unlink(f->fpath);
			free(f->fpath);
			free(f);
			f = tmp;
		} else {
			prev = f;
			f = f->next;
		}
	}
}

static value file_new1( value f ) {
	value o = val_this();
	fio *io;
	stringbuf b;
	val_check_obj(o,t_file);
	if( !val_is_string(f) )
		return NULLVAL;
	b = alloc_stringbuf(NULL);
	io = (fio*)alloc_small(sizeof(fio));
	if( *val_string(f) == '~' ) {
#ifdef _WIN32
		char buf[MAX_PATH];
		GetTempPath(MAX_PATH,buf);
		stringbuf_append(b,buf);
#else
		stringbuf_append(b,"/tmp/");
#endif		
		val_to_string(f,b);
		io->tmp = true;
	} else {
		val_to_string(get_cwd(),b);
		val_to_string(f,b);
	}
	io->name = stringbuf_to_string(b);
	io->io = NULL;	
	val_odata(o) = (value)io;
	return o;
}

static value file_toString() {
	value o = val_this();
	stringbuf b;
	val_check_obj(o,t_file);
	b = alloc_stringbuf("#file:");
	val_to_string(val_file(o)->name,b);
	return stringbuf_to_string(b);
}

static value file_create() {
	value o = val_this();
	fio *f;
	val_check_obj(o,t_file);
	f = val_file(o);
	if( f->io != NULL )
		fclose(f->io);
	f->io = fopen(val_string(f->name),"wb");
	if( f->tmp ) {
		f->tmp = false;
		file_gc_add(val_string(f->name));
	}
	return alloc_bool(f->io != NULL);
}

static value file_append() {
	value o = val_this();
	fio *f;
	val_check_obj(o,t_file);
	f = val_file(o);
	if( f->io != NULL )
		fclose(f->io);
	f->io = fopen(val_string(f->name),"ab");
	return alloc_bool(f->io != NULL);
}

static value file_open() {
	value o = val_this();
	fio *f;
	val_check_obj(o,t_file);
	f = val_file(o);
	if( f->io != NULL )
		fclose(f->io);
	f->io = fopen(val_string(f->name),"rb");
	return alloc_bool(f->io != NULL);
}

static value file_close() {
	value o = val_this();
	fio *f;
	val_check_obj(o,t_file);
	f = val_file(o);
	if( f->io != NULL )
		fclose(f->io);
	f->io = NULL;
	return val_true;
}

static value file_contents() {
	value o = val_this();
	value s;
	fio *f;
	FILE *io;
	int len;
	int p;
	val_check_obj(o,t_file);
	f = val_file(o);
	io = fopen(val_string(f->name),"rb");
	if( io == NULL )
		return NULL;
	
	fseek(io,0,SEEK_END);
	len = ftell(io);
	fseek(io,0,SEEK_SET);
	s = alloc_raw_string(len);
	p = 0;
	while( len > 0 ) {
		int d = fread((char*)val_string(s)+p,1,len,io);
		if( d <= 0 ) {
			fclose(io);
			return NULLVAL;
		}
		p += d;
		len -= d;
	}	
	fclose(io);	
	return s;
}

static value file_write( value s ) {
	value o = val_this();
	fio *f;
	int p = 0;
	int len;
	val_check_obj(o,t_file);
	if( !val_is_string(s) )
		return NULLVAL;
	f = val_file(o);
	if( f->io == NULL )
		return NULLVAL;
	len = val_strlen(s);
	while( len > 0 ) {
		int d = fwrite(val_string(s)+p,1,len,f->io);
		if( d <= 0 )			
			return NULLVAL;		
		p += d;
		len -= d;
	}
	return val_true;
}

static value file_read( value n ) {
	value o = val_this();
	fio *f;
	int p = 0;
	int len;
	value s;
	val_check_obj(o,t_file);
	if( !val_is_int(n) )
		return NULLVAL;
	f = val_file(o);
	len = val_int(n);
	if( f->io == NULL )
		return NULLVAL;
	s = alloc_raw_string(len);
	while( len > 0 ) {
		int d = fread((char*)val_string(s)+p,1,len,f->io);
		if( d <= 0 ) {
			val_strlen(s) = p;
			break;
		}
		p += d;
		len -= d;
	}
	return s;
}

static value file_exists() {
	value o = val_this();
	struct stat inf;
	val_check_obj(o,t_file);
	return alloc_bool( stat(val_string(val_file(o)->name),&inf) == 0 );
}

static value file_delete() {
	value o = val_this();
	val_check_obj(o,t_file);
	return alloc_bool( unlink(val_string(val_file(o)->name)) == 0 );
}

static value file_fullPath() {
	value o = val_this();
	val_check_obj(o,t_file);
	return val_file(o)->name;	
}

value std_sys_file() {
	value o = alloc_class(&t_file);
	Constr(o,file,File,1);
	Method(o,file,toString,0);
	Method(o,file,create,0);
	Method(o,file,append,0);
	Method(o,file,open,0);
	Method(o,file,close,0);
	Method(o,file,exists,0);
	Method(o,file,delete,0);
	Method(o,file,contents,0);
	Method(o,file,write,1);
	Method(o,file,read,1);
	Method(o,file,fullPath,0);
	return o;
}

static value std_sys_execute( value cmd ) {
	if( !val_is_string(cmd) )
		return NULLVAL;
	return alloc_int( system(val_string(cmd)) );
}

DEFINE_PRIM(std_sys_execute,1);

