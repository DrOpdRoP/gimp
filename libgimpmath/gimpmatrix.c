/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-1997 Peter Mattis and Spencer Kimball
 *
 * gimpmatrix.c
 * Copyright (C) 1998 Jay Cox <jaycox@earthlink.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "config.h"

#include <glib.h>

#include "gimpmath.h"


#define EPSILON 1e-6


/**
 * gimp_matrix2_identity:
 * @matrix: A matrix.
 *
 * Sets the matrix to the identity matrix.
 */
void
gimp_matrix2_identity (GimpMatrix2 *matrix)
{
  static const GimpMatrix2 identity = { { { 1.0, 0.0 },
                                          { 0.0, 1.0 } } };

  *matrix = identity;
}

/**
 * gimp_matrix2_mult:
 * @matrix1: The first input matrix.
 * @matrix2: The second input matrix which will be overwritten by the result.
 *
 * Multiplies two matrices and puts the result into the second one.
 */
void
gimp_matrix2_mult (const GimpMatrix2 *matrix1,
                   GimpMatrix2       *matrix2)
{
  GimpMatrix2  tmp;

  tmp.coeff[0][0] = (matrix1->coeff[0][0] * matrix2->coeff[0][0] +
                     matrix1->coeff[0][1] * matrix2->coeff[1][0]);
  tmp.coeff[0][1] = (matrix1->coeff[0][0] * matrix2->coeff[0][1] +
                     matrix1->coeff[0][1] * matrix2->coeff[1][1]);
  tmp.coeff[1][0] = (matrix1->coeff[1][0] * matrix2->coeff[0][0] +
                     matrix1->coeff[1][1] * matrix2->coeff[1][0]);
  tmp.coeff[1][1] = (matrix1->coeff[1][0] * matrix2->coeff[0][1] +
                     matrix1->coeff[1][1] * matrix2->coeff[1][1]);

  *matrix2 = tmp;
}

/**
 * gimp_matrix3_identity:
 * @matrix: A matrix.
 *
 * Sets the matrix to the identity matrix.
 */
void
gimp_matrix3_identity (GimpMatrix3 *matrix)
{
  static const GimpMatrix3 identity = { { { 1.0, 0.0, 0.0 },
                                          { 0.0, 1.0, 0.0 },
                                          { 0.0, 0.0, 1.0 } } };

  *matrix = identity;
}

/**
 * gimp_matrix3_transform_point:
 * @matrix: The transformation matrix.
 * @x: The source X coordinate.
 * @y: The source Y coordinate.
 * @newx: The transformed X coordinate.
 * @newy: The transformed Y coordinate.
 *
 * Transforms a point in 2D as specified by the transformation matrix.
 */
void
gimp_matrix3_transform_point (const GimpMatrix3 *matrix,
                              gdouble            x,
                              gdouble            y,
                              gdouble           *newx,
                              gdouble           *newy)
{
  gdouble  w;

  w = matrix->coeff[2][0] * x + matrix->coeff[2][1] * y + matrix->coeff[2][2];

  if (w == 0.0)
    w = 1.0;
  else
    w = 1.0/w;

  *newx = (matrix->coeff[0][0] * x +
           matrix->coeff[0][1] * y +
           matrix->coeff[0][2]) * w;
  *newy = (matrix->coeff[1][0] * x +
           matrix->coeff[1][1] * y +
           matrix->coeff[1][2]) * w;
}

/**
 * gimp_matrix3_mult:
 * @matrix1: The first input matrix.
 * @matrix2: The second input matrix which will be overwritten by the result.
 *
 * Multiplies two matrices and puts the result into the second one.
 */
void
gimp_matrix3_mult (const GimpMatrix3 *matrix1,
                   GimpMatrix3       *matrix2)
{
  gint         i, j;
  GimpMatrix3  tmp;
  gdouble      t1, t2, t3;

  for (i = 0; i < 3; i++)
    {
      t1 = matrix1->coeff[i][0];
      t2 = matrix1->coeff[i][1];
      t3 = matrix1->coeff[i][2];

      for (j = 0; j < 3; j++)
        {
          tmp.coeff[i][j]  = t1 * matrix2->coeff[0][j];
          tmp.coeff[i][j] += t2 * matrix2->coeff[1][j];
          tmp.coeff[i][j] += t3 * matrix2->coeff[2][j];
        }
    }

  *matrix2 = tmp;
}

/**
 * gimp_matrix3_translate:
 * @matrix: The matrix that is to be translated.
 * @x: Translation in X direction.
 * @y: Translation in Y direction.
 *
 * Translates the matrix by x and y.
 */
void
gimp_matrix3_translate (GimpMatrix3 *matrix,
                        gdouble      x,
                        gdouble      y)
{
  gdouble g, h, i;

  g = matrix->coeff[2][0];
  h = matrix->coeff[2][1];
  i = matrix->coeff[2][2];

  matrix->coeff[0][0] += x * g;
  matrix->coeff[0][1] += x * h;
  matrix->coeff[0][2] += x * i;
  matrix->coeff[1][0] += y * g;
  matrix->coeff[1][1] += y * h;
  matrix->coeff[1][2] += y * i;
}

/**
 * gimp_matrix3_scale:
 * @matrix: The matrix that is to be scaled.
 * @x: X scale factor.
 * @y: Y scale factor.
 *
 * Scales the matrix by x and y
 */
void
gimp_matrix3_scale (GimpMatrix3 *matrix,
                    gdouble      x,
                    gdouble      y)
{
  matrix->coeff[0][0] *= x;
  matrix->coeff[0][1] *= x;
  matrix->coeff[0][2] *= x;

  matrix->coeff[1][0] *= y;
  matrix->coeff[1][1] *= y;
  matrix->coeff[1][2] *= y;
}

/**
 * gimp_matrix3_rotate:
 * @matrix: The matrix that is to be rotated.
 * @theta: The angle of rotation (in radians).
 *
 * Rotates the matrix by theta degrees.
 */
void
gimp_matrix3_rotate (GimpMatrix3 *matrix,
                     gdouble      theta)
{
  gdouble t1, t2;
  gdouble cost, sint;

  cost = cos (theta);
  sint = sin (theta);

  t1 = matrix->coeff[0][0];
  t2 = matrix->coeff[1][0];
  matrix->coeff[0][0] = cost * t1 - sint * t2;
  matrix->coeff[1][0] = sint * t1 + cost * t2;

  t1 = matrix->coeff[0][1];
  t2 = matrix->coeff[1][1];
  matrix->coeff[0][1] = cost * t1 - sint * t2;
  matrix->coeff[1][1] = sint * t1 + cost * t2;

  t1 = matrix->coeff[0][2];
  t2 = matrix->coeff[1][2];
  matrix->coeff[0][2] = cost * t1 - sint * t2;
  matrix->coeff[1][2] = sint * t1 + cost * t2;
}

/**
 * gimp_matrix3_xshear:
 * @matrix: The matrix that is to be sheared.
 * @amount: X shear amount.
 *
 * Shears the matrix in the X direction.
 */
void
gimp_matrix3_xshear (GimpMatrix3 *matrix,
                     gdouble      amount)
{
  matrix->coeff[0][0] += amount * matrix->coeff[1][0];
  matrix->coeff[0][1] += amount * matrix->coeff[1][1];
  matrix->coeff[0][2] += amount * matrix->coeff[1][2];
}

/**
 * gimp_matrix3_yshear:
 * @matrix: The matrix that is to be sheared.
 * @amount: Y shear amount.
 *
 * Shears the matrix in the Y direction.
 */
void
gimp_matrix3_yshear (GimpMatrix3 *matrix,
                     gdouble      amount)
{
  matrix->coeff[1][0] += amount * matrix->coeff[0][0];
  matrix->coeff[1][1] += amount * matrix->coeff[0][1];
  matrix->coeff[1][2] += amount * matrix->coeff[0][2];
}


/**
 * gimp_matrix3_affine:
 * @matrix: The input matrix.
 * @a:
 * @b:
 * @c:
 * @d:
 * @e:
 * @f:
 *
 * Applies the affine transformation given by six values to @matrix.
 * The six values form define an affine transformation matrix as
 * illustrated below:
 *
 *  ( a c e )
 *  ( b d f )
 *  ( 0 0 1 )
 **/
void
gimp_matrix3_affine (GimpMatrix3 *matrix,
                     gdouble      a,
                     gdouble      b,
                     gdouble      c,
                     gdouble      d,
                     gdouble      e,
                     gdouble      f)
{
  GimpMatrix3 affine;

  affine.coeff[0][0] = a;
  affine.coeff[1][0] = b;
  affine.coeff[2][0] = 0.0;

  affine.coeff[0][1] = c;
  affine.coeff[1][1] = d;
  affine.coeff[2][1] = 0.0;

  affine.coeff[0][2] = e;
  affine.coeff[1][2] = f;
  affine.coeff[2][2] = 1.0;

  gimp_matrix3_mult (&affine, matrix);
}


/**
 * gimp_matrix3_determinant:
 * @matrix: The input matrix.
 *
 * Calculates the determinant of the given matrix.
 *
 * Returns: The determinant.
 */
gdouble
gimp_matrix3_determinant (const GimpMatrix3 *matrix)
{
  gdouble determinant;

  determinant  = (matrix->coeff[0][0] *
                  (matrix->coeff[1][1] * matrix->coeff[2][2] -
                   matrix->coeff[1][2] * matrix->coeff[2][1]));
  determinant -= (matrix->coeff[1][0] *
                  (matrix->coeff[0][1] * matrix->coeff[2][2] -
                   matrix->coeff[0][2] * matrix->coeff[2][1]));
  determinant += (matrix->coeff[2][0] *
                  (matrix->coeff[0][1] * matrix->coeff[1][2] -
                   matrix->coeff[0][2] * matrix->coeff[1][1]));

  return determinant;
}

/**
 * gimp_matrix3_invert:
 * @matrix: The matrix that is to be inverted.
 *
 * Inverts the given matrix.
 */
void
gimp_matrix3_invert (GimpMatrix3 *matrix)
{
  GimpMatrix3 inv;
  gdouble     det;

  det = gimp_matrix3_determinant (matrix);

  if (det == 0.0)
    return;

  det = 1.0 / det;

  inv.coeff[0][0] =   (matrix->coeff[1][1] * matrix->coeff[2][2] -
                       matrix->coeff[1][2] * matrix->coeff[2][1]) * det;

  inv.coeff[1][0] = - (matrix->coeff[1][0] * matrix->coeff[2][2] -
                       matrix->coeff[1][2] * matrix->coeff[2][0]) * det;

  inv.coeff[2][0] =   (matrix->coeff[1][0] * matrix->coeff[2][1] -
                       matrix->coeff[1][1] * matrix->coeff[2][0]) * det;

  inv.coeff[0][1] = - (matrix->coeff[0][1] * matrix->coeff[2][2] -
                       matrix->coeff[0][2] * matrix->coeff[2][1]) * det;

  inv.coeff[1][1] =   (matrix->coeff[0][0] * matrix->coeff[2][2] -
                       matrix->coeff[0][2] * matrix->coeff[2][0]) * det;

  inv.coeff[2][1] = - (matrix->coeff[0][0] * matrix->coeff[2][1] -
                       matrix->coeff[0][1] * matrix->coeff[2][0]) * det;

  inv.coeff[0][2] =   (matrix->coeff[0][1] * matrix->coeff[1][2] -
                       matrix->coeff[0][2] * matrix->coeff[1][1]) * det;

  inv.coeff[1][2] = - (matrix->coeff[0][0] * matrix->coeff[1][2] -
                       matrix->coeff[0][2] * matrix->coeff[1][0]) * det;

  inv.coeff[2][2] =   (matrix->coeff[0][0] * matrix->coeff[1][1] -
                       matrix->coeff[0][1] * matrix->coeff[1][0]) * det;

  *matrix = inv;
}


/*  functions to test for matrix properties  */


/**
 * gimp_matrix3_is_diagonal:
 * @matrix: The matrix that is to be tested.
 *
 * Checks if the given matrix is diagonal.
 *
 * Returns: TRUE if the matrix is diagonal.
 */
gboolean
gimp_matrix3_is_diagonal (const GimpMatrix3 *matrix)
{
  gint i, j;

  for (i = 0; i < 3; i++)
    {
      for (j = 0; j < 3; j++)
        {
          if (i != j && fabs (matrix->coeff[i][j]) > EPSILON)
            return FALSE;
        }
    }

  return TRUE;
}

/**
 * gimp_matrix3_is_identity:
 * @matrix: The matrix that is to be tested.
 *
 * Checks if the given matrix is the identity matrix.
 *
 * Returns: TRUE if the matrix is the identity matrix.
 */
gboolean
gimp_matrix3_is_identity (const GimpMatrix3 *matrix)
{
  gint i, j;

  for (i = 0; i < 3; i++)
    {
      for (j = 0; j < 3; j++)
        {
          if (i == j)
            {
              if (fabs (matrix->coeff[i][j] - 1.0) > EPSILON)
                return FALSE;
            }
          else
            {
              if (fabs (matrix->coeff[i][j]) > EPSILON)
                return FALSE;
            }
        }
    }

  return TRUE;
}

/*  Check if we'll need to interpolate when applying this matrix.
    This function returns TRUE if all entries of the upper left
    2x2 matrix are either 0 or 1
 */


/**
 * gimp_matrix3_is_simple:
 * @matrix: The matrix that is to be tested.
 *
 * Checks if we'll need to interpolate when applying this matrix as
 * a transformation.
 *
 * Returns: TRUE if all entries of the upper left 2x2 matrix are either
 * 0 or 1
 */
gboolean
gimp_matrix3_is_simple (const GimpMatrix3 *matrix)
{
  gdouble absm;
  gint    i, j;

  for (i = 0; i < 2; i++)
    {
      for (j = 0; j < 2; j++)
        {
          absm = fabs (matrix->coeff[i][j]);
          if (absm > EPSILON && fabs (absm - 1.0) > EPSILON)
            return FALSE;
        }
    }

  return TRUE;
}

/**
 * gimp_matrix4_to_deg:
 * @matrix:
 * @a:
 * @b:
 * @c:
 *
 *
 **/
void
gimp_matrix4_to_deg (const GimpMatrix4 *matrix,
                     gdouble           *a,
                     gdouble           *b,
                     gdouble           *c)
{
  *a = 180 * (asin (matrix->coeff[1][0]) / G_PI_2);
  *b = 180 * (asin (matrix->coeff[2][0]) / G_PI_2);
  *c = 180 * (asin (matrix->coeff[2][1]) / G_PI_2);
}
