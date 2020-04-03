/* ************************************************************************ */
/*																			*/
/*	Honsen ActiveX Plugin													*/
/*	Julien Chevreux, Nicolas Cannasse										*/
/*	(c)2004-2005 Motion-Twin												*/
/*																			*/
/* ************************************************************************ */
#pragma once

void honsen_plugin_init();
void honsen_plugin_close();

struct Impl;

Impl *implementation_init( COleControl *ctrl, const char *url );
void implementation_close( Impl *i );
void implementation_event( Impl *i, unsigned int msg, int wparam, int lparam );
bool implementation_main( Impl *i, HWND h );

/* ************************************************************************ */
