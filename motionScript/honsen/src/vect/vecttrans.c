/* ************************************************************************ */
/*																			*/
/*	Honsen Vector Rendering													*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include <malloc.h>
#include <string.h>
#include <math.h>
#include "vecttrans.h"

/* ------------------------------------------------------------------------ */

void matrix_identity( matrix m ) {
	m[0] = 1;
	m[1] = 0;
	m[2] = 0;
	m[3] = 1;
	m[4] = 0;
	m[5] = 0;
}

/* ------------------------------------------------------------------------ */

void matrix_multiply( matrix out, matrix a, matrix b ) {
	double tmp0, tmp1, tmp2, tmp3, tmp4, tmp5;
	tmp0 = a[0] * b[0] + a[1] * b[2];
	tmp1 = a[0] * b[1] + a[1] * b[3];
	tmp2 = a[2] * b[0] + a[3] * b[2];
	tmp3 = a[2] * b[1] + a[3] * b[3];
	tmp4 = a[4] * b[0] + a[5] * b[2] + b[4];
	tmp5 = a[4] * b[1] + a[5] * b[3] + b[5];
	out[0] = tmp0;
	out[1] = tmp1;
	out[2] = tmp2;
	out[3] = tmp3;
	out[4] = tmp4;
	out[5] = tmp5;
}

/* ------------------------------------------------------------------------ */

void matrix_inverse( matrix out, matrix m ) {
	double det = m[0] * m[3] - m[1] * m[2];
	double tmp0, tmp1, tmp2, tmp3, tmp4, tmp5;
	if( fabs(det) < 0.00001 ) {
		matrix_identity(out);
		return;
	}
	tmp0 = m[3] / det;
	tmp1 = -m[1] / det;
	tmp2 = -m[2] / det;
	tmp3 = m[0] / det;
	tmp4 = (m[2] * m[5] - m[4] * m[3]) / det;
	tmp5 = (m[4] * m[1] - m[0] * m[5]) / det;
	out[0] = tmp0;
	out[1] = tmp1;
	out[2] = tmp2;
	out[3] = tmp3;
	out[4] = tmp4;
	out[5] = tmp5;
}

/* ------------------------------------------------------------------------ */

void matrix_copy( matrix out, matrix m ) {
	out[0] = m[0];
	out[1] = m[1];
	out[2] = m[2];
	out[3] = m[3];
	out[4] = m[4];
	out[5] = m[5];
}

/* ------------------------------------------------------------------------ */

BPath *bpath_transform( BPath *path, matrix m, int nsegs ) {
	BPath *out = malloc(sizeof(BPath)*(nsegs+1));
	BPath *tmp = out;
	float x,y;
	if( m[0] == 1 && m[1] == 0 && m[2] == 0 && m[3] == 1 ) {
		if( m[4] == 0 && m[5] == 0 )
			memcpy(out,path,sizeof(BPath)*(nsegs+1));
		else {
			x = (float)m[4];
			y = (float)m[5];
			while( nsegs-- ) {
				tmp->code = path->code;
				tmp->x = path->x + x;
				tmp->y = path->y + y;
				if( path->code == PATH_CURVETO ) {
					tmp->cx = path->cx + x;
					tmp->cy = path->cy + y;
				}
				path++;
				tmp++;
			}
			tmp->code = PATH_END;
		}
	} else {
		while( nsegs-- ) {
			x = (float)path->x;
			y = (float)path->y;
			tmp->code = path->code;
			tmp->x = x * (float)m[0] + y * (float)m[2] + (float)m[4];
			tmp->y = x * (float)m[1] + y * (float)m[3] + (float)m[5];
			if( path->code == PATH_CURVETO ) {
				x = path->cx;
				y = path->cy;
				tmp->cx = x * (float)m[0] + y * (float)m[2] + (float)m[4];
				tmp->cy = x * (float)m[1] + y * (float)m[3] + (float)m[5];
			}
			path++;
			tmp++;
		}
		tmp->code = PATH_END;
	}
	return out;
}

/* ------------------------------------------------------------------------ */

#define RENDER_LEVEL 4
#define RENDER_SIZE (1 << (RENDER_LEVEL))

static void render_bezier( VPath **vec, VPath *vmax, float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, float flatness ) {
	float x3_0, y3_0;
	float z3_0_dot;
	float z1_dot, z2_dot;
	float z1_perp, z2_perp;
	float max_perp_sq;

	float x_m, y_m;
	float xa1, ya1;
	float xa2, ya2;
	float xb1, yb1;
	float xb2, yb2;

	if( *vec >= vmax )
		return;

	x3_0 = x3 - x0;
	y3_0 = y3 - y0;

	z3_0_dot = x3_0 * x3_0 + y3_0 * y3_0;

	if (z3_0_dot < 0.001)
		goto nosubdivide;

	max_perp_sq = flatness * flatness * z3_0_dot;

	z1_perp = (y1 - y0) * x3_0 - (x1 - x0) * y3_0;
	if (z1_perp * z1_perp > max_perp_sq)
		goto subdivide;

	z2_perp = (y3 - y2) * x3_0 - (x3 - x2) * y3_0;
	if (z2_perp * z2_perp > max_perp_sq)
		goto subdivide;

	z1_dot = (x1 - x0) * x3_0 + (y1 - y0) * y3_0;
	if (z1_dot < 0 && z1_dot * z1_dot > max_perp_sq)
		goto subdivide;

	z2_dot = (x3 - x2) * x3_0 + (y3 - y2) * y3_0;
	if (z2_dot < 0 && z2_dot * z2_dot > max_perp_sq)
		goto subdivide;

	if (z1_dot + z1_dot > z3_0_dot)
		goto subdivide;

	if (z2_dot + z2_dot > z3_0_dot)
		goto subdivide;

nosubdivide:
	(*vec)->code = PATH_LINETO;
	(*vec)->x = x3;
	(*vec)->y = y3;
	(*vec)++;
	return;

subdivide:

	xa1 = (x0 + x1) * 0.5f;
	ya1 = (y0 + y1) * 0.5f;
	xa2 = (x0 + 2 * x1 + x2) * 0.25f;
	ya2 = (y0 + 2 * y1 + y2) * 0.25f;
	xb1 = (x1 + 2 * x2 + x3) * 0.25f;
	yb1 = (y1 + 2 * y2 + y3) * 0.25f;
	xb2 = (x2 + x3) * 0.5f;
	yb2 = (y2 + y3) * 0.5f;
	x_m = (xa2 + xb1) * 0.5f;
	y_m = (ya2 + yb1) * 0.5f;

	render_bezier(vec, vmax, x0, y0, xa1, ya1, xa2, ya2, x_m, y_m, flatness);
	render_bezier(vec, vmax, x_m, y_m, xb1, yb1, xb2, yb2, x3, y3, flatness);
}

/* ------------------------------------------------------------------------ */

int bpath_segment( VPath *vec, int nvecs, BPath *path, float epsilon ) {
	VPath *vec_max = vec + nvecs;
	float x = 0, y = 0;
	while( 1 ) {
		if( vec >= vec_max )
			return 0;
		switch( path->code ) {
		case PATH_MOVETO:
		case PATH_LINETO:
			x = path->x;
			y = path->y;
			vec->code = path->code;
			vec->x = x;
			vec->y = y;
			vec++;
			break;
		case PATH_END:
			vec->code = path->code;
			return 1;
		case PATH_CURVETO:
			render_bezier(&vec, vec_max, x, y, (x + path->cx * 2) / 3.0f, (y + path->cy * 2) / 3.0f, (path->x + path->cx * 2) / 3.0f, (path->y + path->cy * 2) / 3.0f, path->x, path->y, epsilon);
			x = path->x;
			y = path->y;
			break;
		}
		path++;
	}
	return 0;
}

/* ************************************************************************ */
