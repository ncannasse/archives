/* ************************************************************************ */
/*																			*/
/*	M3D-SE Progammable Pipeline												*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once

#include "m3d_sdk.h"
#include "m3d_cache.h"
#include "m3d_res.h"
#include "../common/wpairbtree.h"

namespace M3D {

class PipeImpl : public Pipeline, public StreamPipeline {

	// Pipeline Types
	typedef enum {
		OP_READ,
		OP_WRITE,
		OP_DELETE
	} TAG_OP;

	struct TagOp {
		TAG_OP op;
		TAG tag;
		TagOp() { }
		TagOp( TAG_OP op, TAG tag ) : op(op), tag(tag) { }
		bool operator == (const TagOp &t) { return op == t.op && tag == t.tag; }
	};
	
	struct StreamTag {
		TAG tag;
		TAG_FORMAT fmt;
		int stride;
		Renderer *writer;
		bool written;
		StreamTag( TAG tag, TAG_FORMAT fmt, int stride, Renderer *writer, bool nmr ) : tag(tag), fmt(fmt), stride(stride), writer(writer), written(nmr) { }
		bool operator == (const TAG &t) { return tag == t; }
	};

	typedef WList<TagOp> TagPipeline;

	struct LocalTag {
		TAG tag;
		int stride;
		void *data;
		LocalTag( TAG tag, int stride, void *data ) : tag(tag), stride(stride), data(data) { }
		~LocalTag() { delete data; }
	};

	// Pipeline Datas
	WList<TagPipeline*> pipeline;
	WList<TagPipeline*>::LCel *dynamic_pipeline;
	WList<Renderer*> statics;
	WList<Renderer*> dynamics;
	WList<StreamTag*> stream_format;
	WList<ShaderConst*> sh_consts;

	// Pipeline Functions
	int PipelinePos( TAG_OP op, TAG t, int spos = 0 );
	int PipelineRPos( TAG_OP op, TAG t, int spos = 0 );
	bool IsStatic( Renderer *r );
	Renderer *GetWriter( TAG t, bool *written );

	// Validate Datas
	WList<TAG> available;
	WList<TAG> deleted;
	WList<WString> log;
	WString last_error;
	TagPipeline *cur_pipeline;
	Renderer *cur_renderer;
	
	// Validate Functions
	bool ValidateShader();
	bool ValidateBuffer();
	void ValidateWarning( WString msg );
	void SetLastError( WString msg );
	bool Read( TAG t );
	bool Write( TAG t );
	bool Delete( TAG t );

	// Render Functions
	int NVerts();
	int NFaces();
	bool TimeCoef( TAG t, void **f1, void **f2, FLOAT *time );
	bool TimeCoef2( TAG t, void **f1, void **f2, void **f3, FLOAT *time );
	const void *QueryRead( TAG t, int *size );
	WriteStream<BYTE> QueryWrite( TAG t );
	void QueryDelete( TAG t );
	void CopyStream( TAG t, TAG_FORMAT fmt );

	// Shader Datas
	WString shader_code;
	SHADER shader;
	
	// Render Datas
	WString object_name;
	Cached<Resource::Texture> texture;
	MTW *indexes;
	IBUF indexed_buffer;
	VBUF buffer;
	BYTE *lock;
	int buffer_stride;
	int nverts, nfaces;
	WList<Anim*> curanims;

	bool need_dynamic_copy;
	bool validated;

	// Raw PipeImpl
	MTW* root;
	WBTree<TAG,LocalTag*> local;
	Mesh *parent;
	static int find_tag( TAG, LocalTag * );

public:
	PipeImpl( Mesh *parent, MTW *root );
	~PipeImpl();

	void AddRenderer( Renderer *r, bool is_static );
	bool RemoveRenderer( WString name );
	void CleanRenderers();

	WString GetShader();
	void SetShader( WString data );

	MTW *GetTexture();
	void SetTexture( MTW *t );

	bool IsValid();
	void Invalidate();
	bool Validate();
	bool HasTag( TAG t );

	void AddAnim( Anim *a );
	void RemoveAnim( Anim *a );
	void StopAnims();

	int Render();

	static bool Prepare( MTW *o );

};


/* ------------------------------------------------------------------------ */
}; // namespace M3D
/* ************************************************************************ */


