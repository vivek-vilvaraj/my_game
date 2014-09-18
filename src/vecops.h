/*=============================================================================
Asteroids3D - a first person game of blowing up asteroids
  Copyright © Jan Engelhardt <jengelh [at] gmx de>, 2005

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public
  License along with this program kit; if not, write to:
  Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
  Boston, MA  02110-1301  USA
=============================================================================*/
#ifndef AS3D_VECOPS_H
#define AS3D_VECOPS_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <string.h>
#include "asteroids3D.h"

#if !defined(__NO_INLINE__) && defined(__OPTIMIZE__)
#	ifndef IMPLEMENT_vecops
#		define IMPLEMENT_vecops
#	endif
#	define autolinkage static inline
#else
#	ifdef IMPLEMENT_vecops
#		define autolinkage
#	endif
#	define DECLARE_vecops
#endif

extern const Vector ZERO_VEC;
#ifdef DECLARE_vecops
extern void vec_add(Vector *, const Vector *, const Vector *);
extern void vec_addmul(Vector *, const Vector *, double, const Vector *);
extern double vec_dist(const Vector *, const Vector *);
extern double vec_dot(const Vector *, const Vector *);
extern double vec_length(const Vector *);
extern void vec_sdiv(Vector *, const Vector *, double);
extern void vec_smul(Vector *, const Vector *, double);
extern void vec_sub(Vector *, const Vector *, const Vector *);
extern void vec_zero(Vector *);
extern double vp_length(double, double, double);
#endif // DECLARE_vecops

#ifdef IMPLEMENT_vecops
autolinkage void vec_add(Vector *r, const Vector *v, const Vector *w)
{
	r->x = v->x + w->x;
	r->y = v->y + w->y;
	r->z = v->z + w->z;
	//vec_addmul(r, v, 1, w);
	return;
}

autolinkage void vec_addmul(Vector *r, const Vector *v, double f,
    const Vector *w)
{
	r->x = v->x + f * w->x;
	r->y = v->y + f * w->y;
	r->z = v->z + f * w->z;
	return;
}

autolinkage void vec_sub(Vector *r, const Vector *v, const Vector *w)
{
	r->x = v->x - w->x;
	r->y = v->y - w->y;
	r->z = v->z - w->z;
	//vec_addmul(r, v, -1, w);
	return;
}

autolinkage void vec_smul(Vector *r, const Vector *v, double f)
{
	r->x = v->x * f;
	r->y = v->y * f;
	r->z = v->z * f;
	//vec_addmul(r, &ZERO_VEC, f, v);
	return;
}

autolinkage void vec_sdiv(Vector *r, const Vector *v, double f)
{
	r->x = v->x / f;
	r->y = v->y / f;
	r->z = v->z / f;
	//vec_addmul(r, &ZERO_VEC, 1 / f, v); // <- imprecise
	return;
}

autolinkage double vec_dot(const Vector *a, const Vector *b)
{
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

autolinkage double vec_length(const Vector *v)
{
	return sqrt(vec_dot(v, v));
}

autolinkage double vp_length(double x, double y, double z)
{
	return sqrt(x * x + y * y + z * z);
}

autolinkage double vec_dist(const Vector *a, const Vector *b)
{
	return vp_length(b->x - a->x, b->y - a->y, b->z - a->z);
}

autolinkage void vec_zero(Vector *v)
{
	memset(v, 0, sizeof(Vector));
	return;
}

#endif // IMPLEMENT_vecops

#undef DECLARE_vecops
#undef IMPLEMENT_vecops

#ifdef __cplusplus
} // extern "C"
#endif

#endif // AS3D_VECOPS_H
