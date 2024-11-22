// Affine manipulation routines for PLplot.
//
// Copyright (C) 2009-2014 Alan W. Irwin
//
// This file is part of PLplot.
//
// PLplot is free software; you can redistribute it and/or modify
// it under the terms of the GNU Library General Public License as published
// by the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// PLplot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with PLplot; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
//
//

#include "plplotP.h"

//! @file
//!  These function perform variuos affine transformations.
//!
//! For background on these affine functions see SVG specification, e.g.,
//! http://www.w3.org/TR/SVGTiny12/coords.html#EstablishingANewUserSpace.
//! Affine 3x3 matrices with elements A_i,j always have a last row of
//! 0, 0, 1 so the elements are conveniently stored in a vector in the order
//! A_1_1, A_2_1, A_1_2, A_2_2, A_1_3, A_2_3, with the last row implied.
//!
//! N.B.  The PLplot affine interpretation of translate, scale, etc., is
//! as actions on coordinate systems, rather than actions on objects.  This
//! is identical to the SVG specficiation interpretation.  However, the
//! SVG specification interprets the affine matrix as follows:
//! old_coord_vector = affine_matrix * new_coordinate vector.  In PLplot
//! we use the alternative interpretation
//! new_coord_vector = affine_matrix * old_coordinate vector.
//! The result is all affine matrices below are the inverses of the SVG
//! equivalents.
//!
//! N.B. All PLplot affine functions below return the affine matrix result (in
//! vector form) as the first argument).  It is the calling routine's
//! responsibility to provide the space for all affine matrix arguments,
//! i.e., a PLFLT array with a dimension of 6.
//!
//

//! Returns affine identity matrix
//!
//! @param affine_vector Initialize a (pre-allocated) transform matrix.
//!
void
plP_affine_identity( PLFLT *affine_vector )
{
    affine_vector[0] = 1.;
    affine_vector[1] = 0.;
    affine_vector[2] = 0.;
    affine_vector[3] = 1.;
    affine_vector[4] = 0.;
    affine_vector[5] = 0.;
}

//! Translate new coordinate system axes relative to the old.
//!
//! @param affine_vector Pre-allocated storage for a translation matrix.
//! @param xtranslate Amount to translate in x.
//! @param ytranslate Amount to translate in y.
//!
void
plP_affine_translate( PLFLT *affine_vector, PLFLT xtranslate, PLFLT ytranslate )
{
    affine_vector[0] = 1.;
    affine_vector[1] = 0.;
    affine_vector[2] = 0.;
    affine_vector[3] = 1.;
    // If the new coordinate system axis is shifted by xtranslate and ytranslate
    // relative to the old, then the actual new coordinates are shifted in
    // the opposite direction.
    affine_vector[4] = -xtranslate;
    affine_vector[5] = -ytranslate;
}

//! Scale new coordinate system axes relative to the old.
//!
//! @param affine_vector Pre-allocate storage for a scale matrix.
//! @param xscale Amount to scale in x.
//! @param yscale Amount to scale in y.
//!
void
plP_affine_scale( PLFLT *affine_vector, PLFLT xscale, PLFLT yscale )
{
    // If the new coordinate system axes are scaled by xscale and yscale
    // relative to the old, then the actual new coordinates are scaled
    // by the inverses.
    if ( xscale == 0. )
    {
        plwarn( "plP_affine_scale: attempt to scale X coordinates by zero." );
        xscale = 1.;
    }
    if ( yscale == 0. )
    {
        plwarn( "plP_affine_scale: attempt to scale Y coordinates by zero." );
        yscale = 1.;
    }
    affine_vector[0] = 1. / xscale;
    affine_vector[1] = 0.;
    affine_vector[2] = 0.;
    affine_vector[3] = 1. / yscale;
    affine_vector[4] = 0.;
    affine_vector[5] = 0.;
}

//! Rotate new coordinate system axes relative to the old.
//! angle is in degrees.
//!
//! @param affine_vector Pre-allocated storage for a rotation matrix.
//! @param angle Amount to rotate in degrees.
//!
void
plP_affine_rotate( PLFLT *affine_vector, PLFLT angle )
{
    PLFLT cosangle = cos( PI * angle / 180. );
    PLFLT sinangle = sin( PI * angle / 180. );
    affine_vector[0] = cosangle;
    affine_vector[1] = -sinangle;
    affine_vector[2] = sinangle;
    affine_vector[3] = cosangle;
    affine_vector[4] = 0.;
    affine_vector[5] = 0.;
}

//! Skew new X coordinate axis relative to the old.
//! angle is in degrees.
//!
//! @param affine_vector Pre-allocated storage for a skew (in x) matrix.
//! @param angle Amount to skew in degrees.
//!

void
plP_affine_xskew( PLFLT *affine_vector, PLFLT angle )
{
    PLFLT tanangle = tan( PI * angle / 180. );
    affine_vector[0] = 1.;
    affine_vector[1] = 0.;
    affine_vector[2] = -tanangle;
    affine_vector[3] = 1.;
    affine_vector[4] = 0.;
    affine_vector[5] = 0.;
}

//! Skew new Y coordinate axis relative to the old.
//! angle is in degrees.
//!
//! @param affine_vector Pre-allocated storage for a skew (in y) matrix.
//! @param angle Amount to skew in degrees.
//!

void
plP_affine_yskew( PLFLT *affine_vector, PLFLT angle )
{
    PLFLT tanangle = tan( PI * angle / 180. );
    affine_vector[0] = 1.;
    affine_vector[1] = -tanangle;
    affine_vector[2] = 0.;
    affine_vector[3] = 1.;
    affine_vector[4] = 0.;
    affine_vector[5] = 0.;
}

//! Multiply two affine transformation matrices to form a third.
//!
//! A = B * C
//!
//! @param affine_vectorA Pre-allocated storage for the result of
//! multiplying matrix affine_vectorB by matrix affine_vectorC.
//! @param affine_vectorB First matrix to multiply.
//! @param affine_vectorC Second matrix to multiply.
//!

void
plP_affine_multiply(
    PLFLT *affine_vectorA,
    PLFLT_VECTOR affine_vectorB,
    PLFLT_VECTOR affine_vectorC )
{
    int   i;
    PLFLT result[NAFFINE];
    // Multiply two affine matrices stored in affine vector form.
    result[0] = affine_vectorB[0] * affine_vectorC[0] +
                affine_vectorB[2] * affine_vectorC[1];
    result[2] = affine_vectorB[0] * affine_vectorC[2] +
                affine_vectorB[2] * affine_vectorC[3];
    result[4] = affine_vectorB[0] * affine_vectorC[4] +
                affine_vectorB[2] * affine_vectorC[5] +
                affine_vectorB[4];

    result[1] = affine_vectorB[1] * affine_vectorC[0] +
                affine_vectorB[3] * affine_vectorC[1];
    result[3] = affine_vectorB[1] * affine_vectorC[2] +
                affine_vectorB[3] * affine_vectorC[3];
    result[5] = affine_vectorB[1] * affine_vectorC[4] +
                affine_vectorB[3] * affine_vectorC[5] +
                affine_vectorB[5];

    for ( i = 0; i < NAFFINE; i++ )
        affine_vectorA[i] = result[i];
}
