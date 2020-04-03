/* ************************************************************************ */
/*																			*/
/*	M3D-SE Context declaration												*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once
#include "mdx.h"
#include "../common/wstring.h"
#include "../common/context.h"
#include "../common/timer.h"

namespace M3D {

	struct _2DContext;
	struct CoreContext;
	struct LibContext;	
	struct ScriptContext;
	
	class Group;
	class Text;

	namespace Resource {
		struct TextureContext;
		struct ResContext;
	};

	struct Context {
		OBJECT	object;
		DEVICE	device;
		HWND	render_target;
		FORMAT	buffer_format;
		bool	software;
		bool	soft_tnl;
		bool	use_hwshaders;
		bool	aa_enabled;
		HRESULT lasterror;
		
		int		width;
		int		height;
		DWORD	bgcolor;

		WString disp_text;
		bool	show_fps;

		timer *time;
		CoreContext *core;
		_2DContext *_2d;
		LibContext *lib;
		Resource::ResContext *res;
		Resource::TextureContext *textures;
		ScriptContext *script;

		Group *display_group;
		Text *display;

		void *weak_cache;
		void *cache;

		void	(*debug_out)( const char *);
		int		(*get_config)( const char *);
	};

	__inline Context *__GetContext() { return (Context*)get_context(); }

};