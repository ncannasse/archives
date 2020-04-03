/* ************************************************************************ */
/*																			*/
/*	Honsen Runtime															*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once

#define MOUSE_FIRST		MOUSE_BLEFT
#define MOUSE_BLEFT		(0xFF + 1)
#define MOUSE_BRIGHT	(0xFF + 2)
#define MOUSE_BMIDDLE	(0xFF + 3)

#define MOUSE_SCAN_BLEFT	(0xFFFF - 0)
#define MOUSE_SCAN_BRIGHT	(0xFFFF - 1)
#define MOUSE_SCAN_BMIDDLE	(0xFFFF - 2)

#define MAX_KEY 259
#define INVALID_KEY	MAX_KEY

typedef struct {
	char *keynames[MAX_KEY];
	int keys[MAX_KEY];
	int scancodes[MAX_KEY];
	int krepeat[MAX_KEY];
	int mouse_x;
	int mouse_y;
} controls_data;

typedef struct _controls controls;

enum CONTROLEVENT {
	EVT_KEY_DOWN,
	EVT_KEY_UP,
	EVT_SET_MOUSEPOS,
	EVT_LOSTFOCUS
};

#define MOUSE_POS(x,y) (((unsigned short)x) | (((unsigned short)y) << 16))

controls *controls_init();
controls_data *controls_get_data();
void controls_destroy( controls *c );
void controls_refresh( controls *c, unsigned int delta );
void controls_event( controls *c, CONTROLEVENT event, unsigned int key );

/* ************************************************************************ */
