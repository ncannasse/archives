#pragma once
#include "plugin.h"

void CreateInterface();

bool NewObjClick( OBJECT );
bool DelObjClick( OBJECT );
bool MoveUPClick( OBJECT );

bool ListObjClick( OBJECT );

bool OnDropPAKItem( LPCSTR anim, LPCSTR id );
bool OnSaveProject( LPCSTR );
bool OnLoadProject( LPCSTR );

bool ParamChange( OBJECT , void *Key );

bool GetIndex( OBJECT );
bool MoveUpFrame( OBJECT );
bool DeleteFrame( OBJECT );

bool Duplicate ( OBJECT );

bool ImportAnim( OBJECT );

void Redraw();

typedef struct TListeObjs {

	DragoonMenu   MainMenu;
	DragoonMenu   ImportMenu;
	List		  ImportList;

	DragoonButton MainButton;

	Label label_animation;
	Label label_nom;

	Label obj_label;
	List obj_list;

	Button obj_button;
	Button obj_del;
	Button obj_moveup;

	Grid obj_grid;

	Label obj_lab_fin;
	Label obj_lab_debut;

	Button run_button;

	Button move_up_frame;
	Button delete_frame;

	Button duplicate;

	Edit anm_params[3];

} TListeObjs;

extern TListeObjs Objets;

