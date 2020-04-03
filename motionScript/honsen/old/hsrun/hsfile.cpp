/* ************************************************************************ */
/*																			*/
/*	Honsen Runtime															*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include <windows.h>
#undef ERROR
#include "hsfile.h"
#include "hsconfig.h"
#include "../common/mtw.h"
#include "../std/load.h"
#include "../std/interp.h"
#include "../std/context.h"
#include "window.h"
#include "../m3dse/m3d_api.h"
#include "../hslib/hslib.h"
#include "../common/context.h"
#include "../common/timer.h"
#include <time.h>

#define MAX_MESSAGE 1000

struct Context {
	HWND log_window;
	char *errmsg;
	M3D::Context *ctx;
	void *std_context;
};

Context *GetContext() {
	return (Context*)get_context();
}

void ShareContext( Context *c ) {
	M3D::ShareContext(c->ctx);
	share_std_context(c->std_context);
}

Context *last_context = NULL;

/* ------------------------------------------------------------------------ */

C_FUNCTION_BEGIN

EXTERN void set_printer(void *printer);
EXTERN void process_main();
EXTERN unsigned int process_events(unsigned int delta);
EXTERN void gc_loop();

/* ------------------------------------------------------------------------ */

void log_message( const char *msg ) {
	Context *c = GetContext();
	if( c->log_window == NULL )
		c->log_window = create_log_window();
	append_log(c->log_window,msg);
}

/* ------------------------------------------------------------------------ */

void log_message_println( const char *msg ) {
	log_message(msg);
	log_message("\n");
}

/* ------------------------------------------------------------------------ */

value find_resource( unsigned int id, void *ctx ) {
	MTW *r = M3D::Resource::Find((M3D::Context*)ctx,id);
	if( r == NULL ) {
		log_message_println(("Resource not found : "+itos(id)).c_str());
		return NULLVAL;
	}
	return alloc_resource(r);
}

C_FUNCTION_END

/* ------------------------------------------------------------------------ */

const char *get_error_msg() {
	return GetContext()->errmsg;
}

void free_error_msg() {
	Context *c = GetContext();
	close_window(c->log_window);
	free(c->errmsg);
	free_context();
}

/* ------------------------------------------------------------------------ */

WString result_error( EXEC_RESULT r, unsigned int pc ) {
	WString err = "HS Runtime Error : ";
	switch(r) {
	case ABORTED: err += "Aborted"; break;
	case INVALID_OPCODE: err += "Invalid opcode"; break;
	case OVERFLOW: err += "Stack Overflow"; break;
	case UNDERFLOW: err += "Stack Underflow"; break;
	case OUT_CODE: err += "Outside Code"; break;
	case OUT_STACK: err += "Outside Stack"; break;
	case OUT_ENV: err += "Outside Environment"; break;
	default: err += "*Unknown*"; break;
	}
	return err+" at "+htos(pc);
}

/* ------------------------------------------------------------------------ */

#define CLEANUP() \
	delete hs; \
	time_close(time); \
	free_std_context(); \
	if( bytecode != NULL ) free_code(bytecode); \
	M3D::Close(c->ctx); \
	HSLib::Close();

/* ------------------------------------------------------------------------ */

#define ERROR(msg) { \
		c->errmsg = strdup(((WString)(msg)).c_str()); \
		CLEANUP(); \
		return false; \
	}

/* ------------------------------------------------------------------------ */

bool hsuncompress( MTW *m, MTW *parent ) {
	if( m->IsGroup() ) {		
		FOREACH(MTW*,*m->childs);			
			if( !hsuncompress(item,m) )
				return false;
			if( item->tag == MAKETAG(HSGZ) ) {
				m->childs->Delete(item);
				return hsuncompress(m,parent);
			}
		ENDFOR;
	} else if(
		m->tag == MAKETAG(VECT) || m->tag == MAKETAG(MAP0) ||
		(m->tag == MAKETAG(DATA) && parent->tag == MAKETAG(Fram))
	) {
		int nvects = HSLib::UncompressHeader(m->data,m->size);
		if( nvects == -1 )
			return false;
		VECTOR *tmp = new VECTOR[nvects];		
		if( !HSLib::Uncompress(tmp,&nvects,m->data,m->size) ) {
			delete tmp;
			return false;
		}
		delete m->data;
		m->data = tmp;
		m->size = nvects * sizeof(VECTOR);
	} else if( m->tag == MAKETAG(HSGZ) && m->size >= 4 ) {
		int len = *(DWORD*)m->data;
		char *data = new char[len];
		if( !HSLib::Unzip(data,&len,(char*)m->data+4,m->size-4) ) {
			delete data;
			return false;
		}
		MTW *u = MTW::ReadData(data,len);
		delete data;
		if( u == NULL )
			return false;
		FOREACH(MTW*,*u->childs);
			parent->childs->Add(item);
		ENDFOR;
		u->childs->Clean();
		delete u;
	}
	return true;
}

/* ------------------------------------------------------------------------ */

bool open_hs_file( MTW* hs, const char *filename, HWND hwnd, void callback(void *), void *cbparam ) {
	srand(time(NULL));	
	timer *time = NULL;
	binary *bytecode = NULL;
	HWND wnd = NULL;
	Context *c = (Context*)alloc_context(sizeof(Context));
	last_context = c;

	// load file
	if( hs == NULL )
		ERROR("Invalid HS file : "+(WString)filename);

	if( !hsuncompress(hs,NULL) )
		ERROR("Decompression failed : "+(WString)filename);

	// retreive header infos
	MTW *hs_head = hs->Child(MAKETAG(Head));
	if( hs_head == NULL )
		ERROR("HS Header not found : "+(WString)filename);

	MTW *hs_width = hs_head->Child(MAKETAG(wdth));
	MTW *hs_height = hs_head->Child(MAKETAG(hght));
	MTW *hs_bgcolor = hs_head->Child(MAKETAG(bgcl));
	MTW *hs_fps = hs_head->Child(MAKETAG(nfps));
	if( !hs_width || !hs_height || !hs_bgcolor || !hs_fps ||
		hs_fps->number <= 0 || hs_fps->number > 150 ||
		hs_width->number < 0 || hs_width->number > 2000 ||
		hs_height->number < 0 || hs_height->number > 2000 )
		ERROR("HS Invalid Header : "+(WString)filename);

	// get code
	MTW *hs_code = hs->Child(MAKETAG(CODE));
	if( hs_code == NULL )
		ERROR("HS Code not found : "+(WString)filename);

	// initialise window & GC
	c->std_context = init_std_context();
	if(!hwnd) {
		init_window();
		wnd = create_window("Honsen Player",hs_width->number,hs_height->number);
	}
	else
		wnd = hwnd;
	// initialise M3D
	int nfps = hs_fps->number;
	M3D::InitData id;
	id.bgcolor = 0xFF000000 | hs_bgcolor->number;
	id.root = hs;
	id.get_config = get_config;
	id.debug_out = error_window;
	id.target = wnd;
	id.width = hs_width->number;
	id.height = hs_height->number;
	c->ctx = M3D::Init(&id);
	if( c->ctx == NULL )
		ERROR("Failed to Init M3DSE");

	// initialise HSLIB
	HSLib::InitContext();
	HSLib::SetConfig(get_config);
	HSLib::SetDebugOutput(error_window);
	if( !HSLib::Init(wnd) )
		ERROR("Failed to Init HSLib");

	// load bytecode
	LOADERROR lerr;
	set_printer(error_window);
	set_resource_loader(find_resource,c->ctx);
	bytecode = load_code(hs_code->data,hs_code->size,&lerr);
	if( bytecode == NULL )
		ERROR("HS Code Load error ("+itos(lerr)+") : "+(WString)filename);

	// free file data
	delete hs;
	hs = NULL;

	// execute code startup
	init(bytecode);
	M3D::SetDebugOutput(c->ctx,log_message_println);
	HSLib::SetDebugOutput(log_message_println);
	set_printer(log_message);
	exec(0,0);


	// process events loop
	time = time_init();
	unsigned int pc_error = 0;
	int sleep_time = get_config("SLEEP");
	bool stats = get_config("STATS") == 1;
	double fps_delta = 1.0 / nfps;
	time_update(time);
	double init_time = time->realtime;
	double last_log_time = init_time;
	double last_time = init_time;
	unsigned int next_event = 0;
	double code_time = 0;
	double render_time = 0;
	double gc_time = 0;
	double win_time = 0;
	cpu_time t = time_cpu();

	MSG pMsg;
	bool exit = false;
	int nb_msg = 0;
	while(!exit) {
		if( callback != NULL )
			callback(cbparam);
		if(hwnd) {
			while(PeekMessage( &pMsg, NULL, 0, 0, PM_REMOVE ) && nb_msg++ < MAX_MESSAGE) {
				switch (pMsg.message){
				case WM_KEYUP:
				case WM_KEYDOWN:
				case WM_LBUTTONDOWN:
				case WM_LBUTTONUP:
				case WM_RBUTTONDOWN:
				case WM_RBUTTONUP:
				case WM_MBUTTONDOWN:
				case WM_MBUTTONUP:
				case WM_KILLFOCUS:
					HSLib::EmitEvent(pMsg.message, pMsg.wParam, pMsg.lParam);
					break;
				case WM_DESTROY:
					exit = true;
					break;
				}
			}
			nb_msg = 0;
		}
		time_update(time);
		double curt = time->curtime;
		if( stats && curt - last_log_time > 5 ) {
			char buf[256];
			double tot_time = (code_time + render_time + gc_time + win_time) / 100.0;
			sprintf(buf,"CODE=%.2f%% RENDER=%.2f%% GC=%.2f%% CPU=%.2f%%\n",code_time/tot_time,render_time/tot_time,gc_time/tot_time,tot_time*10000/(curt - init_time));
			log_message(buf);
			last_log_time = curt;
		}

		int skip = 0;
		while( curt - last_time > fps_delta && skip++ < 10 ) {
			M3D::ClearDisplay(c->ctx);
			HSLib::Refresh(hwnd == NULL);
			process_main();
			last_time += fps_delta;
			next_event = process_events((int)(fps_delta*1000));
		}
		code_time += time_spent(t);

		if( get_result(&pc_error) != EXEC_OK )
			ERROR(result_error(get_result(&pc_error),pc_error));
		M3D::Render(c->ctx);
		time_sleep(time,sleep_time);
		render_time += time_spent(t);
		gc_loop();
		gc_time += time_spent(t);
		if(!hwnd && !window_loop())
			break;
		win_time += time_spent(t);
		if(hwnd)
			Sleep(10);
	}

	M3D::SetDebugOutput(c->ctx,error_window);
	HSLib::SetDebugOutput(error_window);
	set_printer(error_window);
	CLEANUP();
	time_cpu_free(t);
	if( !hwnd )
		close_window(wnd);
	close_window(c->log_window);
	free(c->errmsg);
	free_context();
	return true;
}

/* ************************************************************************ */
