/* **************************************************************************** */
/*																				*/
/*   MDX - DirectX Defines & Types												*/
/*	 Nicolas Cannasse															*/
/*	 (c)2002 Motion-Twin														*/
/*																				*/
/* **************************************************************************** */
#pragma once
#include <d3d8.h>
#include <d3dx8.h>
#undef CreateFont

/* ---------------------------------------------------------------------------- */

typedef D3DXMATRIX		MATRIX;
typedef D3DXVECTOR3		VECTOR;
typedef D3DXVECTOR2		VECTOR2;
typedef D3DXVECTOR4		VECTOR4;
typedef D3DXQUATERNION	QUATERNION;
typedef D3DXCOLOR		COLOR;
typedef LPD3DXFONT		FONT;

typedef D3DLIGHT8				LIGHT;
typedef D3DCOLORVALUE			COLORVALUE;
typedef D3DMATERIAL8			MATERIAL;
typedef LPDIRECT3DTEXTURE8		TEXTURE;
typedef LPDIRECT3DDEVICE8		DEVICE;
typedef LPDIRECT3DVERTEXBUFFER8 VBUF;
typedef LPDIRECT3DINDEXBUFFER8	IBUF;
typedef LPDIRECT3D8				OBJECT;
typedef LPDIRECT3DSURFACE8		SURFACE;

typedef D3DTEXTUREOP			TEXTUREOP;
typedef D3DBLEND				BLEND;

/* ---------------------------------------------------------------------------- */

#define mmult	D3DXMatrixMultiply
#define mrx		D3DXMatrixRotationX
#define mry		D3DXMatrixRotationY
#define mrz		D3DXMatrixRotationZ
#define mrot(m,x,y,z)	D3DXMatrixRotationYawPitchRoll(m,x,y,z)
#define mscale	D3DXMatrixScaling
#define mtrans	D3DXMatrixTranslation
#define mid		D3DXMatrixIdentity
#define misid	D3DXMatrixIsIdentity
#define mtranspose	D3DXMatrixTranspose
#define minv(mout,min) D3DXMatrixInverse(mout,NULL,min)
#define vnormalize D3DXVec3Normalize
#define vlength	D3DXVec3Length
#define vcross	D3DXVec3Cross
#define vlerp D3DXVec3Lerp
#define vtransform D3DXVec4Transform
#define vtransform3 D3DXVec3TransformNormal
#define mquaternion D3DXQuaternionRotationMatrix
#define qslerp D3DXQuaternionSlerp
#define qnormalize D3DXQuaternionNormalize
#define clerp D3DXColorLerp

#define NODELETE(x) // tip : the non deletion of the ptr is minded !

/* ---------------------------------------------------------------------------- */

typedef CRITICAL_SECTION		LOCK;

#ifdef USE_R_THREAD
#	define INITLOCK(l)		InitializeCriticalSection(&(l))
#	define DELETELOCK(l)	DeleteCriticalSection(&(l))
#	define BEGINLOCK(l)		EnterCriticalSection(&(l))
#	define ENDLOCK(l)		LeaveCriticalSection(&(l))
#else
#	define INITLOCK(l)
#	define DELETELOCK(l)
#	define BEGINLOCK(l)
#	define ENDLOCK(l)
#endif


/* ---------------------------------------------------------------------------- */

DWORD color_lerp( DWORD ca, DWORD cb, FLOAT delta );

typedef struct FRECT {
	FLOAT left;
	FLOAT top;
	FLOAT right;
	FLOAT bottom;
	FRECT() { }
	FRECT( FLOAT left, FLOAT top, FLOAT right, FLOAT bottom ) :
		left(left),top(top),right(right),bottom(bottom) { }
} FRECT;

/* **************************************************************************** */
