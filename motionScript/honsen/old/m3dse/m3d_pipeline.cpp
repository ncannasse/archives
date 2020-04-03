/* ************************************************************************ */
/*																			*/
/*	M3D-SE Progammable Pipeline												*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "m3d.h"
#include "m3d_api.h"
#include "m3d_context.h"
#include "m3d_pipeline.h"
#include "../common/mtw.h"

#undef M3DDEBUG
#define M3DDEBUG(msg) M3D::DebugMessage(parent->context,"Mesh '"+this->object_name+"' : "+(WString)(msg),__FILE__,__LINE__)

namespace M3D {

/* ------------------------------------------------------------------------ */

static int ShaderFormatOfFormat( TAG_FORMAT format ) {
	switch( format ) {
	case TF_FLOAT1: return D3DVSDT_FLOAT1;
	case TF_FLOAT2: return D3DVSDT_FLOAT2;
	case TF_FLOAT3: return D3DVSDT_FLOAT3;
	case TF_FLOAT4: return D3DVSDT_FLOAT4;
	case TF_SHORT2: return D3DVSDT_SHORT2;
	case TF_SHORT4: return D3DVSDT_SHORT4;
	case TF_COLOR: return D3DVSDT_D3DCOLOR ;
	default: return 0;
	}
}

static TAG TagFromString( const char *str ) {
	int n = 0;
	TAG t = 0;
	while( *str && n < 4 ) {
		t |= ((unsigned char)*str)<<(n<<3);
		str++;
		n++;
	}
	return t;
}

int
PipeImpl::find_tag( TAG t, LocalTag *tag ) {
	return (int)tag->tag-(int)t;
}

/* ------------------------------------------------------------------------ */

bool 
PipeImpl::Prepare( MTW *o ) {
	if( o->HasChild(MAKETAG(pREP)) )
		return true;
	MTW *indx = o->Child(MAKETAG(INDX));
	MTW *vect = o->Child(MAKETAG(VECT));
	if( !indx || !vect )
		return false;
	MTW *map0 = o->Child(MAKETAG(MAP0));
	MTW *mid0 = o->Child(MAKETAG(MID0));
	if( !map0 && !mid0 ) {
		int i;
		int nverts = indx->size/sizeof(WORD);
		WORD *indexes = new WORD[nverts];
		for(i=0;i<nverts;i++)
			indexes[i] = i;
		o->AddChild( new MTW(MAKETAG(RIDX),nverts*2,indexes) );
		o->AddChild( new MTW(MAKETAG(pREP)) );
		return true;
	}
	if( !map0 || !mid0 || mid0->size != indx->size )
		return false;

	int npoints = indx->size/sizeof(WORD);
	WORD *indexes = (WORD*)indx->data;
	WList<DWORD> data;
	int i;
	for(i=0;i<npoints;i++) {
		DWORD v = indexes[i] | (((WORD*)mid0->data)[i] << 16);
		int p = data.IndexOf(v);
		if( p == -1 ) {
			indexes[i] = data.Length();
			data.Add(v);
		} else
			indexes[i] = p;
	}
	
	int nverts = data.Length();
	VECTOR *newvect = new VECTOR[nverts];
	VECTOR2 *newmap0 = new VECTOR2[nverts];
	indexes = new WORD[nverts];
	i = 0;
	FOREACH(DWORD,data);
		indexes[i] = (WORD)(item & 0xFFFF);
		newvect[i] = ((VECTOR*)vect->data)[item & 0xFFFF];
		newmap0[i] = ((VECTOR2*)map0->data)[item >> 16];
		i++;
	ENDFOR;
	o->RemoveChild(vect);
	o->RemoveChild(map0);
	o->RemoveChild(mid0);
	delete vect;
	delete map0;
	delete mid0;

	o->AddChild( new MTW(MAKETAG(RIDX),nverts*sizeof(WORD),indexes) );
	o->AddChild( new MTW(MAKETAG(VECT),nverts*sizeof(VECTOR),newvect) );
	o->AddChild( new MTW(MAKETAG(MAP0),nverts*sizeof(VECTOR2),newmap0) );
	o->AddChild( new MTW(MAKETAG(pREP)) );
	return true;
}

/* ------------------------------------------------------------------------ */

PipeImpl::PipeImpl( Mesh *parent, MTW *root ) : parent(parent), root(root), local(find_tag) {
	validated = false;
	buffer = NULL;
	shader = NULL;
	lock = NULL;
	indexed_buffer = NULL;
	MTW *t = root->Child(MAKETAG(VECT));
	nverts = t->size/12;
	indexes = root->Child(MAKETAG(INDX));
	nfaces = indexes?(indexes->size/6):(nverts/3);
	t = root->Child(MAKETAG(NAME));
	object_name = (char*)t->data;
	t = root->Child(MAKETAG(TEX0));
	if( t ) {
		MTW *tdata = Resource::Find(parent->context,(const char *)t->data);
		if( tdata == NULL )
			M3DDEBUG("Texture not found "+(WString)(char*)t->data);
		else
			texture = Resource::LoadTexture(parent->context,tdata);
	}
}

PipeImpl::~PipeImpl() {
	CleanRenderers();
	StopAnims();
}

/* ------------------------------------------------------------------------ */

void
PipeImpl::AddAnim( Anim *a ) {
	curanims.AddFirst(a);
}

void
PipeImpl::RemoveAnim( Anim *a  ) {
	curanims.Remove(a);
}

void
PipeImpl::StopAnims() {
	curanims.Delete();
}

/* ------------------------------------------------------------------------ */

MTW*
PipeImpl::GetTexture() {
	return texture.Available()?texture.Data()->res:NULL;
}

void
PipeImpl::SetTexture( MTW *t ) {
	if( t == NULL )
		texture.Release();
	else {
		texture = Resource::LoadTexture(parent->context,t);
		Validate();
	}
}

void
PipeImpl::SetShader( WString code ) {
	Invalidate();
	shader_code = code;
}

WString
PipeImpl::GetShader() {
	return shader_code;
}

void
PipeImpl::AddRenderer( Renderer *r, bool is_static ) {
	Invalidate();
	if( is_static ) {
		FOREACH(Renderer*,dynamics);
			statics.Add(item);
		ENDFOR;
		dynamics.Clean();
		statics.Add(r);
	}
	else
		dynamics.Add(r);
}

bool
PipeImpl::RemoveRenderer( WString name ) {
	Invalidate();
	FOREACH(Renderer*,statics);
		if( strcmp(item->Name(),name.c_str()) == 0 ) {
			statics.Delete(item);
			return true;
		}
	ENDFOR;
	FOREACH(Renderer*,dynamics);
		if( strcmp(item->Name(),name.c_str()) == 0 ) {
			dynamics.Delete(item);
			return true;
		}
	ENDFOR;
	return false;
}

void
PipeImpl::CleanRenderers() {
	Invalidate();
	statics.Delete();
	dynamics.Delete();
}

/* ------------------------------------------------------------------------ */

bool
PipeImpl::Validate() {

	Invalidate();

	// Validate Static Pipeline
	//  - call Validate on each static renderer
	//  - this will construct the Read/Write/Delete Pipeline
	FOREACH(Renderer*,statics);
		cur_renderer = item;
		cur_pipeline = new TagPipeline();
		pipeline.Add(cur_pipeline);
		SetLastError("Unknown error");
		if( !item->Validate(this) ) {
			Invalidate();
			M3DDEBUG(last_error);
			return false;
		}
	ENDFOR;

	// Validate Dynamic Pipeline
	//  - call Validate on each dynamic renderer
	//  - this will construct the Read/Write/Delete Pipeline
	dynamic_pipeline = NULL;
	FOREACH(Renderer*,dynamics);
		cur_renderer = item;
		cur_pipeline = new TagPipeline();
		pipeline.Add(cur_pipeline);
		if( dynamic_pipeline == NULL )
			dynamic_pipeline = pipeline.End();
		SetLastError("Unknown error");
		if( !item->Validate(this) ) {
			Invalidate();
			M3DDEBUG(last_error);
			return false;
		}
	ENDFOR;

	// Validate All TAG's Strides
	//  - if a tag has an unknown stride, we cannot allocate the stream
	FOREACH(TagPipeline*,pipeline);
		TagPipeline *current = item;
		FOREACH(TagOp,*current);
			TAG t = item.tag;
			if( item.op == OP_WRITE && TagFormat::GetStride(parent->context,t) <= 0 ) {
				Invalidate();
				M3DDEBUG("Unknown stride for tag '"+MTW::ToString(t)+"'.");
				return false;
			}
		ENDFOR;
	ENDFOR;

	// Validate Shader
	if( !ValidateShader() ) {
		Invalidate();
		return false;
	}

	// Validate Buffer
	if( !ValidateBuffer() ) {
		Invalidate();
		return false;
	}

	// Log Warnings
	FOREACH(WString,log);
		M3DDEBUG(item);
	ENDFOR;

	// Done !
	validated = true;
	log.Clean();
	available.Clean();
	deleted.Clean();

	return true;
}

/* ------------------------------------------------------------------------ */

bool
PipeImpl::ValidateShader() {
	int vnb = 0;
	char tagname[6];
	DWORD decl[32];
	int dpos = 0;
	tagname[0] = '$';
	tagname[5] = 0;
	decl[dpos++] = D3DVSD_STREAM(0);

	WString local_code = "vs.1.0\n"+shader_code;
	const char *tag_finder = local_code.c_str();
	bool written;

	// Loop for '$tname' in the shader source code
	while( (tag_finder = strchr(tag_finder,'$')) != NULL ) {
		TAG t = TagFromString(tag_finder+1);
		bool is_available = available.Exists(t);
		bool found = is_available || (root->Child(t) != NULL);
		if( found ) {
			char *strp = (char*)tag_finder;
			*(TAG*)(tagname+1) = t;
			// global replace with the vertex number
			while( (strp = strstr(strp,tagname)) != NULL ) {
				strp[0] = 'v';
				strp[1] = '0'+(vnb/10);
				strp[2] = '0'+(vnb%10);
				strp[3] = ' ';
				strp[4] = ' ';
				strp++;
			}
			TAG_FORMAT format = TagFormat::GetFormat(parent->context,t);
			int stride = TagFormat::StrideOfFormat(format);
			if( format == TF_UNKNOWN ) {
				M3DDEBUG("Unknown Format for tag '"+MTW::ToString(t)+"'.");
				return false;
			}
			if( stride == 0 ) {
				M3DDEBUG("Invalid Format for tag '"+MTW::ToString(t)+"'.");
				return false;
			}
			decl[dpos++] = D3DVSD_REG(vnb, ShaderFormatOfFormat(format));
			// prevent possible buffer overflow
			if( dpos >= sizeof(decl) ) {
				M3DDEBUG("Too many tags in stream.");
				return false;
			}
			Renderer *writer = GetWriter(t,&written);
			// add a new element at the final stream
			stream_format.Add( new StreamTag(t,format,stride,writer,written) );
			vnb++;
			if( is_available )
				available.Remove(t);
		}
		else {
			M3DDEBUG("The tag '"+MTW::ToString(t)+"' needed by shader is not available.");
			return false;
		}
		tag_finder++;
	}
	decl[dpos++] = D3DVSD_END();

	FOREACH(TAG,available);
		ValidateWarning("The stream '"+MTW::ToString(item)+"' is available but not used by shader.");
	ENDFOR;

	int cnb = 0;
	WPairBTree<WString,int> bounds(wstrcmp);
	tag_finder = local_code.c_str();
	while( (tag_finder = strchr(tag_finder,'#')) != NULL ) {
		WString cname;
		char *tag_end = (char*)(tag_finder+1);
		while( *tag_end != 0 && *tag_end != '.' && *tag_end != '\r' && *tag_end != '\n' && *tag_end != ' ' && *tag_end != ',' && *tag_end != '\t' ) {
			cname += *tag_end;
			*tag_end = ' ';
			tag_end++;
		}
		int found_nb;
		if( bounds.Exists(cname) )
			found_nb = bounds.Find(cname);
		else {
			ShaderConst *sconst = ShaderConst::New(cname.c_str(),parent);
			if( sconst == NULL )
				return false;			
			sh_consts.Add(sconst);
			bounds.Add(cname,cnb);
			found_nb = cnb;
			cnb += sconst->Size();
		}
		((char*)tag_finder)[0] = 'c';
		((char*)tag_finder)[1] = '0'+(found_nb/10);
		((char*)tag_finder)[2] = '0'+(found_nb%10);
		tag_finder = tag_end;
	}

	// *TODO* use a shader cache

	// create the shader
	LPD3DXBUFFER error = NULL;
	LPD3DXBUFFER shader_buffer = NULL;
	Context *c = parent->context;
	if( D3DXAssembleShader(local_code.c_str(),local_code.length(),0,NULL,&shader_buffer,&error) != D3D_OK ) {
		if( error )
			M3DDEBUG("Shader Compilation Error");
		else {
			M3DDEBUG("Shader Compilation Error: \n"+(WString)(char*)error->GetBufferPointer());
			error->Release();
		}
		return false;
	}
		
	if( M3DERROR(c->device->CreateVertexShader(decl,(DWORD*)shader_buffer->GetBufferPointer(),&shader,c->use_hwshaders?0:D3DUSAGE_SOFTWAREPROCESSING),"Create VertexShader") ) {
		shader_buffer->Release();
		return false;
	}
	
	shader_buffer->Release();	
	return true;
}

/* ------------------------------------------------------------------------ */

bool
PipeImpl::ValidateBuffer() {
	Renderer *writer;
	bool full_static = true;
	bool full_dynamic = true;
	bool need_copy = false;
	int copy_stride = 0;

	buffer_stride = 0;

	// calculate the stream stride and the render mode
	FOREACH(StreamTag*,stream_format);
		buffer_stride += item->stride;
		if( item->writer == NULL || !item->written ) {
			need_copy = true;
			copy_stride += item->stride;
		}
		else if( IsStatic(item->writer) ) {
			writer = item->writer;
			full_dynamic = false;
		} else
			full_static = false;
	ENDFOR;

	if( buffer_stride == 0 ) {
		M3DDEBUG("Zero buffer stride.");
		return false;
	}

	if( nverts <= 0 ) {
		M3DDEBUG("Invalid vertex count.");
		return false;
	}

	if( !full_dynamic && !full_static && !need_copy )
		ValidateWarning((WString)writer->Name()+" is causing mixed render.");
	
	// *TODO* if full_dynamic = true & copy_stride/buffer_stride is low
	// then we can use a dynamic buffer strategy (allow batching)
 
	// create the local vertex buffer	
	Context *c = parent->context;
	if( M3DERROR(c->device->CreateVertexBuffer(buffer_stride*nverts, (full_static?0:D3DUSAGE_DYNAMIC) | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &buffer),"CreateVertexBuffer") )
		return false;

	// lock and unlock to discard its contents (bug when first lock is D3DLOCK_NOOVERWRITE)
	buffer->Lock(0,0,&lock,D3DLOCK_DISCARD);
	buffer->Unlock();
	lock = NULL;

	// create the indexed buffer (if needed)
	if( indexes != NULL ) {
		if( M3DERROR(c->device->CreateIndexBuffer(indexes->size, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &indexed_buffer),"CreateIndexBuffer") )
			return false;
		BYTE *idx;
		if( M3DERROR(indexed_buffer->Lock(0,0,&idx,D3DLOCK_NOOVERWRITE),"Lock IndexBuffer") )
			return false;
		memcpy(idx,indexes->data,indexes->size);
		indexed_buffer->Unlock();
	}

	// do the static render (will be done only once)
	WList<TagPipeline*>::LCel *pipe_iter = pipeline.Begin();
	FOREACH(Renderer*,statics);
		cur_renderer = item;
		cur_pipeline = pipe_iter->item;
		pipe_iter = pipe_iter->next;
		item->Render(this);
	ENDFOR;

	need_dynamic_copy = false;
	cur_renderer = NULL;
	cur_pipeline = NULL;

	// transform the original texture coordinates
	MTW *t = root->Child(MAKETAG(MAP0));
	if( t && texture.Available() ) {
		int i;
		int ncoords = t->size/8;
		FLOAT *tcoords = new FLOAT[ncoords*2];
		FLOAT *data = (FLOAT*)t->data;
		Resource::TPos *pos = &texture.Data()->pos;
		for(i=0;i<ncoords;i++,data+=2) {
			tcoords[i<<1] = data[0] * pos->sx + pos->x;
			tcoords[(i<<1)+1] = data[1] * pos->sy + pos->y;
		}
		local.Add( MAKETAG(MAP0), new LocalTag(MAKETAG(MAP0),t->size,tcoords) );
	}

	// copy the static streams into the final stream
	FOREACH(StreamTag*,stream_format);
		if( item->writer == NULL || (!item->written && IsStatic(item->writer)) ) {
			CopyStream(item->tag,item->fmt);
			item->written = true;
		}
		else
			need_dynamic_copy |= !item->written;
	ENDFOR;

	// *TODO* clean up local root with deleted/no-more-readed static Streams

	return true;
}

/* ------------------------------------------------------------------------ */

void
PipeImpl::Invalidate() {
	validated = false;
	log.Clean();
	available.Clean();
	deleted.Clean();
	sh_consts.Delete();
	pipeline.Delete();
	stream_format.Delete();
	cur_renderer = NULL;
	cur_pipeline = NULL;
	local.Delete();
	if( indexed_buffer != NULL ) {
		indexed_buffer->Release();
		indexed_buffer = NULL;
	}
	if( buffer != NULL ) {
		buffer->Release();
		buffer = NULL;
	}
	if( shader != NULL ) {
		DEVICE d = parent->context->device;
		if( d != NULL )
			d->DeleteVertexShader(shader);
		shader = NULL;
	}
}

/* ------------------------------------------------------------------------ */

int
PipeImpl::PipelinePos( TAG_OP op, TAG t, int spos ) {
	TagOp x(op,t);
	int pos = 0;
	FOREACH(TagPipeline*,pipeline);
		if( pos >= spos && item->Exists(x) )
			return pos;
		pos++;
	ENDFOR;
	return -1;
}

int
PipeImpl::PipelineRPos( TAG_OP op, TAG t, int spos ) {
	TagOp x(op,t);
	int lpos = -1;
	int pos = 0;
	FOREACH(TagPipeline*,pipeline);
		if( pos >= spos && item->Exists(x) )
			lpos = pos;
		pos++;
	ENDFOR;
	return lpos;
}

bool
PipeImpl::IsStatic( Renderer *r ) {
	return statics.Exists(r);
}

bool
PipeImpl::IsValid() {
	return validated;
}

bool
PipeImpl::HasTag( TAG t ) {
	return root->HasChild(t);
}

Renderer*
PipeImpl::GetWriter( TAG t, bool *written ) {
	Renderer *writer = NULL;
	*written = true;
	WList<Renderer*>::LCel *renders = statics.Begin();
	if( renders == NULL )
		renders = dynamics.Begin();
	FOREACH(TagPipeline*,pipeline);
		if( item->Exists(TagOp(OP_WRITE,t)) ) {
			writer = renders->item;
			*written = true;
		} else if( writer != NULL && (item->Exists(TagOp(OP_READ,t)) || item->Exists(TagOp(OP_DELETE,t))) )
			*written = false;
		renders = renders->next;
		if( renders == NULL )
			renders = dynamics.Begin();
	ENDFOR;
	return writer;
}

/* ------------------------------------------------------------------------ */

void
PipeImpl::SetLastError( WString msg ) {
	last_error = (WString)cur_renderer->Name()+": "+msg;
}

void
PipeImpl::ValidateWarning( WString msg ) {
	log.Add(msg);
}

bool
PipeImpl::Read( TAG t ) {
	bool found = available.Exists(t) || (root->Child(t) != NULL);
	if( deleted.Exists(t) ) {
		if( found )
			ValidateWarning((WString)cur_renderer->Name()+" is reading deleted stream '"+MTW::ToString(t)+"'.");
		else
			last_error = "Stream '"+MTW::ToString(t)+"' required by "+cur_renderer->Name()+" not found : has been deleted.";
	}
	else if( !found )
		last_error = "Stream '"+MTW::ToString(t)+"' required by "+cur_renderer->Name()+" not found.";
	else
		cur_pipeline->Add( TagOp(OP_READ,t) );
	return found;
}

bool
PipeImpl::Write( TAG t ) {
	if( deleted.Exists(t) )
		ValidateWarning((WString)cur_renderer->Name()+" is writing deleted stream '"+MTW::ToString(t)+"'.");
	bool found = available.Exists(t) || (root->Child(t) != NULL);
	if( found && !cur_pipeline->Exists(TagOp(OP_READ,t)) ) {
		if( available.Exists(t) )
			ValidateWarning((WString)cur_renderer->Name()+" is overwriting existing stream '"+MTW::ToString(t)+"'.");
		else
			available.Add(t);
	}
	else
		available.Add(t);
	cur_pipeline->Add( TagOp(OP_WRITE,t) );
	return true;
}

bool
PipeImpl::Delete( TAG t ) {
	bool found = available.Exists(t) || (root->Child(t) != NULL);
	if( !found )
		ValidateWarning((WString)cur_renderer->Name()+" is deleting not existing stream '"+MTW::ToString(t)+"'.");
	else
		available.Remove(t);
	deleted.Add(t);
	cur_pipeline->Add( TagOp(OP_DELETE,t) );
	return true;
}

int
PipeImpl::NVerts() {
	return nverts;
}

int
PipeImpl::NFaces() {
	return nfaces;
}

bool
PipeImpl::TimeCoef( TAG t, void **f1, void **f2, FLOAT *coef ) {
	return TimeCoef2(t,f1,f2,NULL,coef);
}

bool
PipeImpl::TimeCoef2( TAG t, void **f1, void **f2, void **f3, FLOAT *coef ) {
	FOREACH(Anim*,curanims);
		MTW *data = item->GetData();
		if( data != NULL && data->HasChild(t) ) {
			item->TimeCoef2(f1,f2,f3,coef);
			return true;
		}
	ENDFOR;
	return false;
}

/* ------------------------------------------------------------------------ */

const void *
PipeImpl::QueryRead( TAG t, int *size ) {
	// check permissions
	if( cur_pipeline != NULL && !cur_pipeline->Exists(TagOp(OP_READ,t)) ) {
		M3DDEBUG((WString)cur_renderer->Name()+" has not right to read stream '"+MTW::ToString(t)+"'.");
		return NULL;
	}

	LocalTag *tag = local.Find(t);
	if( tag != NULL ) {
		if( size != NULL )
			*size = tag->stride * nverts;
		return tag->data;
	}
	
	MTW *rtag = root->Child(t);
	if( rtag == NULL ) {
		M3DDEBUG("ASSERT! - Validate read checking failure.");
		return NULL;
	}

	if( size != NULL )
		*size = rtag->size;
	return rtag->data;
}


WriteStream<BYTE>
PipeImpl::QueryWrite( TAG t ) {
	// check permissions
	if( cur_pipeline != NULL && !cur_pipeline->Exists(TagOp(OP_WRITE,t)) ) {
		M3DDEBUG((WString)cur_renderer->Name()+" has not right to write stream '"+MTW::ToString(t)+"'.");
		return WriteStream<BYTE>(NULL,0);
	}

	bool write_stream = (cur_renderer == NULL);
	int delta = 0, stride = 0;
	FOREACH(StreamTag*,stream_format);
		if( item->tag == t ) {
			stride = item->stride;
			write_stream |= item->written && (cur_renderer == item->writer);
			break;
		}
		delta += item->stride;
	ENDFOR;

	if( write_stream ) {
		Context *c = parent->context;
		if( lock == NULL && M3DERROR(buffer->Lock(0,0,&lock,D3DLOCK_NOOVERWRITE),"Lock") )
			return WriteStream<BYTE>(NULL,0);
		return WriteStream<BYTE>((char*)(lock+delta),buffer_stride);
	}
	
	LocalTag *tag = local.Find(t);
	if( tag == NULL ) {
		// *TODO* use a Tmp Buffer Strategy to share memory between multiple objects
		if( stride == 0 )
			stride = TagFormat::GetStride(parent->context,t);
		tag = new LocalTag(t,stride,new char[stride*nverts]);
		local.Add(t,tag);
	}

	return WriteStream<BYTE>((char*)tag->data,tag->stride);
}

/* ------------------------------------------------------------------------ */

typedef struct { DWORD a,b; } SIZE8;
typedef struct { DWORD a,b,c; } SIZE12;
typedef struct { DWORD a,b,c,d; } SIZE16;

void
PipeImpl::CopyStream( TAG tag, TAG_FORMAT fmt ) {
	const void *read = QueryRead(tag,NULL);
	WriteStream<BYTE> *write = &QueryWrite(tag);
	// we cannot use something like memcpy since the stream
	// data is not aligned
	switch( fmt ) {
	case TF_FLOAT3:
		((WriteStream<SIZE12>*)write)->Copy((SIZE12*)read,nverts);
		return;
	case TF_FLOAT4:
		((WriteStream<SIZE16>*)write)->Copy((SIZE16*)read,nverts);
		return;
	case TF_FLOAT1:
	case TF_SHORT2:
		((WriteStream<DWORD>*)write)->Copy((DWORD*)read,nverts);
		return;
	case TF_FLOAT2:
	case TF_SHORT4:
		((WriteStream<SIZE8>*)write)->Copy((SIZE8*)read,nverts);
		return;	
	}
}

/* ------------------------------------------------------------------------ */

int
PipeImpl::Render() {
	if( !validated )
		return 0;

	// update animation times
	FOREACH(Anim*,curanims);
		item->UpdateTime();
	ENDFOR;

	// *TODO* If using a dynamic buffer strategy, query a buffer

	// Do the dynamic render
	WList<TagPipeline*>::LCel *pipe_iter = dynamic_pipeline;
	FOREACH(Renderer*,dynamics);
		cur_renderer = item;
		cur_pipeline = pipe_iter->item;
		pipe_iter = pipe_iter->next;
		item->Render(this);
	ENDFOR;

	// Copy unwritten dynamic streams
	if( need_dynamic_copy ) {
		cur_renderer = NULL;
		cur_pipeline = NULL;
		FOREACH(StreamTag*,stream_format);
			if( !item->written )
				CopyStream(item->tag,item->fmt);
		ENDFOR;
	}

	if( lock != NULL ) {
		buffer->Unlock();
		lock = NULL;
	}

	// *TODO* Emit buffer

	// setup shader and constants
	int cnb = 0;
	Context *c = parent->context;
	c->device->SetVertexShader(shader);
	if( texture.Available() )
		c->device->SetTexture(0,texture.Data()->t);
	else
		c->device->SetTexture(0,NULL);

	FOREACH(ShaderConst*,sh_consts);
		int n = item->Size();
		void *data = item->Data(c);
		c->device->SetVertexShaderConstant(cnb,data,n);
		cnb += item->Size();
	ENDFOR;

	c->device->SetStreamSource(0,buffer,buffer_stride);
	DWORD npasses;
	if( c->device->ValidateDevice(&npasses) == D3D_OK ) {
		if( indexed_buffer ) {
			c->device->SetIndices(indexed_buffer,0);
			c->device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,nverts,0,nfaces);
		} else
			c->device->DrawPrimitive(D3DPT_TRIANGLELIST,0,nfaces);
	}

	local.Delete();

	return nfaces;
}

/* ------------------------------------------------------------------------ */
}; // namespace M3D
/* ************************************************************************ */