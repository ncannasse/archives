/* ************************************************************************ */
/*																			*/
/*	M3D-SE Mesh for MTS														*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "script.h"
#include "../../common/mtw.h"

DEFINE_CLASS(m3dse,mesh);

void mesh_delete( value o ) {
	if( val_mesh(o) )
		val_mesh(o)->script_ref = NULL;
}

void M3D::Mesh::CleanRef( void *ref ) {
	val_odata(ref) = NULL;
	val_otype(ref) = NULL;
	val_gc((value)ref,NULL);
}

int
M3D::Mesh::MakeId( const char *name ) {
	return (int)val_id(name);
}

void*
M3D::Mesh::GetData( VECTOR4 *vtmp, void *o, int id, int *size ) {
	value v = val_field((value)o,(field)id);
	if( val_is_number(v) ) {
		vtmp->x = val_number(v);
		vtmp->y = val_number(v);
		vtmp->z = val_number(v);
		vtmp->w = 1;
		*size = 1;
		return vtmp;
	}
	if( !val_is_object(v) ) {
		*size = 0;
		return NULL;
	}
	if( val_is_obj(v,t_matrix) ) {
		*size = 4;
		return val_matrix(v);
	}
	if( val_is_obj(v,t_vector) ) {
		*size = 1;
		VECTOR *vv = val_vector(v);
		vtmp->x = vv->x;
		vtmp->y = vv->y;
		vtmp->z = vv->z;
		vtmp->w = 1;
		return vtmp;
	}	
	*size = 1;
	value f;
	f = val_field(v,val_id("x"));
	vtmp->x = val_is_number(f)?val_number(f):0;
	f = val_field(v,val_id("y"));
	vtmp->y = val_is_number(f)?val_number(f):0;
	f = val_field(v,val_id("z"));
	vtmp->z = val_is_number(f)?val_number(f):0;
	f = val_field(v,val_id("w"));
	vtmp->w = val_is_number(f)?val_number(f):0;
	return vtmp;
}

value mesh_new2( value r, value p ) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	val_check_obj(r,t_resource);

	M3D::Mesh *parent = NULL;
	if( p != NULLVAL ) {
		val_check_obj(p,t_mesh);
		parent = val_mesh(p);
	}

	MTW *obj = (MTW*)val_resource(r);
	if( !M3D::Mesh::Prepare(obj) )
		return NULL;

	M3D::Mesh *omesh = new M3D::Mesh(parent?parent->context:M3D::__GetContext(),parent,obj);
	omesh->script_ref = o;
	if( obj->HasChild(MAKETAG(SKEL)) ) {
		omesh->AddRenderer("anim_skin",false);
		omesh->AddRenderer("calc_normals",false);
	} else
		omesh->AddRenderer("calc_normals",true);	
	if( obj->HasChild(MAKETAG(TEX0)) )
		omesh->SetShader(" \
			m4x4 r0 , $VECT, #pos \n\
			m4x4 oPos, r0, #camera \n\
			m3x3 r0 , $NORM, #pos \n\
			dp3 r0, r0, #light\n\
			max r0, r0, #zero \n\
			mul r0, r0, #light_color \n\
			add r0, r0, #ambient_color \n\
			min r0, r0, #one \n\
			mul oD0, r0, #color \n\
			mov oT0, $MAP0 \n\
		");
	else
		omesh->SetShader(" \
			m4x4 r0 , $VECT, #pos \n\
			m4x4 oPos, r0, #camera \n\
			m3x3 r0 , $NORM, #pos \n\
			dp3 r0, r0, #light\n\
			max r0, r0, #zero \n\
			mul r0, r0, #light_color \n\
			add r0, r0, #ambient_color \n\
			min r0, r0, #one \n\
			mul oD0, r0, #color \n\
		");

	// *TODO* : alloc all sub-objects , with field name

	if( !omesh->Validate() ) {
		delete omesh;
		return NULL;
	}
	val_odata(o) = (value)omesh;
	val_gc(o,mesh_delete);
	return o;
}

value mesh_set_shader( value s ) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	if( !val_is_string(s) )
		return NULL;
	val_mesh(o)->SetShader( val_string(s) );
	return alloc_bool( val_mesh(o)->Validate() );
}

value mesh_get_shader() {
	value o = val_this();
	val_check_obj(o,t_mesh);
	return alloc_string( val_mesh(o)->GetShader().c_str() );
}

value mesh_has_tag( value s ) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	if( !val_is_string(s) || val_strlen(s) != 4 )
		return NULL;
	const char *str = val_string(s);
	TAG t = ((str[3]<<24) | (str[2]<<16) | (str[1]<<8) | str[0]);
	return alloc_bool( val_mesh(o)->HasTag(t) );
}

value mesh_new1( value r ) {
	return mesh_new2( r , NULLVAL);
}

value mesh_get_visible() {
	value o = val_this();
	val_check_obj(o,t_mesh);
	return alloc_bool(val_mesh(o)->visible);
}

value mesh_set_visible( value v ) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	if( !val_is_int(v) )
		return NULLVAL;
	val_mesh(o)->visible = val_bool(v);
	return alloc_bool( val_bool(v) );
}

value mesh_get_pos() {
	value o = val_this();
	val_check_obj(o,t_mesh);
	return alloc_matrix( &val_mesh(o)->pos );
}

value mesh_set_pos( value m ) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	if( val_is_obj(m,t_matrix) )
		val_mesh(o)->pos = *val_matrix(m);
	else if( val_is_obj(m,t_trs) )
		matrix_of_trs(m,&val_mesh(o)->pos);
	else
		return NULLVAL;
	val_mesh(o)->PosModified();
	return m;
}

value mesh_get_color() {
	value o = val_this();
	val_check_obj(o,t_mesh);
	return alloc_int( val_mesh(o)->color );
}

value mesh_set_color( value v ) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	if( !val_is_int(v) )
		return NULLVAL;
	val_mesh(o)->color = val_int(v);
	val_mesh(o)->ColorAlphaModified();
	return v;
}

value mesh_get_alpha() {
	value o = val_this();
	val_check_obj(o,t_mesh);
	return alloc_float( val_mesh(o)->alpha );
}

value mesh_set_alpha( value v ) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	if( !val_is_number(v) )
		return NULLVAL;
	val_mesh(o)->alpha = val_number(v);
	val_mesh(o)->ColorAlphaModified();
	return v;
}

value mesh_remove() {
	value o = val_this();
	val_check_obj(o,t_mesh);
	delete val_mesh(o);
	return val_true;
}

value mesh_get_texture() {
	value o = val_this();
	val_check_obj(o,t_mesh);
	MTW *t = val_mesh(o)->GetTexture();
	return t?alloc_resource(t):NULLVAL;
}

value mesh_set_texture( value v ) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	if( v == NULLVAL )
		val_mesh(o)->SetTexture(NULL);
	else {
		val_check_obj(v,t_resource);
		val_mesh(o)->SetTexture( (MTW*)val_resource(v) );
	}
	return v;
}

value mesh_scale( value v ) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	if( !val_is_number(v) )
		return NULL;
	MATRIX tmp;
	FLOAT f = val_number(v);
	D3DXMatrixScaling(&tmp,f,f,f);
	mmult(&val_mesh(o)->pos,&val_mesh(o)->pos,&tmp);
	val_mesh(o)->PosModified();
	return val_true;
}

value mesh_move( value x, value y, value z ) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	if( !val_is_number(x) || !val_is_number(y) || !val_is_number(z) )
		return NULL;
	MATRIX *m = &val_mesh(o)->pos;
	m->_41 += val_number(x);
	m->_42 += val_number(y);
	m->_43 += val_number(z);
	val_mesh(o)->PosModified();
	return val_true;
}

value mesh_rot( value x, value y, value z ) {
	value o = val_this();
	val_check_obj(o,t_mesh);
	if( !val_is_number(x) || !val_is_number(y) || !val_is_number(z) )
		return NULL;
	MATRIX tmp;
	D3DXMatrixRotationYawPitchRoll(&tmp,val_number(y),val_number(x),val_number(z));
	mmult(&val_mesh(o)->pos,&val_mesh(o)->pos,&tmp);
	val_mesh(o)->PosModified();
	return val_true;
}

value m3dse_mesh() {
	value o = alloc_class(&t_mesh);
	Constr(o,mesh,Mesh,1);
	Constr(o,mesh,Mesh,2);
	Property(o,mesh,visible);
	Property(o,mesh,pos);
	Property(o,mesh,color);
	Property(o,mesh,alpha);
	Property(o,mesh,texture);
	Property(o,mesh,shader);
	Method(o,mesh,remove,0);
	Method(o,mesh,scale,1);
	Method(o,mesh,move,3);
	Method(o,mesh,rot,3);
	Method(o,mesh,has_tag,1);
	return o; 
}

/* ************************************************************************ */
