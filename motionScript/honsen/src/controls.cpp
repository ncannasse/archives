/* ************************************************************************ */
/*																			*/
/*	Honsen Lib																*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include <string.h>
#include "controls.h"
#include "../common/context.h"
#include "../common/wlist.h"
#ifdef _WIN32

#include <windows.h>

int ScanOfKey( int key ) {
	int scan = MapVirtualKey(key,0);
	if( scan == 0 )
		return -1;
	switch( key ) {
	case VK_INSERT:
	case VK_DELETE:
	case VK_HOME:
	case VK_END:
	case VK_NEXT:
	case VK_PRIOR:
	case VK_LEFT:
	case VK_RIGHT:
	case VK_UP:
	case VK_DOWN:
	case VK_RMENU:
	case VK_LWIN:
	case VK_RWIN:
	case VK_RCONTROL:
		scan = (scan << 8) | 0xe0;
		break;
	}
	return scan;
}

char *NameOfScan( int scan ) {
	char tmpname[100];
	if( scan == -1 )
		return NULL;
	if( GetKeyNameText(scan<<16,tmpname,100) > 0 )
		return strdup(tmpname);
	return NULL;
}

#endif

#define KEYNAME(code,name)  case code: c->keynames[i] = strdup(#name); break;
#define DEFAULT_REPEAT		200

struct KeyState {
	bool down;
	WList<bool> queue;
};

struct _controls : public controls_data {
	KeyState kstates[MAX_KEY];
	int lcontrol;
};

_context *controls_context = context_new();

/* ------------------------------------------------------------------------ */

controls *controls_init() {
	controls *c = new _controls;
	c->lcontrol = 0;
	int i;
	for(i=0;i<MAX_KEY;i++){
		c->kstates[i].down = false;
		c->krepeat[i] = DEFAULT_REPEAT;
	}
	c->krepeat[MOUSE_BLEFT] = -1;
	c->krepeat[MOUSE_BRIGHT] = -1;
	c->krepeat[MOUSE_BMIDDLE] = -1;
	// compute scancodes+keynames tables
	for(i=0;i<MAX_KEY;i++) {
		c->scancodes[i] = ScanOfKey(i);
		if( c->scancodes[i] == 29 && c->lcontrol == 0 ) c->lcontrol = i;
		switch( c->scancodes[i] ) {
		KEYNAME(1,ESC);
		KEYNAME(14,BACKSPACE);
		KEYNAME(15,TAB);
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
		KEYNAME(7648,RCTRL);
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
			c->keynames[i] = NameOfScan(c->scancodes[i]);
			break;
		}
	}
	c->scancodes[MOUSE_BLEFT] = MOUSE_SCAN_BLEFT;
	c->scancodes[MOUSE_BRIGHT] = MOUSE_SCAN_BRIGHT;
	c->scancodes[MOUSE_BMIDDLE] = MOUSE_SCAN_BMIDDLE;
	c->keynames[MOUSE_BLEFT] = strdup("MOUSELEFT");
	c->keynames[MOUSE_BRIGHT] = strdup("MOUSERIGHT");
	c->keynames[MOUSE_BMIDDLE] = strdup("MOUSEMIDDLE");
	context_set_data(controls_context,c);
	return c;
}

/* ------------------------------------------------------------------------ */

controls_data *controls_get_data() {
	return (controls_data*)context_get_data(controls_context);
}

/* ------------------------------------------------------------------------ */

void controls_destroy( controls *c ) {
	int i;
	for(i=0;i<MAX_KEY;i++) {
		if( c->keynames[i] ) {
			delete c->keynames[i];
			c->keynames[i] = NULL;
		}
	}
	delete c;
}

/* ------------------------------------------------------------------------ */

void controls_refresh( controls *c, unsigned int delta ) {	
	int i;
	for(i=0;i<MAX_KEY;i++) {
		KeyState *s = &c->kstates[i];
		if( !s->queue.Empty() )
			s->down = s->queue.Pop();
		int v = c->keys[i];
		if( s->down ) {
			if( v > 0 ) {
				v += delta;
				if( v == 1 )
					v = 2;
				if( c->krepeat[i] >= 0 && v > c->krepeat[i] )
					v = 1;
			} else
				v = 1;
		} else {
			if( v > 0 )
				v = -v;
			else
				v = 0;
		}
		c->keys[i] = v;
	}
}

/* ------------------------------------------------------------------------ */

void controls_event( controls *c, CONTROLEVENT evt, unsigned int key ) {
	int i;
	switch( evt ) {
	case EVT_LOSTFOCUS:
		for(i=0;i<MAX_KEY;i++) {
			bool down = c->kstates[i].queue.Empty()?c->kstates[i].down:c->kstates[i].queue.Last();
			if( down )
				c->kstates[i].queue.Add(false);
		}
		return;
	case EVT_KEY_DOWN:
	case EVT_KEY_UP:
		if( key < 0 || key >= MAX_KEY )
			return;
		{
			bool edown = (evt == EVT_KEY_DOWN); 
			bool kdown = c->kstates[key].queue.Empty()?c->kstates[key].down:c->kstates[key].queue.Last();
			if( kdown && edown )
				break;
			if( !kdown && !edown )
				c->kstates[key].queue.Add(true);
			c->kstates[key].queue.Add(edown);
			if( c->scancodes[key] == 14560 && edown && !kdown )
				c->kstates[c->lcontrol].queue.Clean();
			break;
		}
		break;
	case EVT_SET_MOUSEPOS:
		c->mouse_x = (short)(key & 0xFFFF);
		c->mouse_y = (short)(key >> 16);
		break;
	}
}

/* ************************************************************************ */
