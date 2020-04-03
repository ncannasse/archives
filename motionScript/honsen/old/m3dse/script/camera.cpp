/* ************************************************************************ */
/*																			*/
/*	M3D-SE Camera for MTS													*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "script.h"

namespace M3D {

	extern VECTOR defpos;
	extern VECTOR deftarget;
	extern VECTOR defworldup;

	void InitScript( Context *c );
	void CloseScript( Context *c );

};

void camera_update( M3D::Context *c ) {
	MATRIX mtmp;
	D3DXMatrixLookAtLH(&mtmp,&c->script->camera_pos,&c->script->camera_target,&c->script->worldup);
	M3D::SetCamera(c,&mtmp);
}

void M3D::InitScript( M3D::Context *c ) {
	M3D::ScriptContext *sc = new M3D::ScriptContext;
	c->script = sc;
	sc->camera_pos = M3D::defpos;
	sc->camera_start_pos = M3D::defpos;
	sc->camera_target = M3D::deftarget;
	sc->worldup = M3D::defworldup;
	sc->camera_zoom = 1;
	sc->camera_roll = 0;
	camera_update(c);
}

void M3D::CloseScript( M3D::Context *c ) {
	delete c->script;
}

value camera_get_pos() {
	return alloc_vector( &M3D::__GetContext()->script->camera_pos );
}

value camera_get_target() {
	return alloc_vector( &M3D::__GetContext()->script->camera_target );
}

value camera_get_roll() {
	return alloc_float( M3D::__GetContext()->script->camera_roll );
}

value camera_get_proj() {
	return alloc_matrix( M3D::GetMatrixProj(M3D::__GetContext()) );
}

value camera_get_zoom() {
	return alloc_float( M3D::__GetContext()->script->camera_zoom );
}

value camera_set_pos( value v ) {
	val_check_obj(v,t_vector);
	M3D::Context *c = M3D::__GetContext();
	M3D::ScriptContext *sc = c?c->script:NULL;
	if( sc != NULL ) {
		sc->camera_pos = *val_vector(v);
		sc->camera_start_pos = *val_vector(v);
		sc->camera_zoom = 1;
		camera_update(c);
	}
	return v;
}

value camera_set_target( value v ) {
	val_check_obj(v,t_vector);
	M3D::Context *c = M3D::__GetContext();
	if( c && c->script ) {
		c->script->camera_target = *val_vector(v);
		camera_update(c);
	}
	return v;
}

value camera_set_roll( value v ) {
	if( !val_is_number(v) )
		return NULLVAL;
	M3D::Context *c = M3D::__GetContext();
	M3D::ScriptContext *sc = c->script;
	sc->camera_roll = (FLOAT)val_number(v);
	MATRIX m;
	VECTOR vec;
	VECTOR vecUp(0,0,1);
	vec = sc->camera_pos - sc->camera_target;
	D3DXMatrixRotationAxis(&m, &vec, (FLOAT)sc->camera_roll);
	D3DXVec3TransformCoord(&sc->worldup, &vecUp, &m);
	camera_update(c);
	return v;
}

value camera_set_proj( value v ) {
	val_check_obj(v,t_matrix);
	M3D::SetMatrixProj(M3D::__GetContext(),val_matrix(v));
	return v;
}

value camera_set_zoom( value v ) {
	if( !val_is_number(v) || fabs(val_number(v)) < 0.000001 ) 
		return NULLVAL;
	M3D::Context *c = M3D::__GetContext();
	c->script->camera_zoom = val_number(v);
	VECTOR delta = (c->script->camera_target - c->script->camera_start_pos);
	c->script->camera_pos = c->script->camera_target - delta / (FLOAT)c->script->camera_zoom;
	camera_update(c);
	return val_true;
}

value camera_set_world(value v) {
	val_check_obj(v,t_matrix);
	M3D::Context *c = M3D::__GetContext();
	M3D::SetWorld(c, val_matrix(v));
	return v;
}

value camera_get_world() {
	M3D::Context *c = M3D::__GetContext();
	return alloc_matrix(M3D::GetWorld(c));
}

value camera_toString() {
	return alloc_string("#camera");
}

value m3dse_camera() {
	value o = alloc_object(NULL);
	Property(o,camera,target);
	Property(o,camera,pos);
	Property(o,camera,proj);
	Property(o,camera,roll);
	Property(o,camera,zoom);
	Property(o,camera,world);
	Method(o,camera,toString,0);
	return o; 
}

DEFINE_PRIM(m3dse_camera,0);

/* ************************************************************************ */
