/* ************************************************************************ */
/*																			*/
/*	MaxTwin																	*/
/*	Morgan Bachelier, Nicolas Cannasse										*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once

#include "../../common/wstring.h"

class MTW;
class INode;
class TriObject;
class Mesh;

class MaxObj {
	MTW *o;
	INode *node;
	TriObject *geometry;
	Mesh *mesh;
	bool free_geometry;
	bool geom_modified;
public:
	WString obj_name;
	WString log_text;

	MaxObj( MTW *o, INode *node );
	~MaxObj();

	void Log( WString line );
	bool Question( WString text );

	bool Init();
	bool IsExportMesh();
	bool IsExportTexture();

	bool ReplaceMesh();
	bool ReplaceTexture();
	void RemoveTexture();
};

/* ************************************************************************ */
