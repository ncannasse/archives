/* ************************************************************************ */
/*																			*/
/*	Honsen 3D Rendering														*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "script.h"
#include "../m3d_priv.h"
#include "../mesh.h"

DEFINE_CLASS(honsen,mesh);

static void set_null_ref( value o ) {
	Mesh *m = val_mesh(o);
	if( m != NULL )
		m->ref = NULL;
}

void Mesh::RefRelease( void *ref ) {
	val_odata(ref) = NULL;
	val_otype(ref) = NULL;
}

static value mesh_new2( value res, value parent ) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	if( res != NULLVAL && !val_is_resource(res) )
		return NULLVAL;
	if( parent != NULLVAL )
		val_check_obj(parent,t_mesh);
	MTW *m = (MTW*)(res?val_resource(res):NULL);
	MeshData *data = Mesh::Make(M3D(),m);
	if( data == NULL ) {
		val_print(alloc_string("Invalid M3D data could not be loaded"));
		return NULL;
	}
	Mesh *pmesh = parent?val_mesh(parent):NULL;
	Mesh *mesh = new Mesh(pmesh,data);
	if( pmesh != NULL )
		pmesh->childs.Add(mesh);
	else
		M3D()->meshes.Add(mesh);
	mesh->ref = o;
	val_odata(o) = (value)mesh;
	val_gc(o,set_null_ref);
	return o;
}

static value mesh_get_pos() {
	value o = val_this();
	val_check_obj(o,t_mesh);
	return alloc_matrix( &val_mesh(o)->pos );
}

static value mesh_set_pos( value m ) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	val_check_obj(m,t_matrix);
	matrix3d_copy( &val_mesh(o)->pos, val_matrix(m) );
	return m;
}

static value mesh_get_color() {
	value o = val_this();
	val_check_obj(o,t_mesh);
	return alloc_int( val_mesh(o)->color & 0x00FFFFFF );
}

static value mesh_set_color( value c ) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	if( !val_is_int(c) )
		return NULLVAL;
	unsigned int cc = val_int(c) & 0x00FFFFFF;
	val_mesh(o)->color = (val_mesh(o)->color & 0xFF000000) | cc;
	return alloc_int(cc);
}

static value mesh_get_alpha() {
	value o = val_this();
	val_check_obj(o,t_mesh);
	return alloc_float( val_mesh(o)->alpha );
}

static value mesh_set_alpha( value c ) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	if( !val_is_number(c) )
		return NULLVAL;
	if( val_number(c) < 0 )
		c = alloc_int(0);
	val_mesh(o)->alpha = val_number(c);
	return c;
}

static value mesh_get_visible() {
	value o = val_this();
	val_check_obj(o,t_mesh);
	return alloc_bool( val_mesh(o)->visible );
}

static value mesh_set_visible( value b) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	if( !val_is_bool(b) )
		return NULLVAL;
	val_mesh(o)->visible = val_bool(b);
	return b;
}

static value mesh_remove() {
	value o = val_this();
	val_check_obj(o,t_mesh);
	val_gc(o,NULL);
	Mesh *m = val_mesh(o);
	if( m->parent == NULL )
		M3D()->meshes.Remove(m);
	else
		m->parent->childs.Remove(m);
	delete m;
	return val_true;
}

static value mesh_resize( value s ) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	if( !val_is_int(s) )
		return NULL;
	int v = val_int(s);
	if( v < 0 || v >= 0x10000 )
		return false;
	val_mesh(o)->Resize(v);
	return val_true;
}

static value mesh_drawVector( value x, value y, value z ) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	if( !val_is_number(x) || !val_is_number(y) || !val_is_number(z) )
		return NULL;
	return alloc_bool( val_mesh(o)->DrawVector(val_number(x),val_number(y),val_number(z)) );
}

static value mesh_drawTCoords( value tu, value tv ) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	if( !val_is_number(tu) || !val_is_number(tv) )
		return NULL;
	return alloc_bool( val_mesh(o)->DrawTCoords(val_number(tu),val_number(tv)) );
}

static value mesh_drawColor( value c, value a ) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	if( !val_is_int(c) || !val_is_number(a) )
		return NULL;
	int va = (int)(val_number(a)*255.0/100.0);
	if( va < 0 )
		va = 0;
	else if( va > 0xFF )
		va = 0xFF;
	return alloc_bool( val_mesh(o)->DrawColor((val_int(c)&0xFFFFFF) | (va << 24)) );
}

static value mesh_calcNormals() {
	value o = val_this();
	val_check_obj(o,t_mesh);
	val_mesh(o)->CalcNormals();
	return val_true;
}

static value mesh_toString() {
	return alloc_string("#mesh");
}

extern value alloc_texture( cached_texture *t );

static value mesh_get_texture() {
	value o = val_this();
	val_check_obj(o,t_mesh);
	return alloc_texture( val_mesh(o)->GetTexture() );
}

static value mesh_set_texture( value t ) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	if( t != NULLVAL )
		val_check_obj(t,t_texture);
	val_mesh(o)->SetTexture( (t == NULL)?NULL:val_texture(t) );
	return t;
}

static value mesh_get_transform() {
	value o = val_this();
	val_check_obj(o,t_mesh);
	return alloc_bool( val_mesh(o)->transform );
}

static value mesh_set_transform( value v ) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	if( !val_is_bool(v) )
		return NULLVAL;
	val_mesh(o)->transform = val_bool(v);
	return v;
}

static value mesh_get_sprite() {
	value o = val_this();
	val_check_obj(o,t_mesh);
	return alloc_bool( val_mesh(o)->sprite );
}

static value mesh_set_sprite( value v ) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	if( !val_is_bool(v) )
		return NULLVAL;
	val_mesh(o)->sprite = val_bool(v);
	return v;
}

static value mesh_get_light() {
	value o = val_this();
	val_check_obj(o,t_mesh);
	return alloc_bool( val_mesh(o)->light );
}

static value mesh_set_light( value v ) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	if( !val_is_bool(v) )
		return NULLVAL;
	val_mesh(o)->light = val_bool(v);
	return v;
}

static value mesh_get_srcblend() {
	value o = val_this();
	val_check_obj(o,t_mesh);
	return alloc_int( val_mesh(o)->srcblend );
}

static value mesh_set_srcblend( value v ) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	if( !val_is_int(v) )
		return NULLVAL;
	val_mesh(o)->srcblend = val_int(v);
	return v;
}

static value mesh_get_dstblend() {
	value o = val_this();
	val_check_obj(o,t_mesh);
	return alloc_int( val_mesh(o)->dstblend );
}

static value mesh_set_dstblend( value v ) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	if( !val_is_int(v) )
		return NULLVAL;
	val_mesh(o)->dstblend = val_int(v);
	return v;
}

static value honsen_mesh() {
	value o = alloc_class(&t_mesh);
	Constr(o,mesh,Mesh,2);
	Property(o,mesh,pos);
	Property(o,mesh,color);
	Property(o,mesh,alpha);
	Property(o,mesh,visible);
	Property(o,mesh,texture);
	Property(o,mesh,transform);
	Property(o,mesh,sprite);
	Property(o,mesh,light);
	Property(o,mesh,srcblend);
	Property(o,mesh,dstblend);
	Method(o,mesh,remove,0);
	Method(o,mesh,toString,0);
	Method(o,mesh,resize,1);
	Method(o,mesh,drawVector,3);
	Method(o,mesh,drawTCoords,2);
	Method(o,mesh,drawColor,2);
	Method(o,mesh,calcNormals,0);
	return o;
}

/* ************************************************************************ */
