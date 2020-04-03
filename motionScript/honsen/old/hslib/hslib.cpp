/* ************************************************************************ */
/*																			*/
/*	Honsen Lib																*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#define DIRECTINPUT_VERSION 0x0800
#define DI_BUFFERSIZE		32
#include <dinput.h>
#include "hslib.h"
#include <crtdbg.h>
#include "../common/wstring.h"
#include "../common/context.h"
#include <comdef.h>
#include "../common/wlist.h"

#define KEYNAME(code,name)  case code: c->keynames[i] = strdup(#name); break;
#define EMPTY_KEYS()		memset(c->keys,0,sizeof(c->keys)-3*sizeof(int));
#define EMPTY_BUTTONS()		{ c->mb_state[0] = 2; c->mb_state[1] = 2; c->mb_state[2] = 2; }
#define DEBUG(msg)			DebugMessage(msg,__FILE__,__LINE__)
#define DIERROR(call,msg)	(((lasterror = (call)) == DI_OK)?false:(DebugMessage(msg##" #"+itos(lasterror),__FILE__,__LINE__),true))
#define DEFAULT_REPEAT		200

typedef LPDIRECTINPUT8	OBJECT;
typedef LPDIRECTINPUTDEVICE8 DEVICE;

namespace HSLib {

/* ------------------------------------------------------------------------ */

HRESULT lasterror = 0;

struct KeyState {
	bool refreshed;
	WList<char> states;
};

struct Context : LibData {
	OBJECT object;
	DEVICE mouse;
	DEVICE keyb;
	DWORD lastick;
	DWORD delta;
	HWND target;
	bool available;
	char ktmp[MAX_KEY];
	struct KeyState ktmpState[MAX_KEY];
	int mb_state[3];
	void (*debug_out)( const char *);
	int (*get_config)( const char *);	
};

/* ------------------------------------------------------------------------ */

int no_config( const char * ) {
	return 0;
}

/* ------------------------------------------------------------------------ */

Context *GetContext() {
	return (Context*)get_context();
}

/* ------------------------------------------------------------------------ */

void SetConfig( int (*callback)( const char * ) ) {
	GetContext()->get_config = callback;
}

/* ------------------------------------------------------------------------ */

void SetDebugOutput( void (*callback)( const char * ) ) {
	GetContext()->debug_out = callback;
}

/* ------------------------------------------------------------------------ */

void DebugMessage(WString explain, const char *file, int line ) {
	WString msg = (file?((WString)file+"("+itos(line)+") : "):"Unknown : ")+explain;
#ifdef _DEBUG
	_RPT0(_CRT_WARN,(msg+"\n").c_str());
#endif
	Context *c = GetContext();
	if( c->debug_out != NULL )
		c->debug_out(msg.c_str());
	else
		MessageBox(c->target,msg.c_str(),"HSLib Debug",MB_OK | MB_ICONERROR );
}

/* ------------------------------------------------------------------------ */

void InitContext() {
	Context *c = (Context*)alloc_context(sizeof(Context));
}

/* ------------------------------------------------------------------------ */

bool Init( HWND hwnd ) {
	Context *c = GetContext();	
	if( c->get_config == NULL )
		c->get_config = no_config;
	c->target = hwnd;
	if( DIERROR(DirectInput8Create(GetModuleHandle(NULL),DIRECTINPUT_VERSION,IID_IDirectInput8,(void**)&c->object,NULL),"DirectInput Create") ) {
		Close();
		return false;
	}

	DIPROPDWORD dipdw = { {sizeof(DIPROPDWORD),sizeof(DIPROPHEADER),0,DIPH_DEVICE,},DI_BUFFERSIZE,};
	if( DIERROR(c->object->CreateDevice(GUID_SysMouse,&c->mouse,NULL),"Create Mouse") )  {
		Close();
		return false;
	}
	c->mouse->SetDataFormat(&c_dfDIMouse);
	c->mouse->SetCooperativeLevel(c->target,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
	c->mouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
	if( DIERROR(c->object->CreateDevice(GUID_SysKeyboard,&c->keyb,NULL),"Create Keyboard") ) {
		Close();
		return false;
	}
	c->keyb->SetDataFormat(&c_dfDIKeyboard);
	c->keyb->SetCooperativeLevel(c->target,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	c->keyb->Acquire();

	EMPTY_KEYS();
	EMPTY_BUTTONS();

	int i;
	for(i=0;i<MAX_KEY;i++){
		c->ktmpState[i].refreshed = true;
		c->ktmpState[i].states.Clean();
		assert(c->ktmpState[i].states.Empty());
		c->krepeat[i] = DEFAULT_REPEAT;
	}
	c->krepeat[MOUSE_BLEFT] = -1;
	c->krepeat[MOUSE_BRIGHT] = -1;
	c->krepeat[MOUSE_BMIDDLE] = -1;

	DIPROPSTRING dips;
	dips.diph.dwSize = sizeof(DIPROPSTRING);
	dips.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dips.diph.dwHow = DIPH_BYOFFSET;
	dipdw.diph.dwHow = DIPH_BYOFFSET;

	// compute scancodes+keynames tables
	for(i=0;i<MAX_KEY;i++) {
		dipdw.diph.dwObj = i;
		dips.diph.dwObj = i;
		if( c->keyb->GetProperty(DIPROP_SCANCODE, &dipdw.diph) == S_OK ) 
			c->scancodes[i] = dipdw.dwData;
		else
			c->scancodes[i] = -1;
		switch( c->scancodes[i] ) {
		KEYNAME(1,ESC);
		KEYNAME(14,BACKSPACE);
		KEYNAME(28,ENTER);
		KEYNAME(29,LCTRL);
		KEYNAME(42,LSHIFT);
		KEYNAME(54,RSHIFT);
		KEYNAME(56,LALT);
		KEYNAME(57,SPACE);
		KEYNAME(71,NUM7);
		KEYNAME(72,NUM8);
		KEYNAME(73,NUM9);
		KEYNAME(75,NUM4);
		KEYNAME(76,NUM5);
		KEYNAME(77,NUM6);
		KEYNAME(79,NUM1);
		KEYNAME(80,NUM2);
		KEYNAME(81,NUM3);
		KEYNAME(82,NUM0);
		KEYNAME(83,NUMDOT);
		KEYNAME(7392,NUMENTER);
		KEYNAME(7648,RCTRL);
		KEYNAME(13792,NUMDIV)
		KEYNAME(14560,RALT);
		KEYNAME(18400,HOME);
		KEYNAME(18656,UP);
		KEYNAME(18912,PGUP);
		KEYNAME(19424,LEFT);
		KEYNAME(19936,RIGHT);
		KEYNAME(20448,END);
		KEYNAME(20704,DOWN);
		KEYNAME(20960,PGDOWN)
		KEYNAME(21216,INS);
		KEYNAME(21472,DEL);
		KEYNAME(23520,LWIN);
		KEYNAME(23776,RWIN);
		default:
			if( c->keyb->GetProperty(DIPROP_KEYNAME, &dips.diph) == S_OK )
				c->keynames[i] = strdup(_com_util::ConvertBSTRToString(dips.wsz));
			else
				c->keynames[i] = NULL;
			break;
		}
	}

	c->scancodes[MOUSE_BLEFT] = MOUSE_SCAN_BLEFT;
	c->scancodes[MOUSE_BRIGHT] = MOUSE_SCAN_BRIGHT;
	c->scancodes[MOUSE_BMIDDLE] = MOUSE_SCAN_BMIDDLE;
	c->keynames[MOUSE_BLEFT] = strdup("MOUSELEFT");
	c->keynames[MOUSE_BRIGHT] = strdup("MOUSERIGHT");
	c->keynames[MOUSE_BMIDDLE] = strdup("MOUSEMIDDLE");

	return true;
}

/* ------------------------------------------------------------------------ */

void Close() {
	Context *c = GetContext();
	if( c == NULL )
		return;
	EMPTY_KEYS();
	EMPTY_BUTTONS();
	int i;
	for(i=0;i<MAX_KEY;i++)
		if( c->keynames[i] ) {
			free(c->keynames[i]);
			c->keynames[i] = NULL;
		}
	if( c->keyb ) {
		c->keyb->Unacquire();
		c->keyb->Release();
		c->keyb = NULL;
	}
	if( c->mouse ) {
		c->mouse->Unacquire();
		c->mouse->Release();
		c->mouse = NULL;
	}
	if( c->object ) {
		c->object->Release();
		c->object = NULL;
	}
	free_context();
}

/* ------------------------------------------------------------------------ */

void RefreshKeyb( DWORD delta, Context *c, bool process ) {	
	HRESULT rval;
	int i;
	int tmp_size = 256;
	if(process) {
		switch( (rval = c->keyb->GetDeviceState(tmp_size,&c->ktmp)) )
		{
		case DI_OK:
		case DI_BUFFEROVERFLOW:
			break;
		case DIERR_NOTACQUIRED:
		case DIERR_INPUTLOST:
			EMPTY_KEYS();
			c->keyb->Acquire();		
			return;
		default :
			EMPTY_KEYS();
			DEBUG("GetKeyboardState failed : #"+itos(rval));
			return;
		}
	}
	
	for(i=0; i<tmp_size;i++) {
		c->ktmpState[i].refreshed = true;
		if( c->ktmp[i] & 0x80 ) {
			if( c->keys[i] <= 0 )
				c->keys[i] = 1;
			else {
				c->keys[i] += delta;
				if( c->keys[i] == 1 )
					c->keys[i] = 2;
				if( c->krepeat[i] >= 0 && c->keys[i] > c->krepeat[i] )
					c->keys[i] = -c->keys[i];
			}
		}
		else {
			if( c->keys[i] > 0 )
				c->keys[i] = -c->keys[i];
			else
				c->keys[i] = 0;
		}
		if(!c->ktmpState[i].states.Empty()){
			c->ktmp[i] = c->ktmpState[i].states.First();
			c->ktmpState[i].states.Remove(c->ktmp[i]);
			c->ktmpState[i].refreshed = false;
		}
	}
}

/* ------------------------------------------------------------------------ */

void RefreshMouse( DWORD delta, Context *c, bool process ) {
	HRESULT rval;
	DWORD nb = DI_BUFFERSIZE;
	DIDEVICEOBJECTDATA mbuf[DI_BUFFERSIZE];
	DWORD i;

	if(process) {
		switch( (rval = c->mouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),mbuf,&nb,0)) )
		{
		case DI_OK:
		case DI_BUFFEROVERFLOW:
			break;
		case DIERR_NOTACQUIRED:
		case DIERR_INPUTLOST:
			EMPTY_BUTTONS();
			c->mouse->Acquire();
			return;
		default:
			EMPTY_BUTTONS();
			DEBUG("GetMouseState failed : #"+itos(rval));
			return;
		}
	
		for (i=0;i<nb;i++)
		{
			switch (mbuf[i].dwOfs)
			{
			case DIMOFS_BUTTON0:
				c->mb_state[0] <<= 2;
				c->mb_state[0] |= (mbuf[i].dwData & 0x80)?1:2;
				break;
			case DIMOFS_BUTTON1: 
				c->mb_state[1] <<= 2;
				c->mb_state[1] |= (mbuf[i].dwData & 0x80)?1:2;
				break;
			case DIMOFS_BUTTON2:
				c->mb_state[2] <<= 2;
				c->mb_state[2] |= (mbuf[i].dwData & 0x80)?1:2;
				break;
			}
		}
	}

	for(i=MOUSE_BLEFT;i<MOUSE_BLEFT+3;i++) {
		c->ktmpState[i].refreshed = true;
		unsigned int s = c->mb_state[i-MOUSE_BLEFT];
		unsigned int mask = 3;
		while( s >> 2 ) {
			s >>= 2;
			mask <<= 2;
		}
		c->mb_state[i-MOUSE_BLEFT] &= ~mask;
		switch( s ) {
		case 2: // released
			if( c->keys[i] > 0 )
				c->keys[i] = -c->keys[i];
			else
				c->keys[i] = 0;
			break;
		case 1: // down
			if( c->keys[i] <= 0 )
				c->keys[i] = 1;
			else {
				c->keys[i] += delta;
				if( c->keys[i] == 1 )
					c->keys[i] = 2;
				if( c->krepeat[i] >= 0 && c->keys[i] > c->krepeat[i] )
					c->keys[i] = -c->keys[i];
			}
			break;
		case 0: // no change
			if( c->keys[i] < 0 )
				c->keys[i] = 0;
			else if( c->keys[i] > 0 ) {
				c->keys[i] += delta;
				if( c->keys[i] == 1 )
					c->keys[i] = 2;
				if( c->krepeat[i] >= 0 && c->keys[i] > c->krepeat[i] )
					c->keys[i] = -c->keys[i];
			}
			break;
		}
		if(!c->ktmpState[i].states.Empty()){
			c->mb_state[i-MOUSE_BLEFT] = c->ktmpState[i].states.First();
			c->ktmpState[i].states.Remove(c->ktmp[i]);
			c->ktmpState[i].refreshed = false;
		}
	}

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(c->target,&pt);
	c->mouse_x = pt.x;
	c->mouse_y = pt.y;
}

/* ------------------------------------------------------------------------ */

void Refresh( bool process ) {
	int i = 0;
	Context *c = GetContext();
	DWORD delta = GetTickCount()-c->lastick;
	c->lastick += delta;
	c->delta = delta;
	RefreshMouse(delta,c,process);
	RefreshKeyb(delta,c,process);
}

/* ------------------------------------------------------------------------ */

void EmitEvent( unsigned int msg, unsigned int wp, unsigned int lp ) {
	Context *c = GetContext();
	if(!c)
		return;
	unsigned int scancode = 0;
	int i = 0;
	char v = 0x00;
	switch( msg ) {
	case WM_KILLFOCUS:
		c->available = false;
		for(i=0;i<MAX_KEY;i++){
			if(c->ktmpState[i].refreshed)
				if(i < MOUSE_BLEFT)
					c->ktmp[i] = 0x00;
				else
					c->mb_state[i - MOUSE_BLEFT] = 2;
			else
				if(i < MOUSE_BLEFT)
                    c->ktmpState[i].states.Add(0x00);
				else
					c->ktmpState[i].states.Add(2);

			c->ktmpState[i].refreshed = false;
		}
		return;
	case WM_LBUTTONDOWN:
		c->available = true;
		scancode = MOUSE_BLEFT;
		v = 1;
		break;
	case WM_LBUTTONUP:
		scancode = MOUSE_BLEFT;
		v = 2;
		break;
	case WM_RBUTTONDOWN:
		scancode = MOUSE_BRIGHT;
		v = 1;
		break;
	case WM_RBUTTONUP:
		scancode = MOUSE_BRIGHT;
		v = 2;
		break;
	case WM_MBUTTONDOWN:
		scancode = MOUSE_BMIDDLE;
		v = 1;
		break;
	case WM_MBUTTONUP:
		scancode = MOUSE_BMIDDLE;
		v = 2;
		break;
	case WM_KEYDOWN:
		v = (char)0x80;
	case WM_KEYUP:
		if((lp & 65535) != 1)
			return;
		scancode = (lp>>16 & 255) + (lp>>17 & 128);
		if(wp == VK_TAB)
			c->available = true;
		if(!c->available)
			return;
		break;
	default:
		MessageBox(NULL,wprintf("%d (%d,%d)",msg,wp,lp).c_str(),"Unhandled message",MB_OK);
		return;
	}
	if(c->ktmpState[scancode].refreshed) {
		if(scancode < MOUSE_BLEFT)
			c->ktmp[scancode] = v;
		else
			c->mb_state[scancode - MOUSE_BLEFT] = v;
	}
	else
		c->ktmpState[scancode].states.Add(v);
	c->ktmpState[scancode].refreshed = false;
}

/* ------------------------------------------------------------------------ */

void SetProgress(double progress) {
	Context *c = GetContext();
	c->progress = progress;
}

/* ------------------------------------------------------------------------ */

LibData *GetData() {
	return (LibData*)GetContext();
}

/* ------------------------------------------------------------------------ */
}; // namespace
/* ************************************************************************ */
