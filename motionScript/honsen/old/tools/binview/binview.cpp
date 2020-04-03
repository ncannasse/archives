//*****************************************************************************
//*                               MTW Editor
//*                           (c)2003 Motion-Twin
//*
//*     Author   :  M.B.
//*
//*     Desc     :  BinView - class implementation
//*
//*     Date     :  Observations                            :   Author
//*
//*     23.05.03    enable MenuItems, Undo
//*     22.05.03    Edit Tag ok
//*     21.05.03    View menu : dynamic display of sizes 
//*     20.05.03    Open, Close, Delete
//*     19.05.03    Menus
//*     14.05.03    Data & Sizes of items (percent & unit)
//*     12.05.03    Tree from mtw ok
//*     10.05.03    Creation                                    M.B.
//*
//*****************************************************************************

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Includes - Standard
//

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Includes - Project
//
#include "BinView.h"
#include "m3dview.h"
#include "../../hslib/hslib.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
//

#define SPACE_DATA      " "
#define SPACE_SIZE      1


//-----------------------------------------------------------------------------
// Constructor & Destructor
//
BinView::BinView(M3dView *m3dview):m3dview(m3dview)
{
    treeView = NULL;

    rootTag  = NULL;
    rootTagSave = NULL;
    rootSize = 0;
    modified = false;

    sizeView = VIEW_SIZE_NONE;
	lastWrite.dwHighDateTime = 0;
	lastWrite.dwLowDateTime = 0;
}

BinView::~BinView()
{
	Close();
}


//-----------------------------------------------------------------------------
// Open
// creates the tag list from a mtw file path and call CreateItems
//
BOOL
BinView::Open(const WString path)
{
    // close document before opening a new one
    if(rootTag)
        if(!Close())
            return FALSE;

    filePath = path;
	WIN32_FILE_ATTRIBUTE_DATA wfad;
	GetFileAttributesEx(filePath, GetFileExInfoStandard, &wfad);
	lastWrite = wfad.ftLastWriteTime;

    rootTag = MTW::ReadFile(filePath);
    if(!rootTag)
        return FALSE;

    if(!CreateItems(NULL, rootTag))
    {
        //DestroyWindow(treeView);
        return FALSE;
    }

    // save
    rootTagSave = new MTW(*rootTag);

    return TRUE;
}


//-----------------------------------------------------------------------------
// 
// 
/*
BOOL
BinView::LoadObject(const WString filename) 
{
	MTW *t = MTW::ReadFile(filename);
	if( t == NULL ) {
		M3DDEBUG("Error while loading file "+filename);
		return FALSE;
	}

	MTW *tobj = t->Child(MAKETAG(Objt));
	if( tobj == NULL ) {
		delete t;
		M3DDEBUG("No object found in "+filename);
		return FALSE;
	}

	t->RemoveChild(tobj);
	delete t;

	obj = new MTW(tobj);
	if( tobj->Child(MAKETAG(MAP0)) )
		obj->SetShader("mov oPos, $VECT\nmov oT0, $MAP0");
	else
		obj->SetShader("mov oPos, $VECT");
	if( !obj->Validate() ) {
		delete obj;
		return FALSE;
	}

	return TRUE;
}

*/
//-----------------------------------------------------------------------------
// CreateItems
// creates the treeview items from the list of tags
//
BOOL
BinView::CreateItems(HTREEITEM parentItem, MTW* tag)
{
    HTREEITEM hItem = AddItem(parentItem, tag);
	if( tag->tag == MAKETAG(HSGZZ) ){
		int len = *(DWORD*)tag->data;
		char *data = new char[len];
		if( !HSLib::Unzip(data,&len,(char*)tag->data+4,tag->size-4) ) {
			delete data;
			return false;
		}
		MTW *u = MTW::ReadData(data,len);
		delete data;
		if( u == NULL )
			return false;
		FOREACH(MTW*,*u->childs);
			//parent->childs->Add(item);
			CreateItems(hItem, item);
		ENDFOR;
		u->childs->Clean();
		delete u;
	}
    if( tag->IsGroup() )
    {
        WList<MTW*> listChild = tag->Childs();
        FOREACH(MTW*, listChild)
            if(!CreateItems(hItem, item))
                return FALSE;
            TreeView_Expand(treeView, hItem, TVE_EXPAND);
        ENDFOR;
    }
    return TRUE;
}


//-----------------------------------------------------------------------------
// AddItem
// adds items to a tree-view control
// returns the handle of the newly added item
//
HTREEITEM
BinView::AddItem(HTREEITEM parentItem, MTW* tag)
{
    int offset = 0, unitSize;
    char size[20], sDataSize[20];

    if(tag->tag==MAKETAG(Root))
    {
        rootSize = tag->size;    // TODO : tagname + tagsize ?
    }

    if(tag->IsNumber())
    {
	    sprintf(sDataSize, "%ld", tag->number);
    }

    switch(sizeView)
    {
        case VIEW_SIZE_PRCT:
            sprintf(size, "%.1f%%", ComputePrctSize(tag));
            break;

        case VIEW_SIZE_BYTE:
            sprintf(size, "%ld", tag->size);
            break;

        case VIEW_SIZE_UNIT:
            // not all item have a unit size
            unitSize = ComputeUnitSize(tag);
            if(unitSize!=-1)
                sprintf(size, "%ld", unitSize);
            else
                sprintf(size, "%s", "");
            break;

        default:
            break;
    }

    // init text with tag name

    char* text = new char[256];

    memcpy(text, tag->ctag, 4);
    offset+=4;
    memcpy(text+offset, SPACE_DATA, SPACE_SIZE);
    offset+=SPACE_SIZE;

    // append data

    if( ISTEXT(tag) )
    {
        memcpy(text+offset, SPACE_DATA, SPACE_SIZE);
        offset+=SPACE_SIZE;
        memcpy(text+offset, "\"", 1);
        offset++;
        // do not copy the last character '\0'
        memcpy(text+offset, (char*)tag->data, tag->size-1);
        offset+=tag->size-1;
        memcpy(text+offset, "\"", 1);
        offset++;
    }
    else if(tag->IsNumber())
    {
        memcpy(text+offset, SPACE_DATA, SPACE_SIZE);
        offset+=SPACE_SIZE;
        memcpy(text+offset, sDataSize, strlen(sDataSize));
        offset+=strlen(sDataSize);
    }

    // append size

    if(sizeView!=VIEW_SIZE_NONE)
    {
        memcpy(text+offset, SPACE_DATA, SPACE_SIZE);
        offset+=SPACE_SIZE;

        memcpy(text+offset, size, strlen(size));
        offset+=strlen(size);
    }

    text[offset] = '\0';

    // init item struct 

    TV_ITEM item;
    item.mask = TVIF_HANDLE|TVIF_TEXT|TVIF_PARAM;
    item.pszText = text;
    item.cchTextMax = 256;
    item.lParam = (LPARAM)tag;

    TV_INSERTSTRUCT tvins;
    tvins.item = item;
    tvins.hInsertAfter = TVI_LAST;
 
    if(parentItem==NULL)
        tvins.hParent = TVI_ROOT;
    else
        tvins.hParent = parentItem;

    HTREEITEM hItem = TreeView_InsertItem(treeView, &tvins);

    delete text;

    return hItem;
}


//-----------------------------------------------------------------------------
// ComputePrctSize
// return the "percent size" of the specified tag (% of the file)
//
float
BinView::ComputePrctSize(MTW* tag)
{
    if(tag->tag==MAKETAG(Root))
        return 100.f;
    else
        return tag->size*100/(float)rootSize;
}


//-----------------------------------------------------------------------------
// ComputeUnitSize
// return the unit size of the specified tag (e.g. nb of vectors, characters)
//
int
BinView::ComputeUnitSize(MTW* tag)
{
    int size = -1;

    if(tag->IsGroup())
    {
        size = tag->Childs().Length();
    }
    else if(tag->tag==MAKETAG(VECT))
    {
        size = tag->size/(3*sizeof(float));
    }
    else if(tag->tag==MAKETAG(MAP0))
    {
        size = tag->size/(2*sizeof(float));
    }
    else if(tag->tag==MAKETAG(INDX))
    {
        size = tag->size/sizeof(WORD);
    }
    return size;
}


//-----------------------------------------------------------------------------
// Save
//
BOOL
BinView::Save() const
{
    return SaveAs(filePath);
}


//-----------------------------------------------------------------------------
// SaveAs
//
BOOL
BinView::SaveAs(const WString path) const
{
    if(rootTag)
        return rootTag->WriteFile(path);
    else
        return FALSE;
}


//-----------------------------------------------------------------------------
// Close
// delete the tag list and all items in the treeview
//
BOOL
BinView::Close()
{
    SAFE_DELETE(rootTag);
    SAFE_DELETE(rootTagSave);
    return TreeView_DeleteAllItems(treeView);
}


//-----------------------------------------------------------------------------
// Refresh
// 
//
BOOL
BinView::Refresh()
{
    return Refresh(sizeView);
}

BOOL
BinView::Refresh(int view)
{
    sizeView = view;
    if(rootTag)
    {
        TreeView_DeleteAllItems(treeView);
        return CreateItems(NULL, rootTag);
    }
    return TRUE;
}


//-----------------------------------------------------------------------------
// Undo
// 
//
BOOL
BinView::Undo()
{
    SAFE_DELETE(rootTag);
    rootTag = new MTW(*rootTagSave);
    modified = false;
    return Refresh();
}


//-----------------------------------------------------------------------------
// GetSelectedTag
// 
//
MTW*
BinView::GetSelectedTag()
{
    TV_ITEM item;
    MTW* tag = NULL;

    item.hItem = TreeView_GetSelection(treeView);
    item.mask = TVIF_PARAM;

    if(TreeView_GetItem(treeView, &item))
    {
        DWORD tagId = item.lParam;
        tag = (MTW*)tagId;
    }
	
	if( tag && tag->tag == MAKETAG(Objt) )
		m3dview->setMesh(tag);

    return tag;
}


/*
BOOL
BinView::SetSelectedTag(MTW* t)
{
    TV_ITEM item;
    MTW* tag = NULL;

    item.hItem = TreeView_GetSelection(treeView);
    item.mask = TVIF_PARAM;

    if(TreeView_GetItem(treeView, &item))
    {
        DWORD tagId = item.lParam;
        tag = (MTW*)tagId;

        if(tag->tag!=t->tag) 
            return FALSE;

        tag->size = t->size;

        if(t->IsNumber())
            tag->number = t->number;
        else if(t->IsText())
        {
            tag->data = new char[tag->size];
            memcpy(tag->data, t->data, tag->size);
        }

        return TRUE;
    }
    return FALSE;
}
*/


BOOL
BinView::SetModified()
{
    if(modified)
        rootTagSave = new MTW(*rootTag);

    modified = true;

    return TRUE;
}


//-----------------------------------------------------------------------------
// EditItem
// 
//
/*
BOOL
BinView::EditItem(MTW* tag)
{
    SetSelectedTag(tag);

    return Refresh();
}
*/

//-----------------------------------------------------------------------------
// DeleteItem
// retrieve the selected item, then his parent to delete in the list and in the tree
//
BOOL
BinView::DeleteItem()
{
    // si deja modifie, la sauvegrade est la precedente modif
    if(modified)
        rootTagSave = new MTW(*rootTag);

    MTW* selectedTag = GetSelectedTag();
    if(!selectedTag) 
        return FALSE;

    if(!DeleteTag(selectedTag))
        return FALSE;

    modified = true;

    return TRUE;
}


//-----------------------------------------------------------------------------
// DeleteTag
// Delete the tag corresponding to the selected item
//
BOOL
BinView::DeleteTag(MTW* selectedTag)
{
    if(selectedTag==rootTag)
    {
        SAFE_DELETE(rootTag);
        return TreeView_DeleteAllItems(treeView);
    }

    TV_ITEM parentItem;
    HTREEITEM hSelectedItem = TreeView_GetSelection(treeView);
    HTREEITEM hParentItem = TreeView_GetParent(treeView, hSelectedItem);
    parentItem.hItem = hParentItem;
    parentItem.mask = TVIF_PARAM;

    if(TreeView_GetItem(treeView, &parentItem))
    {
        int parentId = parentItem.lParam;
        if( ((MTW*)parentId)->RemoveChild(selectedTag) )
            return Reload();
    }
    return FALSE;
}


//-----------------------------------------------------------------------------
// Reload
// Reload file from tmp to recompute sizes
//
BOOL
BinView::Reload()
{
    int length = strlen(filePath.c_str());
    char* tmpFilePath = new char[length];
    memcpy(tmpFilePath, filePath.c_str(), length);
    tmpFilePath[length-3]='t';
    tmpFilePath[length-2]='m';
    tmpFilePath[length-1]='p';
    tmpFilePath[length]='\0';

    if(rootTag)
        if(rootTag->WriteFile(tmpFilePath))
            if((rootTag = MTW::ReadFile(tmpFilePath)))
                if(TreeView_DeleteAllItems(treeView))
                {
                    if(DeleteFile(tmpFilePath))
                        return CreateItems(NULL, rootTag);
                }

    return FALSE;
}

BOOL
BinView::reloadTime()
{
	if(lastWrite.dwHighDateTime == 0 && lastWrite.dwLowDateTime == 0)
		return false;

	WIN32_FILE_ATTRIBUTE_DATA wfad;
	GetFileAttributesEx(filePath, GetFileExInfoStandard, &wfad);
	if(lastWrite.dwHighDateTime == wfad.ftLastWriteTime.dwHighDateTime 
		&& lastWrite.dwLowDateTime == wfad.ftLastWriteTime.dwLowDateTime)
		return false;

	Open(filePath);
	return true;
}