/* ************************************************************************ */
/*																			*/
/*   MDX - DirectX Defines & Types											*/
/*	 Nicolas Cannasse														*/
/*	 (c)2002 Motion-Twin													*/
/*																			*/
/* ************************************************************************ */
#pragma once

#include <d3d8.h>
#include <d3dx8.h>
#undef CreateFont

#pragma warning( default : 4244 ) // reactivate loose of precision warning
#pragma warning( disable : 4018 ) // desactive signed/unsigned comparisons

/* ------------------------------------------------------------------------ */

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
typedef DWORD					SHADER;

typedef D3DTEXTUREOP			TEXTUREOP;
typedef D3DBLEND				BLEND;
typedef D3DFORMAT				FORMAT;

/* ------------------------------------------------------------------------ */

#define mmult	D3DXMatrixMultiply
#define mrx		D3DXMatrixRotationX
#define mry		D3DXMatrixRotationY
#define mrz		D3DXMatrixRotationZ
#define mrot(m,x,y,z)	D3DXMatrixRotationYawPitchRoll(m,x,y,z)
#define mscale	D3DXMatrixScaling
#define mtrans	D3DXMatrixTranslation
#define mid		D3DXMatrixIdentity
#define misid	D3DXMatrixIsIdentity
#define mtransform	D3DXMatrixTransformation
#define maffine	D3DXMatrixAffineTransformation
#define mtranspose	D3DXMatrixTranspose
#define minv(mout,min) D3DXMatrixInverse(mout,NULL,min)
#define vnormalize D3DXVec3Normalize
#define vnormalize2 D3DXVec2Normalize
#define vlength	D3DXVec3Length
#define vlength2 D3DXVec2Length
#define vcross	D3DXVec3Cross
#define vlerp D3DXVec3Lerp
#define vlerp2 D3DXVec2Lerp
#define vtransform D3DXVec4Transform
#define vtransform3 D3DXVec3TransformNormal
#define vtransform2 D3DXVec2TransformCoord
#define mquaternion D3DXQuaternionRotationMatrix
#define rquaternion D3DXQuaternionRotationAxis
#define qslerp D3DXQuaternionSlerp
#define qnormalize D3DXQuaternionNormalize
#define clerp D3DXColorLerp

#define NODELETE(x) // tip : the non deletion of the ptr is minded !

/* ************************************************************************ */
