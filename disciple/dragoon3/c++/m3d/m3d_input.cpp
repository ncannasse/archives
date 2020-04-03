/* **************************************************************************** */
/*																				*/
/* Caml Bindings for Input Devices												*/
/*		(c)2002 Motion-Twin														*/
/*																				*/
/* **************************************************************************** */

#include "m3d_caml.h"
#include <m3d.h>
#include "m3d_keys.h"

/* **************************************************************************** */

#define EMPTY(tab)	memset(tab,0,sizeof(tab));

static LPDIRECTINPUT8 diobj = NULL;
static LPDIRECTINPUTDEVICE8 dikeyb = NULL, dimouse = NULL;
static int keys[256];
static int buttons[3];
static int krepeat = 200;
static const char *keynames[256];
static DWORD lastick = 0;
static FLOAT mouse_xspeed = 0.004f, mouse_yspeed = 0.004f;
static HWND target;
FLOAT mouse_x = 0.0f, mouse_y = 0.0f;


extern "C" {

/* **************************************************************************** */
// ALLOC

value alloc_state( int s ) {
	value v;
	if( s == 0 )
		return Val_int(0);
	if( s == 1 )
		return Val_int(1);
	if( s == -1 )
		return Val_int(2);
	v = alloc_small(1,0);
	Field(v,0) = Val_int(s);
	return v;
}


/* **************************************************************************** */
// KEYBOARD

CAMLprim value winput_get_key( value name ) {
	static value *exc = caml_named_value("input keyname exception");
	int i;
	for(i=0;i<ALLKEYSIZE;i++)
		if( strcmpi(allkeys[i].name,String_val(name)) == 0 )
			return Val_int(allkeys[i].code);
	raise_with_arg(*exc,name);
	return Val_unit;
}

CAMLprim value winput_name_of_key( value key ) {
	return copy_string(keynames[Int_val(key)]);
}

CAMLprim value winput_key_state( value key ) {
	return alloc_state(keys[Int_val(key)]);
}

CAMLprim value winput_current_keys( value _ ) {
	CAMLparam1(_);
	CAMLlocal2(list,tmp);
	int i;	
	list = Val_int(0);
	for(i=0;i<256;i++)
		if( keys[i] > 0 ) {
		    tmp = alloc(2, 0);
			Store_field(tmp, 0, Val_int(i));
			Store_field(tmp, 1, list);
			list = tmp;
		}
	CAMLreturn(list);
}

void RefreshKeyb( DWORD delta ) {
	static char ktmp[256];
	HRESULT rval;
	int i;

	switch( (rval = dikeyb->GetDeviceState(sizeof(ktmp),&ktmp)) )
	{
	case DI_OK:
	case DI_BUFFEROVERFLOW:
		break;
	case DIERR_NOTACQUIRED:
	case DIERR_INPUTLOST:
		EMPTY(keys);
		dikeyb->Acquire();		
		return;
	default :
		EMPTY(keys);
		M3DTRACE("GetKeyboardState failed : #"+itos(rval));
		return;
	}
	
	for(i=0; i<sizeof(ktmp);i++)
		if( ktmp[i] & 0x80 )
		{
			if( keys[i] < 0 )
				keys[i] = delta?delta:1;
			else {
				if( keys[i] == 0 )
					keys[i] = 1;
				else {
					keys[i] += delta;
					if( keys[i] == 1 )
						keys[i] = 2;
					if( keys[i] > krepeat )
						keys[i] = -1;
				}
			}
		}
		else
		{
			if( keys[i] > 0 )
				keys[i] = -1;
			else
				keys[i] = 0;
		}
}

/* **************************************************************************** */
// MOUSE

void RefreshButton( int bnb, int state, DWORD delta ) {
	switch( state )
	{
	case -1:
		switch( buttons[bnb] )
		{
		case 0:
			break;
		case -1:
			buttons[bnb] = 0;
			break;
		default:
			buttons[bnb]+=delta;
			if( buttons[bnb] == 1 )
				buttons[bnb] = 2;
			break;
		}
		break;
	case 0:
		buttons[bnb] = -1;
		break;
	case 1:
		buttons[bnb] = 1;
		break;
	}
}

void RefreshMouse( DWORD delta ) {
	HRESULT rval;
	DWORD nb = DI_BUFFERSIZE;
	DIDEVICEOBJECTDATA mbuf[DI_BUFFERSIZE];
	char state[3] = {-1,-1,-1};
	int i;

	switch( (rval = dimouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),mbuf,&nb,0)) )
	{
	case DI_OK:
	case DI_BUFFEROVERFLOW:
		break;
	case DIERR_NOTACQUIRED:
	case DIERR_INPUTLOST:
		EMPTY(buttons);
		dimouse->Acquire();
		return;
	default:
		EMPTY(buttons);
		M3DTRACE("GetMouseState failed : #"+itos(rval));
		return;
	}
	
	for (i=0;i<nb;i++)
	{
		switch (mbuf[i].dwOfs)
		{
		case DIMOFS_BUTTON0:
			state[0] = (mbuf[i].dwData & 0x80)?1:0;
			break;
		case DIMOFS_BUTTON1: 
			state[1] = (mbuf[i].dwData & 0x80)?1:0;
			break;
		case DIMOFS_BUTTON2:
			state[2] = (mbuf[i].dwData & 0x80)?1:0;
			break;
		case DIMOFS_X :
			mouse_x += ((long)mbuf[i].dwData * mouse_xspeed);
			break;
		case DIMOFS_Y :
			mouse_y += ((long)mbuf[i].dwData * mouse_yspeed);
			break;
		}
	}

	for(i=0;i<3;i++)
		RefreshButton(i,state[i],delta);

	if( mouse_x < -1.0f )
		mouse_x = -1.0f;
	else if( mouse_x > 1.0f )
		mouse_x = 1.0f;

	if( mouse_y < -1.0f )
		mouse_y = -1.0f;
	else if( mouse_y > 1.0f )
		mouse_y = 1.0f;
}

CAMLprim value winput_mouse_pos( value _ ) {
	CAMLparam1(_);
	CAMLlocal1(p);
    p = alloc_tuple(2);
    Field(p, 0) = copy_double(mouse_x);
	Field(p, 1) = copy_double(mouse_y);
	CAMLreturn(p);
}

CAMLprim value winput_set_mouse_pos( value p ) {
	mouse_x = Double_val( Field(p,0) );
	mouse_y = Double_val( Field(p,1) );
	return Val_unit;
}

CAMLprim value winput_mouse_left( value _ ) {
	return alloc_state(buttons[0]);
}

CAMLprim value winput_mouse_right( value _ ) {
	return alloc_state(buttons[1]);
}

CAMLprim value winput_mouse_middle( value _ ) {
	return alloc_state(buttons[2]);
}

CAMLprim value winput_mouse_release( value _ ) {
	dimouse->Unacquire();
	return Val_unit;
}

CAMLprim value winput_mouse_reset_left( value _ ) {
	buttons[0] = 1;
	return Val_unit;
}

/* **************************************************************************** */
// INIT / CLOSE / PROCESS

CAMLprim value winput_close( value _ ) {
	EMPTY(keys);
	EMPTY(buttons);

	if( dikeyb ) {
		dikeyb->Unacquire();
		dikeyb->Release();
		dikeyb = NULL;
	}
	if( dimouse ) {
		dimouse->Unacquire();
		dimouse->Release();
		dimouse = NULL;
	}
	if( diobj ) {
		diobj->Release();
		diobj = NULL;
	}
	return Val_unit;
}

CAMLprim value winput_process( value _ ) {
	DWORD delta = GetTickCount()-lastick;

	lastick += delta;
	RefreshMouse(delta);
	RefreshKeyb(delta);

	return Val_unit;
}

CAMLprim value winput_init( value hwnd ) {
	int i;
	DIPROPDWORD dipdw = { {sizeof(DIPROPDWORD),sizeof(DIPROPHEADER),0,DIPH_DEVICE,},DI_BUFFERSIZE,};

	winput_close(Val_unit);
	DirectInput8Create(GetModuleHandle(NULL),DIRECTINPUT_VERSION,IID_IDirectInput8,(void**)&diobj,NULL);
	lastick = GetTickCount();
	target = Handle_val(hwnd);
	
	// mouse
	diobj->CreateDevice(GUID_SysMouse,&dimouse,NULL);
	dimouse->SetDataFormat(&c_dfDIMouse);
	dimouse->SetCooperativeLevel(target,DISCL_FOREGROUND | DISCL_EXCLUSIVE );
	dimouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
#ifndef _DEBUG
	dimouse->Acquire();
#endif

	// keyb
	diobj->CreateDevice(GUID_SysKeyboard,&dikeyb,NULL);
	dikeyb->SetDataFormat(&c_dfDIKeyboard);
	dikeyb->SetCooperativeLevel(target,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	dikeyb->Acquire();

	mouse_x = mouse_y = 0;
	EMPTY(buttons);
	EMPTY(keys);

	for(i=0;i<256;i++)
		keynames[i] = "???";
	for(i=0;i<ALLKEYSIZE;i++)
		keynames[allkeys[i].code] = allkeys[i].name;

	return Val_true;
}

}; // extern
/* **************************************************************************** */
