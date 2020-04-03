/* ************************************************************************ */
/*																			*/
/*	MaxTwin																	*/
/*	Morgan Bachelier, Nicolas Cannasse										*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include <max.h>
#include "../../common/mtw.h"
#include "maxtwin_obj.h"
#include "maxtwin.h"
/* ------------------------------------------------------------------------ */

Maxtwin::Maxtwin() {
    root = NULL;
}

/* ------------------------------------------------------------------------ */

Maxtwin::~Maxtwin() {
}

/* ------------------------------------------------------------------------ */

int
Maxtwin::DoExport(const TCHAR* filePath, ExpInterface* eitf, Interface* maxInterface, BOOL supPrompts, DWORD options)
{
    int pathLength = strlen(filePath); // replace extension
	((char*)filePath)[pathLength-3] = 'm';
	((char*)filePath)[pathLength-2] = 't';
	((char*)filePath)[pathLength-1] = 'w';

    Log("Start Export : " + (WString)filePath);

    root = MTW::ReadFile(filePath);
    if( root == NULL )
        root = new MTW(MAKETAG(Root));

    INode *rootNode = maxInterface->GetRootNode();
    bool err = !EnumNode(root,rootNode);

	if( !err ) {
		root->WriteFile(filePath);
		Log("File Saved : "+wprintf("%.2f KB",root->size/1024.));
	}
	delete root;
	MessageBox(NULL,log_text.c_str(),"MaxTwin : ", MB_OK | (err?MB_ICONERROR:MB_ICONINFORMATION) );
    return true;
}

/* ------------------------------------------------------------------------ */

bool
Maxtwin::EnumNode(MTW* parent, INode* node) {
    if( node == NULL ) {
		Log("NULL Node");
        return false;
	}

    if( node->IsFrozen() || node->IsHidden() )
        return true; // ignore frozen and hidden nodes

    MTW *obj = parent;
	if( IsGeomObject(node) ) {
		MTW *newo = CreateMTW(node);
		if( newo == NULL )
			return false;
		parent->AddChild(newo);
		if( newo->IsGroup() )
			obj = newo;
    }

    int nchilds = node->NumberOfChildren();
	int i;
    for(i=0;i<nchilds;i++)
        if( !EnumNode(parent,node->GetChildNode(i)) )
			return false;

    return true;
}

/* ------------------------------------------------------------------------ */

bool
Maxtwin::IsGeomObject(INode* node)
{
    char* name = node->GetName();

    Object* obj = node->EvalWorldState(0).obj;
    if( !obj )
        return false;

    SClass_ID scId = obj->SuperClassID();
    if( obj->SuperClassID() != GEOMOBJECT_CLASS_ID )
        return false;

    // ignore bones
    Class_ID cId = obj->ClassID();
    if( cId == BONE_OBJ_CLASSID || cId == Class_ID(0x9125,0) )
        return false;

    return true;
}

/* ------------------------------------------------------------------------ */

void
Maxtwin::Log( WString line ) {
	log_text += line+"\n";
}

/* ------------------------------------------------------------------------ */

MTW *FindMTWRec( MTW *p, const char *name, MTW **parent ) {
	if( p->IsGroup() ) {
		MTW *tname = p->Child(MAKETAG(NAME));
		if( tname && strcmp(name,(char*)tname->data) == 0 )
			return p;
		FOREACH(MTW*,*p->childs);
			MTW *f = FindMTWRec(item,name,parent);
			if( f != NULL ) {
				if( parent )
					*parent = p;
				return f;
			}
		ENDFOR;
	}
	return NULL;
}

/* ------------------------------------------------------------------------ */

MTW *
Maxtwin::FindMTW( const char *name, MTW **parent ) {
	if( parent )
		*parent = NULL;
	return FindMTWRec(root,name,parent);
}

/* ------------------------------------------------------------------------ */

MTW *
Maxtwin::CreateMTW( INode *node ) {
	char *node_name  = node->GetName();
	MTW *old_parent = NULL;
	MTW *o = FindMTW( node_name , &old_parent );
	if( o )
		old_parent->RemoveChild(o);
	else {
		o = new MTW(MAKETAG(Objt));
		o->AddChild(new MTW(MAKETAG(NAME), strlen(node_name)+1, strdup(node_name)));
	}

	MaxObj *m = new MaxObj(o,node);
	if( !m->Init() || !m->ReplaceMesh() || !m->ReplaceTexture() ) {
		Log("In <"+m->obj_name+"> :");
		Log(m->log_text);
		delete m;
		delete o;
		return NULL;
	}
	if( m->log_text != "" ) {
		Log("In <"+m->obj_name+"> :");
		Log(m->log_text);
	}
	delete m;
	return o;
}

/* ************************************************************************ */
