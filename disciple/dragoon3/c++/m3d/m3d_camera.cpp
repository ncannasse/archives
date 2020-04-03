/* **************************************************************************** */
/*																				*/
/*   M3D Camera control															*/
/*	 Nicolas Cannasse															*/
/*	 (c)2002 Motion-Twin														*/
/*																				*/
/* **************************************************************************** */
#include <m3d.h>

namespace M3D {
/* ---------------------------------------------------------------------------- */

namespace Camera {

	const VECTOR defpos(0,-200,100);
	const VECTOR deftarget(0,0,30);
	const VECTOR worldup(0,0,1);

	MVec pos(defpos);
	MVec target(deftarget);
	MMat view(IDMATRIX);

	void Reset() {
		pos.Set(defpos);
		target.Set(deftarget);
		Update();
	}

	void Update() {
		if( pos.IsModified() || target.IsModified() ) {			
			D3DXMatrixLookAtLH(view.Get(),pos.Get(),target.Get(),&worldup);			
			pos.ClearModified();
			target.ClearModified();
			view.Invalidate();
		}
	}

	void Zoom( FLOAT f ) {		
		pos.Set((*pos.Get()-*target.Get())*f+*target.Get());
	}

	void Goto( VECTOR p, VECTOR t, FLOAT time ) {
		FX::Lerp::V2Lerp(&pos,pos.Get(),&p,time);
		FX::Lerp::V2Lerp(&target,target.Get(),&t,time);
	}

	void Stop() {
		FX::Lerp::Cancel(&pos);
		FX::Lerp::Cancel(&target);
	}
};

/* ---------------------------------------------------------------------------- */
}; // namespace
/* **************************************************************************** */
