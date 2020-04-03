/* ************************************************************************ */
/*																			*/
/*	Honsen 3D Rendering														*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once

enum pic_kind {
	PIC_JPEG,
	PIC_RAW
};

char *picture_read( void *data, int size, pic_kind k, int *twidth, int *theight );

/* ************************************************************************ */
