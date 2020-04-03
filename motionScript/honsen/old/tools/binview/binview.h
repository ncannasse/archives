#pragma once

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Includes Standard
//
#include <wtypes.h>    // DWORD...
#include <commctrl.h>

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Includes Project
//
#include "../../common/mtw.h"

#define ISTEXT(t)	(t->tag == MAKETAG(NAME) || t->tag == MAKETAG(TEX0) || t->tag == MAKETAG(RSNM))

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Definitions
//

#define SAFE_DELETE(p)       { if((p)) { delete (p); (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if((p)) { delete[] (p); (p)=NULL; } }

class M3dView;

class BinView
{

private:

    HWND        treeView;

    WString     filePath;                   // path of the .mtw file
    MTW			*rootTag, *rootTagSave;     // two list of tags to undo changes 
    DWORD       rootSize;                   // to compute tag sizes in %
    bool        modified;                   // to undo the last modif only
    int         sizeView;                   // current view of tag sizes
	FILETIME	lastWrite;
	M3dView		*m3dview;

public:

    enum { VIEW_SIZE_NONE=0, VIEW_SIZE_PRCT, VIEW_SIZE_BYTE, VIEW_SIZE_UNIT };

    //-------------------------------------------------------------------------
    // Constructor & Destructor
    //
    BinView(M3dView *m3dview);
    ~BinView();

    void        SetTreeView(HWND tv) { treeView = tv; }

    //-------------------------------------------------------------------------
    //
    BOOL        Open(const WString path);
    BOOL        Close();

    BOOL        Save() const;
    BOOL        SaveAs(const WString path) const;

    BOOL        Refresh();
    BOOL        Refresh(int);

    BOOL        Reload();
	BOOL		reloadTime();

    BOOL        Undo();

    //-------------------------------------------------------------------------
    // construction of the tree
    //
    BOOL        CreateTreeView(HINSTANCE hInst, const HWND parentWindow);
    BOOL        CreateItems(HTREEITEM parentItem, MTW* obj);
    float       ComputePrctSize(MTW* tag);
    int         ComputeUnitSize(MTW* tag);

    HTREEITEM   AddItem(HTREEITEM parentItem, MTW* tag);

    //-------------------------------------------------------------------------
    // init the viewer
    //
    // BOOL        LoadObject(const WString path);

    //-------------------------------------------------------------------------
    // modification of the tree
    //
    MTW*        GetSelectedTag();
    BOOL        SetModified();
    //BOOL      SetSelectedTag(Tag* tag);
    //BOOL      EditItem(Tag* tag);
    BOOL        DeleteItem();
    BOOL        DeleteTag(MTW* tag);
};
