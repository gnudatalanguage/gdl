/***************************************************************************
                          brent.cpp  -  GDL library function
                             -------------------
    begin                : Jul 12 2017
    copyright            : (C) 2017 by Gilles Duvert using J.B.'s praxis implementation
                           of R. Brent minimisation code. See Copyright and License below. 
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

# include <cmath>
# include <cstdlib>
# include <cstring>
# include <ctime>
# include <iomanip>
# include <iostream>

using namespace std;

double flin ( int n, int j, double l, double f ( double x[], int n ), 
  double x[], int &nf, double v[], double q0[], double q1[], double &qd0, 
  double &qd1, double &qa, double &qb, double &qc );
void minfit ( int n, double tol, double a[], double q[] );
void minny ( int n, int j, int nits, double &d2, double &x1, double &f1, 
  bool fk, double f ( double x[], int n ), double x[], double t, double h, 
  double v[], double q0[], double q1[], int &nl, int &nf, double dmin, 
  double ldt, double &fx, double &qa, double &qb, double &qc, double &qd0, 
  double &qd1 );
double praxis ( double t0, double h0, int n, int prin, double x[], 
  double f ( double x[], int n ) );
void print2 ( int n, double x[], int prin, double fx, int nf, int nl );
void quad ( int n, double f ( double x[], int n ), double x[], double t, 
  double h, double v[], double q0[], double q1[], int &nl, int &nf, double dmin, 
  double ldt, double &fx, double &qf1, double &qa, double &qb, double &qc, 
  double &qd0, double &qd1 );
double r8_epsilon ( );
double r8_hypot ( double x, double y );
double r8_max ( double x, double y );
double r8_min ( double x, double y );
double r8_uniform_01 ( int &seed );
void r8mat_print ( int m, int n, double a[], string title );
void r8mat_print_some ( int m, int n, double a[], int ilo, int jlo, int ihi,
  int jhi, string title );
void r8mat_transpose_in_place ( int n, double a[] );
void r8vec_copy ( int n, double a1[], double a2[] );
double r8vec_max ( int n, double r8vec[] );
double r8vec_min ( int n, double r8vec[] );
double r8vec_norm ( int n, double a[] );
void r8vec_print ( int n, double a[], string title );
void svsort ( int n, double d[], double v[] ) ;


//****************************************************************************80

double flin ( int n, int jsearch, double l, double f ( double x[], int n ), 
  double x[], int &nf, double v[], double q0[], double q1[], double &qd0, 
  double &qd1, double &qa, double &qb, double &qc )

//****************************************************************************80
//
//  Purpose:
//
//    FLIN is the function of one variable to be minimized by MINNY.
//
//  Discussion:
//
//    F(X) is a scalar function of a vector argument X.
//
//    A minimizer of F(X) is sought along a line or parabola.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    04 August 2016
//
//  Author:
//
//    Original FORTRAN77 version by Richard Brent.
//    C++ version by John Burkardt.
//
//  Reference:
//
//    Richard Brent,
//    Algorithms for Minimization with Derivatives,
//    Prentice Hall, 1973,
//    Reprinted by Dover, 2002.
//
//  Parameters:
//
//    Input, int N, the number of variables.
//
//    Input, int JSEARCH, indicates the kind of search.
//    If JSEARCH is a legal column index, linear search along V(*,JSEARCH).
//    If JSEARCH is -1, then the search is parabolic, based on X, Q0 and Q1.
//
//    Input, double L, is the parameter determining the particular
//    point at which F is to be evaluated.  
//    For a linear search, L is the step size.
//    For a quadratic search, L is a parameter which specifies
//    a point in the plane of X, Q0 and Q1.
//
//    Input, double F ( double X[], int N ), the function to be minimized.
//
//    Input, double X[N], the base point of the search.
//
//    Input/output, int &NF, the function evaluation counter.
//
//    Input, double V[N,N], a matrix whose columns constitute 
//    search directions.
//
//    Input, double Q0[N], Q1[N], two auxiliary points used to
//    determine the plane when a quadratic search is performed.
//
//    Input, double &QD0, &QD1, values needed to compute the 
//    coefficients QA, QB, QC.
//
//    Output, double &QA, &QB, &QC, coefficients used to combine
//    Q0, X, and A1 if a quadratic search is used.
//
//    Output, double FLIN, the value of the function at the 
//    minimizing point.
//
{
  int i;
  double *t;
  double value;

  t = new double[n];
//
//  The search is linear.
//
  if ( 0 <= jsearch )
  {
    for ( i = 0; i < n; i++ )
    {
      t[i] = x[i] + l * v[i+jsearch*n];
    }
  }
//
//  The search is along a parabolic space curve.
//
  else
  {
    qa =                 l * ( l - qd1 ) /       ( qd0 + qd1 ) / qd0;
    qb = - ( l + qd0 ) *     ( l - qd1 ) / qd1                 / qd0;
    qc =   ( l + qd0 ) * l               / qd1 / ( qd0 + qd1 );

    for ( i = 0; i < n; i++ )
    {
      t[i] = qa * q0[i] + qb * x[i] + qc * q1[i];
    }
  }
//
//  The function evaluation counter NF is incremented.
//
  nf = nf + 1;
//
//  Evaluate the function.
//
  value = f ( t, n );

  delete [] t;

  return value;
}
//****************************************************************************80

void minfit ( int n, double tol, double a[], double q[] )

//****************************************************************************80
//
//  Purpose:
//
//    MINFIT computes the singular value decomposition of an N by N array.
//
//  Discussion:
//
//    This is an improved version of the EISPACK routine MINFIT
//    restricted to the case M = N and P = 0.
//
//    The singular values of the array A are returned in Q.  A is
//    overwritten with the orthogonal matrix V such that U * diag(Q) = A * V,
//    where U is another orthogonal matrix.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    04 August 2016
//
//  Author:
//
//    Original FORTRAN77 version by Richard Brent.
//    C++ version by John Burkardt.
//
//  Reference:
//
//    Richard Brent,
//    Algorithms for Minimization with Derivatives,
//    Prentice Hall, 1973,
//    Reprinted by Dover, 2002.
//
//    James Wilkinson, Christian Reinsch,
//    Handbook for Automatic Computation,
//    Volume II, Linear Algebra, Part 2,
//    Springer Verlag, 1971.
//
//    Brian Smith, James Boyle, Jack Dongarra, Burton Garbow, Yasuhiko Ikebe, 
//    Virginia Klema, Cleve Moler,
//    Matrix Eigensystem Routines, EISPACK Guide,
//    Lecture Notes in Computer Science, Volume 6,
//    Springer Verlag, 1976,
//    ISBN13: 978-3540075462,
//    LC: QA193.M37.
//
//  Parameters:
//
//    Input, int N, the order of the matrix A.
//
//    Input, double TOL, a tolerance which determines when a vector
//    (a column or part of a column of the matrix) may be considered
//    "essentially" equal to zero.
//
//    Input/output, double A[N,N].  On input, an N by N array whose
//    singular value decomposition is desired.  On output, the
//    SVD orthogonal matrix factor V.
//
//    Input/output, double Q[N], the singular values.
//
{
  double c;
  double *e;
  double eps;
  double f;
  double g;
  double h;
  int i;
  int ii;
  int j;
  int jj;
  int k;
  int kk;
  int kt;
  const int kt_max = 30;
  int l;
  int l2;
  int ll2;
  double s;
  int skip;
  double temp;
  double x;
  double y;
  double z;
//
//  Householder's reduction to bidiagonal form.
//
  if ( n == 1 )
  {
    q[0] = a[0+0*n];
    a[0+0*n] = 1.0;
    return;
  }

  e = new double[n];

  eps = r8_epsilon ( );
  g = 0.0;
  x = 0.0;

  for ( i = 1; i <= n; i++ )
  {
    e[i-1] = g;
    l = i + 1;

    s = 0.0;
    for ( ii = i; ii <= n; ii++ )
    {
      s = s + a[ii-1+(i-1)*n] * a[ii-1+(i-1)*n];
    }

    g = 0.0;

    if ( tol <= s )
    {
      f = a[i-1+(i-1)*n];

      g = sqrt ( s );

      if ( 0.0 <= f )
      {
        g = - g;
      }

      h = f * g - s;
      a[i-1+(i-1)*n] = f - g;

      for ( j = l; j <= n; j++ )
      {
        f = 0.0;
        for ( ii = i; ii <= n; ii++ )
        {
          f = f + a[ii-1+(i-1)*n] * a[ii-1+(j-1)*n];
        }
        f = f / h;

        for ( ii = i; ii <= n; ii++ )
        {
          a[ii-1+(j-1)*n] = a[ii-1+(j-1)*n] + f * a[ii-1+(i-1)*n];
        }
      } 
    }

    q[i-1] = g;

    s = 0.0;
    for ( j = l; j <= n; j++ )
    {
      s = s + a[i-1+(j-1)*n] * a[i-1+(j-1)*n];
    }

    g = 0.0;

    if ( tol <= s )
    {
      if ( i < n )
      {
        f = a[i-1+i*n];
      }

      g = sqrt ( s );

      if ( 0.0 <= f )
      {
        g = - g;
      }

      h = f * g - s;

      if ( i < n )
      {
        a[i-1+i*n] = f - g;
        for ( jj = l; jj <= n; jj++ )
        {
          e[jj-1] = a[i-1+(jj-1)*n] / h;
        }

        for ( j = l; j <= n; j++ )
        {
          s = 0.0;
          for ( jj = l; jj <= n; jj++ )
          {
            s = s + a[j-1+(jj-1)*n] * a[i-1+(jj-1)*n];
          }
          for ( jj = l; jj <= n; jj++ )
          {
            a[j-1+(jj-1)*n] = a[j-1+(jj-1)*n] + s * e[jj-1];
          }
        }
      }
    }

    y = fabs ( q[i-1] ) + fabs ( e[i-1] );

    x = r8_max ( x, y );
  }
//
//  Accumulation of right-hand transformations.
//
  a[n-1+(n-1)*n] = 1.0;
  g = e[n-1];
  l = n;

  for ( i = n - 1; 1 <= i; i-- )
  {
    if ( g != 0.0 )
    {
      h = a[i-1+i*n] * g;

      for ( ii = l; ii <= n; ii++ )
      {
        a[ii-1+(i-1)*n] = a[i-1+(ii-1)*n] / h;
      }

      for ( j = l; j <= n; j++ )
      {
        s = 0.0;
        for ( jj = l; jj <= n; jj++ )
        {
          s = s + a[i-1+(jj-1)*n] * a[jj-1+(j-1)*n];
        }

        for ( ii = l; ii <= n; ii++ )
        {
          a[ii-1+(j-1)*n] = a[ii-1+(j-1)*n] + s * a[ii-1+(i-1)*n];
        }
      }
    }

    for ( jj = l; jj <= n; jj++ )
    {
      a[i-1+(jj-1)*n] = 0.0;
    }

    for ( ii = l; ii <= n; ii++ )
    {
      a[ii-1+(i-1)*n] = 0.0;
    }

    a[i-1+(i-1)*n] = 1.0;

    g = e[i-1];

    l = i;
  }
//
//  Diagonalization of the bidiagonal form.
//
  eps = eps * x;

  for ( k = n; 1 <= k; k-- )
  {
    kt = 0;

    for ( ; ; )
    {
      kt = kt + 1;

      if ( kt_max < kt )
      {
        e[k-1] = 0.0;
        cerr << "\n";
        cerr << "MINFIT - Fatal error!\n";
        cerr << "  The QR algorithm failed to converge.\n";
        exit ( 1 );
      }

      skip = 0;

      for ( l2 = k; 1 <= l2; l2-- )
      {
        l = l2;

        if ( fabs ( e[l-1] ) <= eps )
        {
          skip = 1;
          break;
        }

        if ( 1 < l )
        {
          if ( fabs ( q[l-2] ) <= eps )
          {
            break;
          }
        }
      }
//
//  Cancellation of E(L) if 1 < L.
//
      if ( ! skip )
      {
        c = 0.0;
        s = 1.0;

        for ( i = l; i <= k; i++ )
        {
          f = s * e[i-1];
          e[i-1] = c * e[i-1];
          if ( fabs ( f ) <= eps )
          {
            break;
          }
          g = q[i-1];
//
//  q(i) = h = sqrt(g*g + f*f).
//
          h = r8_hypot ( f, g );
  
          q[i-1] = h;

          if ( h == 0.0 )
          {
            g = 1.0;
            h = 1.0;
          }

          c =   g / h;
          s = - f / h;
        }
      }
//
//  Test for convergence for this index K.
//
      z = q[k-1];

      if ( l == k )
      {
        if ( z < 0.0 )
        {
          q[k-1] = - z;
          for ( i = 1; i <= n; i++ )
          {
            a[i-1+(k-1)*n] = - a[i-1+(k-1)*n];
          }
        }
        break;
      }
//
//  Shift from bottom 2*2 minor.
//
      x = q[l-1];
      y = q[k-2];
      g = e[k-2];
      h = e[k-1];
      f = ( ( y - z ) * ( y + z ) + ( g - h ) * ( g + h ) ) / ( 2.0 * h * y );

      g = r8_hypot ( f, 1.0 );

      if ( f < 0.0 )
      {
        temp = f - g;
      }
      else
      {
        temp = f + g;
      }

      f = ( ( x - z ) * ( x + z ) + h * ( y / temp - h ) ) / x;
//
//  Next QR transformation.
//
      c = 1.0;
      s = 1.0;

      for ( i = l + 1; i <= k; i++ )
      {
        g = e[i-1];
        y = q[i-1];
        h = s * g;
        g = g * c;

        z = r8_hypot ( f, h );

        e[i-2] = z;

        if ( z == 0.0 )
        {
          f = 1.0;
          z = 1.0;
        }

        c = f / z;
        s = h / z;
        f =   x * c + g * s;
        g = - x * s + g * c;
        h = y * s;
        y = y * c;

        for ( j = 1; j <= n; j++ )
        {
          x = a[j-1+(i-2)*n];
          z = a[j-1+(i-1)*n];
          a[j-1+(i-2)*n] =   x * c + z * s;
          a[j-1+(i-1)*n] = - x * s + z * c;
        }

        z = r8_hypot ( f, h );

        q[i-2] = z;

        if ( z == 0.0 )
        {
          f = 1.0;
          z = 1.0;
        }

        c = f / z;
        s = h / z;
        f =   c * g + s * y;
        x = - s * g + c * y;
      }

      e[l-1] = 0.0;
      e[k-1] = f;
      q[k-1] = x;
    }
  }

  delete [] e;

  return;
}
//****************************************************************************80

void minny ( int n, int jsearch, int nits, double &d2, double &x1, double &f1, 
  bool fk, double f ( double x[], int n ), double x[], double t, double h, 
  double v[], double q0[], double q1[], int &nl, int &nf, double dmin, 
  double ldt, double &fx, double &qa, double &qb, double &qc, double &qd0, 
  double &qd1 )

//****************************************************************************80
//
//  Purpose:
//
//    MINNY minimizes a scalar function of N variables along a line.
//
//  Discussion:
//
//    MINNY minimizes F along the line from X in the direction V(*,JSEARCH) 
//    or else using a quadratic search in the plane defined by Q0, Q1 and X.
//
//    If FK = true, then F1 is FLIN(X1).  Otherwise X1 and F1 are ignored
//    on entry unless final FX is greater than F1.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    04 August 2016
//
//  Author:
//
//    Original FORTRAN77 version by Richard Brent.
//    C++ version by John Burkardt.
//
//  Reference:
//
//    Richard Brent,
//    Algorithms for Minimization with Derivatives,
//    Prentice Hall, 1973,
//    Reprinted by Dover, 2002.
//
//  Parameters:
//
//    Input, int N, the number of variables.
//
//    Input, int JSEARCH, indicates the kind of search.
//    If J is a legal columnindex, linear search in the direction of V(*,JSEARCH).
//    Otherwise, the search is parabolic, based on X, Q0 and Q1.
//
//    Input, int NITS, the maximum number of times the interval 
//    may be halved to retry the calculation.
//
//    Input/output, double &D2, is either zero, or an approximation to 
//    the value of (1/2) times the second derivative of F.
//
//    Input/output, double &X1, on entry, an estimate of the 
//    distance from X to the minimum along V(*,JSEARCH), or a curve.  
//    On output, the distance between X and the minimizer that was found.
//
//    Input/output, double &F1, ?
//
//    Input, bool FK; if FK is TRUE, then on input F1 contains 
//    the value FLIN(X1).
//
//    Input, double F ( double X[], int N ), is the name of the function to 
//    be minimized.
//
//    Input/output, double X[N], ?
//
//    Input, double T, ?
//
//    Input, double H, ?
//
//    Input, double V[N,N], a matrix whose columns are direction
//    vectors along which the function may be minimized.
//
//    ?, double Q0[N], ?
//
//    ?, double Q1[N], ?
//
//    Input/output, int &NL, the number of linear searches.
//
//    Input/output, int &NF, the number of function evaluations.
//
//    Input, double DMIN, an estimate for the smallest eigenvalue.
//
//    Input, double LDT, the length of the step.
//
//    Input/output, double &FX, the value of F(X,N).
//
//    Input/output, double &QA, &QB, &QC;
//
//    Input/output, double &QD0, &QD1, ?.
//
{
  double d1;
  int dz;
  double f0;
  double f2;
  double fm;
  int i;
  int k;
  double m2;
  double m4;
  double machep;
  int ok;
  double s;
  double sf1;
  double small;
  double sx1;
  double t2;
  double temp;
  double x2;
  double xm;

  machep = r8_epsilon ( );
  small = machep * machep;
  m2 = sqrt ( machep );
  m4 = sqrt ( m2 );
  sf1 = f1;
  sx1 = x1;
  k = 0;
  xm = 0.0;
  fm = fx;
  f0 = fx;
  dz = ( d2 < machep );
//
//  Find the step size.
//
  s = r8vec_norm ( n, x );

  if ( dz )
  {
    temp = dmin;
  }
  else
  {
    temp = d2;
  }

  t2 = m4 * sqrt ( fabs ( fx ) / temp + s * ldt ) + m2 * ldt;
  s = m4 * s + t;
  if ( dz && s < t2 )
  {
    t2 = s;
  }

  t2 = r8_max ( t2, small );
  t2 = r8_min ( t2, 0.01 * h );

  if ( fk && f1 <= fm )
  {
    xm = x1;
    fm = f1;
  }

  if ( ( ! fk ) || fabs ( x1 ) < t2 )
  {
    if ( 0.0 <= x1 )
    {
      temp = 1.0;
    }
    else
    {
      temp = - 1.0;
    }

    x1 = temp * t2;
    f1 = flin ( n, jsearch, x1, f, x, nf, v, q0, q1, qd0, qd1, qa, qb, qc );
  }

  if ( f1 <= fm )
  {
    xm = x1;
    fm = f1;
  }
//
//  Evaluate FLIN at another point and estimate the second derivative.
//
  for ( ; ; )
  {
    if ( dz )
    {
      if ( f1 <= f0 )
      {
        x2 = 2.0 * x1;
      }
      else
      {
        x2 = - x1;
      }

      f2 = flin ( n, jsearch, x2, f, x, nf, v, q0, q1, qd0, qd1, qa, qb, qc );

      if ( f2 <= fm )
      {
        xm = x2;
        fm = f2;
      }

      d2 = ( x2 * ( f1 - f0 ) - x1 * ( f2 - f0 ) )
        / ( ( x1 * x2 ) * ( x1 - x2 ) );
    }
//
//  Estimate the first derivative at 0.
//
    d1 = ( f1 - f0 ) / x1 - x1 * d2;
    dz = 1;
//
//  Predict the minimum.
//
    if ( d2 <= small )
    {
      if ( 0.0 <= d1 )
      {
        x2 = - h;
      }
      else
      {
        x2 = h;
      }
    }
    else
    {
      x2 = ( - 0.5 * d1 ) / d2;
    }

    if ( h < fabs ( x2 ) )
    {
      if ( x2 <= 0.0 )
      {
        x2 = - h;
      }
      else
      {
        x2 = h;
      }
    }
//
//  Evaluate F at the predicted minimum.
//
    ok = 1;

    for ( ; ; )
    {
      f2 = flin ( n, jsearch, x2, f, x, nf, v, q0, q1, qd0, qd1, qa, qb, qc );

      if ( nits <= k || f2 <= f0 )
      {
        break;
      }

      k = k + 1;

      if ( f0 < f1 && 0.0 < x1 * x2 )
      {
        ok = 0;
        break;
      }
      x2 = 0.5 * x2;
    }

    if ( ok )
    {
      break;
    }
  }
//
//  Increment the one-dimensional search counter.
//
  nl = nl + 1;

  if ( fm < f2 )
  {
    x2 = xm;
  }
  else
  {
    fm = f2;
  }
//
//  Get a new estimate of the second derivative.
//
  if ( small < fabs ( x2 * ( x2 - x1 ) ) )
  {
    d2 = ( x2 * ( f1 - f0 ) - x1 * ( fm - f0 ) ) 
      / ( ( x1 * x2 ) * ( x1 - x2 ) );
  }
  else
  {
    if ( 0 < k )
    {
      d2 = 0.0;
    }
  }

  d2 = r8_max ( d2, small );

  x1 = x2;
  fx = fm;

  if ( sf1 < fx )
  {
    fx = sf1;
    x1 = sx1;
  }
//
//  Update X for linear search.
//
  if ( 0 <= jsearch )
  {
    for ( i = 0; i < n; i++ )
    {
      x[i] = x[i] + x1 * v[i+jsearch*n];
    }
  }

  return;
}
//****************************************************************************80

double praxis ( double t0, double h0, int n, int prin, double x[], 
  double f ( double x[], int n ) )

//****************************************************************************80
//
//  Purpose:
//
//    PRAXIS seeks an N-dimensional minimizer X of a scalar function F(X).
//
//  Discussion:
//
//    PRAXIS returns the minimum of the function F(X,N) of N variables
//    using the principal axis method.  The gradient of the function is
//    not required.
//
//    The approximating quadratic form is
//
//      Q(x") = F(x,n) + (1/2) * (x"-x)" * A * (x"-x)
//
//    where X is the best estimate of the minimum and 
//
//      A = inverse(V") * D * inverse(V)
//
//    V(*,*) is the matrix of search directions; 
//    D(*) is the array of second differences.  
//
//    If F(X) has continuous second derivatives near X0, then A will tend 
//    to the hessian of F at X0 as X approaches X0.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    04 August 2016
//
//  Author:
//
//    Original FORTRAN77 version by Richard Brent.
//    C++ version by John Burkardt.
//
//  Reference:
//
//    Richard Brent,
//    Algorithms for Minimization with Derivatives,
//    Prentice Hall, 1973,
//    Reprinted by Dover, 2002.
//
//  Parameters:
//
//    Input, double T0, is a tolerance.  PRAXIS attempts to return 
//    praxis = f(x) such that if X0 is the true local minimum near X, then
//    norm ( x - x0 ) < T0 + sqrt ( EPSILON ( X ) ) * norm ( X ),
//    where EPSILON ( X ) is the machine precision for X.
//
//    Input, double H0, is the maximum step size.  H0 should be 
//    set to about the maximum distance from the initial guess to the minimum.
//    If H0 is set too large or too small, the initial rate of
//    convergence may be slow.
//
//    Input, int N, the number of variables.
//
//    Input, int PRIN, controls printing intermediate results.
//    0, nothing is printed.
//    1, F is printed after every n+1 or n+2 linear minimizations.  
//       final X is printed, but intermediate X is printed only 
//       if N is at most 4.
//    2, the scale factors and the principal values of the approximating 
//       quadratic form are also printed.
//    3, X is also printed after every few linear minimizations.
//    4, the principal vectors of the approximating quadratic form are 
//       also printed.
//
//    Input/output, double X[N], is an array containing on entry a
//    guess of the point of minimum, on return the estimated point of minimum.
//
//    Input, double F ( double X[], int N ), is the name of the function to be
//    minimized.
//
//    Output, double PRAXIS, the function value at the minimizer.
//
//  Local parameters:
//
//    Local, double DMIN, an estimate for the smallest eigenvalue.
//
//    Local, double FX, the value of F(X,N).
//
//    Local, bool ILLC, is TRUE if the system is ill-conditioned.
//
//    Local, double LDT, the length of the step.
//
//    Local, int NF, the number of function evaluations.
//
//    Local, int NL, the number of linear searches.
//
{
  double *d;
  double d2;
  double df;
  double dmin;
  double dn;
  double dni;
  double f1;
  bool fk;
  double fx;
  double h;
  int i;
  int ii;
  bool illc;
  int j;
  int jsearch;
  int k;
  int k2;
  int kl;
  int kt;
  int ktm;
  double large;
  double ldfac;
  double lds;
  double ldt;
  double m2;
  double m4;
  double machep;
  int nits;
  int nl;
  int nf;
  double *q0;
  double *q1;
  double qa;
  double qb;
  double qc;
  double qd0;
  double qd1;
  double qf1;
  double r;
  double s;
  double scbd;
  int seed;
  double sf;
  double sl;
  double small;
  double t;
  double temp;
  double t2;
  double *v;
  double value;
  double vlarge;
  double vsmall;
  double *y;
  double *z;
//
//  Allocation.
//
  d = new double[n];
  q0 = new double[n];
  q1 = new double[n];
  v = new double[n*n];
  y = new double[n];
  z = new double[n];
//
//  Initialization.
//
  machep = r8_epsilon ( );
  small = machep * machep;
  vsmall = small * small;
  large = 1.0 / small;
  vlarge = 1.0 / vsmall;
  m2 = sqrt ( machep );
  m4 = sqrt ( m2 );
  seed = 123456789;
//
//  Heuristic numbers:
//
//  If the axes may be badly scaled (which is to be avoided if
//  possible), then set SCBD = 10.  Otherwise set SCBD = 1.
//
//  If the problem is known to be ill-conditioned, initialize ILLC = true.
//
//  KTM is the number of iterations without improvement before the
//  algorithm terminates.  KTM = 4 is very cautious; usually KTM = 1
//  is satisfactory.
//
  scbd = 1.0;
  illc = false;
  ktm = 1;

  if ( illc )
  {
    ldfac = 0.1;
  }
  else
  {
    ldfac = 0.01;
  }

  kt = 0;
  nl = 0;
  nf = 1;
  fx = f ( x, n );
  qf1 = fx;
  t = small + fabs ( t0 );
  t2 = t;
  dmin = small;
  h = h0;
  h = r8_max ( h, 100.0 * t );
  ldt = h;
//
//  The initial set of search directions V is the identity matrix.
//
  for ( j = 0; j < n; j++ )
  {
    for ( i = 0; i < n; i++ )
    {
      v[i+j*n] = 0.0;
    }
    v[j+j*n] = 1.0;
  }

  for ( i = 0; i < n; i++ )
  {
    d[i] = 0.0;
  }
  qa = 0.0;
  qb = 0.0;
  qc = 0.0;
  qd0 = 0.0;
  qd1 = 0.0;
  r8vec_copy ( n, x, q0 );
  r8vec_copy ( n, x, q1 );

  if ( 0 < prin )
  {
    print2 ( n, x, prin, fx, nf, nl );
  }
//
//  The main loop starts here.
//
  for ( ; ; )
  {
    sf = d[0];
    d[0] = 0.0;
//
//  Minimize along the first direction V(*,1).
//
    jsearch = 0;
    nits = 2;
    d2 = d[0];
    s = 0.0;
    value = fx;
    fk = false;

    minny ( n, jsearch, nits, d2, s, value, fk, f, x, t, 
      h, v, q0, q1, nl, nf, dmin, ldt, fx, qa, qb, qc, qd0, qd1 );

    d[0] = d2;

    if ( s <= 0.0 )
    {
      for ( i = 0; i < n; i++ )
      {
        v[i+0*n] = - v[i+0*n];
      }
    }

    if ( sf <= 0.9 * d[0] || d[0] <= 0.9 * sf )
    {
      for ( i = 1; i < n; i++ )
      {
        d[i] = 0.0;
      }
    }
//
//  The inner loop starts here.
//
    for ( k = 2; k <= n; k++ )
    {
      r8vec_copy ( n, x, y );

      sf = fx;

      if ( 0 < kt )
      {
        illc = true;
      }

      for ( ; ; )
      {
        kl = k;
        df = 0.0;
//
//  A random step follows, to avoid resolution valleys.
//
        if ( illc )
        {
          for ( j = 0; j < n; j++ )
          {
            r = r8_uniform_01 ( seed );
            s = ( 0.1 * ldt + t2 * pow ( 10.0, kt ) ) * ( r - 0.5 );
            z[j] = s;
            for ( i = 0; i < n; i++ )
            {
              x[i] = x[i] + s * v[i+j*n];
            }
          }

          fx = f ( x, n );
          nf = nf + 1;
        }
//
//  Minimize along the "non-conjugate" directions V(*,K),...,V(*,N).
//
        for ( k2 = k; k2 <= n; k2++ )
        {
          sl = fx;

          jsearch = k2 - 1;
          nits = 2;
          d2 = d[k2-1];
          s = 0.0;
          value = fx;
          fk = false;

          minny ( n, jsearch, nits, d2, s, value, fk, f, x, t, 
            h, v, q0, q1, nl, nf, dmin, ldt, fx, qa, qb, qc, qd0, qd1 );

          d[k2-1] = d2;

          if ( illc )
          {
            s = d[k2-1] * pow ( s + z[k2-1], 2 );
          }
          else
          {
            s = sl - fx;
          }

          if ( df <= s )
          {
            df = s;
            kl = k2;
          }
        }
//
//  If there was not much improvement on the first try, set
//  ILLC = true and start the inner loop again.
//
        if ( illc )
        {
          break;
        }

        if ( fabs ( 100.0 * machep * fx ) <= df )
        {
          break;
        }
        illc = true;
      }

      if ( k == 2 && 1 < prin )
      {
        r8vec_print ( n, d, "  The second difference array:" );
      }
//
//  Minimize along the "conjugate" directions V(*,1),...,V(*,K-1).
//
      for ( k2 = 1; k2 < k; k2++ )
      {
        jsearch = k2 - 1;
        nits = 2;
        d2 = d[k2-1];
        s = 0.0;
        value = fx;
        fk = false;

        minny ( n, jsearch, nits, d2, s, value, fk, f, x, t, 
          h, v, q0, q1, nl, nf, dmin, ldt, fx, qa, qb, qc, qd0, qd1 );

        d[k2-1] = d2;
      }
 
      f1 = fx;
      fx = sf;

      for ( i = 0; i < n; i++ )
      {
        temp = x[i];
        x[i] = y[i];
        y[i] = temp - y[i];
      }
      
      lds = r8vec_norm ( n, y );
//
//  Discard direction V(*,kl).
//
//  If no random step was taken, V(*,KL) is the "non-conjugate"
//  direction along which the greatest improvement was made.
//
      if ( small < lds )
      {
        for ( j = kl - 1; k <= j; j-- )
        {
          for ( i = 1; i <= n; i++ )
          {
            v[i-1+j*n] = v[i-1+(j-1)*n];
          }
          d[j] = d[j-1];
        }

        d[k-1] = 0.0;

        for ( i = 1; i <= n; i++ )
        {
          v[i-1+(k-1)*n] = y[i-1] / lds;
        }
//
//  Minimize along the new "conjugate" direction V(*,k), which is
//  the normalized vector:  (new x) - (old x).
//
        jsearch = k - 1;
        nits = 4;
        d2 = d[k-1];
        value = f1;
        fk = true;

        minny ( n, jsearch, nits, d2, lds, value, fk, f, x, t, 
          h, v, q0, q1, nl, nf, dmin, ldt, fx, qa, qb, qc, qd0, qd1 );

        d[k-1] = d2;

        if ( lds <= 0.0 )
        {
          lds = - lds;
          for ( i = 1; i <= n; i++ )
          {
            v[i-1+(k-1)*n] = - v[i-1+(k-1)*n];
          }
        }
      }

      ldt = ldfac * ldt;
      ldt = r8_max ( ldt, lds );

      if ( 0 < prin )
      {
        print2 ( n, x, prin, fx, nf, nl );
      }

      t2 = r8vec_norm ( n, x );

      t2 = m2 * t2 + t;
//
//  See whether the length of the step taken since starting the
//  inner loop exceeds half the tolerance.
//
      if ( 0.5 * t2 < ldt )
      {
        kt = - 1;
      }

      kt = kt + 1;

      if ( ktm < kt )
      {
        if ( 0 < prin )
        {
          r8vec_print ( n, x, "  X:" );
        }

        delete [] d;
        delete [] q0;
        delete [] q1;
        delete [] v;
        delete [] y;
        delete [] z;

        return fx;
      }
    }
//
//  The inner loop ends here.
//
//  Try quadratic extrapolation in case we are in a curved valley.
//
    quad ( n, f, x, t, h, v, q0, q1, nl, nf, dmin, ldt, fx, qf1, 
      qa, qb, qc, qd0, qd1 );

    for ( j = 0; j < n; j++ )
    {
      d[j] = 1.0 / sqrt ( d[j] );
    }
 
    dn = r8vec_max ( n, d );

    if ( 3 < prin )
    {
      r8mat_print ( n, n, v, "  The new direction vectors:" );
    }

    for ( j = 0; j < n; j++ )
    {
      for ( i = 0; i < n; i++ )
      {
        v[i+j*n] = ( d[j] / dn ) * v[i+j*n];
      }
    }
//
//  Scale the axes to try to reduce the condition number.
//
    if ( 1.0 < scbd )
    {
      for ( i = 0; i < n; i++ )
      {
        s = 0.0;
        for ( j = 0; j < n; j++ )
        {
          s = s + v[i+j*n] * v[i+j*n];
        }
        s = sqrt ( s );
        z[i] = r8_max ( m4, s );
      }

      s = r8vec_min ( n, z );

      for ( i = 0; i < n; i++ )
      {
        sl = s / z[i];
        z[i] = 1.0 / sl;

        if ( scbd < z[i] )
        {
          sl = 1.0 / scbd;
          z[i] = scbd;
        }
        for ( j = 0; j < n; j++ )
        {
          v[i+j*n] = sl * v[i+j*n];
        }
      }
    }
//
//  Calculate a new set of orthogonal directions before repeating
//  the main loop.
//
//  Transpose V for MINFIT:
//
    r8mat_transpose_in_place ( n, v );
//
//  MINFIT finds the singular value decomposition of V.
//
//  This gives the principal values and principal directions of the
//  approximating quadratic form without squaring the condition number.
//
    minfit ( n, vsmall, v, d );
//
//  Unscale the axes.
//
    if ( 1.0 < scbd )
    {
      for ( i = 0; i < n; i++ )
      {
        for ( j = 0; j < n; j++ )
        {
          v[i+j*n] = z[i] * v[i+j*n];
        }
      }

      for ( j = 0; j < n; j++ )
      {
        s = 0.0;
        for ( i = 0; i < n; i++ )
        {
          s = s + v[i+j*n] * v[i+j*n];
        }
        s = sqrt ( s );

        d[j] = s * d[j];
        for ( i = 0; i < n; i++ )
        {
          v[i+j*n] = v[i+j*n] / s;
        }
      }
    }

    for ( i = 0; i < n; i++ )
    {
      dni = dn * d[i];

      if ( large < dni )
      {
        d[i] = vsmall;
      }
      else if ( dni < small )
      {
        d[i] = vlarge;
      }
      else
      {
        d[i] = 1.0 / dni / dni;
      }
    }
//
//  Sort the eigenvalues and eigenvectors.
//
    svsort ( n, d, v );
//
//  Determine the smallest eigenvalue.
//
    dmin = r8_max ( d[n-1], small );
//
//  The ratio of the smallest to largest eigenvalue determines whether
//  the system is ill conditioned.
//
    if ( dmin < m2 * d[0] )
    {
      illc = true;
    }
    else
    {
      illc = false;
    }

    if ( 1 < prin )
    {
      if ( 1.0 < scbd )
      {
        r8vec_print ( n, z, "  The scale factors:" );
      } 
      r8vec_print ( n, d, "  Principal values of the quadratic form:" );
    }

    if ( 3 < prin )
    {
      r8mat_print ( n, n, v, "  The principal axes:" );
    }
//
//  The main loop ends here.
//
  }

  if ( 0 < prin )
  {
    r8vec_print ( n, x, "  X:" );
  }
//
//  Free memory.
//
  delete [] d;
  delete [] q0;
  delete [] q1;
  delete [] v;
  delete [] y;
  delete [] z;

  return fx;
}
//****************************************************************************80

void print2 ( int n, double x[], int prin, double fx, int nf, int nl )

//****************************************************************************80
//
//  Purpose:
//
//    PRINT2 prints certain data about the progress of the iteration.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    04 August 2016
//
//  Author:
//
//    Original FORTRAN77 version by Richard Brent.
//    C++ version by John Burkardt.
//
//  Reference:
//
//    Richard Brent,
//    Algorithms for Minimization with Derivatives,
//    Prentice Hall, 1973,
//    Reprinted by Dover, 2002.
//
//  Parameters:
//
//    Input, int N, the number of variables.
//
//    Input, double X[N], the current estimate of the minimizer.
//
//    Input, int PRIN, the user-specifed print level.
//    0, nothing is printed.
//    1, F is printed after every n+1 or n+2 linear minimizations.  
//       final X is printed, but intermediate X is printed only 
//       if N is at most 4.
//    2, the scale factors and the principal values of the approximating 
//       quadratic form are also printed.
//    3, X is also printed after every few linear minimizations.
//    4, the principal vectors of the approximating quadratic form are 
//       also printed.
//
//    Input, double FX, the smallest value of F(X) found so far.
//
//    Input, int NF, the number of function evaluations.
//
//    Input, int NL, the number of linear searches.
//
{
  cout << "\n";
  cout << "  Linear searches = " << nl << "\n";
  cout << "  Function evaluations " << nf << "\n";
  cout << "  Function value FX = " << fx << "\n";

  if ( n <= 4 || 2 < prin )
  {
    r8vec_print ( n, x, "  X:" );
  }

  return;
}
//****************************************************************************80

void quad ( int n, double f ( double x[], int n ), double x[], double t, 
  double h, double v[], double q0[], double q1[], int &nl, int &nf, double dmin, 
  double ldt, double &fx, double &qf1, double &qa, double &qb, double &qc, 
  double &qd0, double &qd1 )

//****************************************************************************80
//
//  Purpose:
//
//    QUAD seeks to minimize the scalar function F along a particular curve.
//
//  Discussion:
//
//    The minimizer to be sought is required to lie on a curve defined
//    by Q0, Q1 and X.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    04 August 2016
//
//  Author:
//
//    Original FORTRAN77 version by Richard Brent.
//    C++ version by John Burkardt.
//
//  Reference:
//
//    Richard Brent,
//    Algorithms for Minimization with Derivatives,
//    Prentice Hall, 1973,
//    Reprinted by Dover, 2002.
//
//  Parameters:
//
//    Input, int N, the number of variables.
//
//    Input, double F ( double X[], int N ), the name of the function to 
//    be minimized.
//
//    Input/output, double X[N], ?
//
//    Input, double T, ?
//
//    Input, double H, ?
//
//    Input, double V[N,N], the matrix of search directions.
//
//    Input/output, double Q0[N], Q1[N], two auxiliary points used to define
//    a curve through X.
//
//    Input/output, int &NL, the number of linear searches.
//
//    Input/output, int &NF, the number of function evaluations.
//
//    Input, double DMIN, an estimate for the smallest eigenvalue.
//
//    Input, double LDT, the length of the step.
//
//    Input/output, double &FX, the value of F(X,N).
//
//    Input/output, double &QF1, &QA, &QB, &QC, &QD0, &QD1 ?
//
{
  bool fk;
  int i;
  int jsearch;
  double l;
  int nits;
  double s;
  double temp;
  double value;

  temp = fx;
  fx   = qf1;
  qf1  = temp;

  for ( i = 0; i < n; i++ )
  {
    temp  = x[i];
    x[i]  = q1[i];
    q1[i] = temp;
  }

  qd1 = 0.0;
  for ( i = 0; i < n; i++ )
  {
    qd1 = qd1 + ( x[i] - q1[i] ) * ( x[i] - q1[i] );
  }
  qd1 = sqrt ( qd1 );

  if ( qd0 <= 0.0 || qd1 <= 0.0 || nl < 3 * n * n )
  {
    fx = qf1;
    qa = 0.0;
    qb = 0.0;
    qc = 1.0;
    s = 0.0;
  }
  else
  {
    jsearch = - 1;
    nits = 2;
    s = 0.0;
    l = qd1;
    value = qf1;
    fk = true;

    minny ( n, jsearch, nits, s, l, value, fk, f, x, t, 
      h, v, q0, q1, nl, nf, dmin, ldt, fx, qa, qb, qc, qd0, qd1 );

    qa =                 l * ( l - qd1 )       / ( qd0 + qd1 ) / qd0;
    qb = - ( l + qd0 )     * ( l - qd1 ) / qd1                 / qd0;
    qc =   ( l + qd0 ) * l               / qd1 / ( qd0 + qd1 );
  }

  qd0 = qd1;

  for ( i = 0; i < n; i++ )
  {
    s = q0[i];
    q0[i] = x[i];
    x[i] = qa * s + qb * x[i] + qc * q1[i];
  }

  return;
}
//****************************************************************************80

double r8_epsilon ( )

//****************************************************************************80
//
//  Purpose:
//
//    R8_EPSILON returns the R8 roundoff unit.
//
//  Discussion:
//
//    The roundoff unit is a number R which is a power of 2 with the
//    property that, to the precision of the computer's arithmetic,
//      1 < 1 + R
//    but
//      1 = ( 1 + R / 2 )
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    01 September 2012
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, double R8_EPSILON, the R8 round-off unit.
//
{
  const double value = 2.220446049250313E-016;

  return value;
}
//****************************************************************************80

double r8_hypot ( double x, double y )

//****************************************************************************80
//
//  Purpose:
//
//    R8_HYPOT returns the value of sqrt ( X^2 + Y^2 ).
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    26 March 2012
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, Y, the arguments.
//
//    Output, double R8_HYPOT, the value of sqrt ( X^2 + Y^2 ).
//
{
  double a;
  double b;
  double value;

  if ( fabs ( x ) < fabs ( y ) )
  {
    a = fabs ( y );
    b = fabs ( x );
  }
  else
  {
    a = fabs ( x );
    b = fabs ( y );
  }
//
//  A contains the larger value.
//
  if ( a == 0.0 )
  {
    value = 0.0;
  }
  else
  {
    value = a * sqrt ( 1.0 + ( b / a ) * ( b / a ) );
  }

  return value;
}
//****************************************************************************80

double r8_max ( double x, double y )

//****************************************************************************80
//
//  Purpose:
//
//    R8_MAX returns the maximum of two R8's.
//
//  Discussion:
//
//    The C++ math library provides the function fmax() which is preferred.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    18 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, Y, the quantities to compare.
//
//    Output, double R8_MAX, the maximum of X and Y.
//
{
  double value;

  if ( y < x )
  {
    value = x;
  }
  else
  {
    value = y;
  }
  return value;
}
//****************************************************************************80

double r8_min ( double x, double y )

//****************************************************************************80
//
//  Purpose:
//
//    R8_MIN returns the minimum of two R8's.
//
//  Discussion:
//
//    The C++ math library provides the function fmin() which is preferred.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    31 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, double X, Y, the quantities to compare.
//
//    Output, double R8_MIN, the minimum of X and Y.
//
{
  double value;

  if ( y < x )
  {
    value = y;
  }
  else
  {
    value = x;
  }
  return value;
}
//****************************************************************************80

double r8_uniform_01 ( int &seed )

//****************************************************************************80
//
//  Purpose:
//
//    R8_UNIFORM_01 returns a unit pseudorandom R8.
//
//  Discussion:
//
//    This routine implements the recursion
//
//      seed = ( 16807 * seed ) mod ( 2^31 - 1 )
//      u = seed / ( 2^31 - 1 )
//
//    The integer arithmetic never requires more than 32 bits,
//    including a sign bit.
//
//    If the initial seed is 12345, then the first three computations are
//
//      Input     Output      R8_UNIFORM_01
//      SEED      SEED
//
//         12345   207482415  0.096616
//     207482415  1790989824  0.833995
//    1790989824  2035175616  0.947702
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license. 
//
//  Modified:
//
//    09 April 2012
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Paul Bratley, Bennett Fox, Linus Schrage,
//    A Guide to Simulation,
//    Second Edition,
//    Springer, 1987,
//    ISBN: 0387964673,
//    LC: QA76.9.C65.B73.
//
//    Bennett Fox,
//    Algorithm 647:
//    Implementation and Relative Efficiency of Quasirandom
//    Sequence Generators,
//    ACM Transactions on Mathematical Software,
//    Volume 12, Number 4, December 1986, pages 362-376.
//
//    Pierre L'Ecuyer,
//    Random Number Generation,
//    in Handbook of Simulation,
//    edited by Jerry Banks,
//    Wiley, 1998,
//    ISBN: 0471134031,
//    LC: T57.62.H37.
//
//    Peter Lewis, Allen Goodman, James Miller,
//    A Pseudo-Random Number Generator for the System/360,
//    IBM Systems Journal,
//    Volume 8, Number 2, 1969, pages 136-143.
//
//  Parameters:
//
//    Input/output, int &SEED, the "seed" value.  Normally, this
//    value should not be 0.  On output, SEED has been updated.
//
//    Output, double R8_UNIFORM_01, a new pseudorandom variate, 
//    strictly between 0 and 1.
//
{
  const int i4_huge = 2147483647;
  int k;
  double r;

  if ( seed == 0 )
  {
    cerr << "\n";
    cerr << "R8_UNIFORM_01 - Fatal error!\n";
    cerr << "  Input value of SEED = 0.\n";
    exit ( 1 );
  }

  k = seed / 127773;

  seed = 16807 * ( seed - k * 127773 ) - k * 2836;

  if ( seed < 0 )
  {
    seed = seed + i4_huge;
  }
  r = ( double ) ( seed ) * 4.656612875E-10;

  return r;
}
//****************************************************************************80

void r8mat_print ( int m, int n, double a[], string title )

//****************************************************************************80
//
//  Purpose:
//
//    R8MAT_PRINT prints an R8MAT.
//
//  Discussion:
//
//    An R8MAT is a doubly dimensioned array of R8 values, stored as a vector
//    in column-major order.
//
//    Entry A(I,J) is stored as A[I+J*M]
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    10 September 2009
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int M, the number of rows in A.
//
//    Input, int N, the number of columns in A.
//
//    Input, double A[M*N], the M by N matrix.
//
//    Input, string TITLE, a title.
//
{
  r8mat_print_some ( m, n, a, 1, 1, m, n, title );

  return;
}
//****************************************************************************80

void r8mat_print_some ( int m, int n, double a[], int ilo, int jlo, int ihi,
  int jhi, string title )

//****************************************************************************80
//
//  Purpose:
//
//    R8MAT_PRINT_SOME prints some of an R8MAT.
//
//  Discussion:
//
//    An R8MAT is a doubly dimensioned array of R8 values, stored as a vector
//    in column-major order.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    26 June 2013
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int M, the number of rows of the matrix.
//    M must be positive.
//
//    Input, int N, the number of columns of the matrix.
//    N must be positive.
//
//    Input, double A[M*N], the matrix.
//
//    Input, int ILO, JLO, IHI, JHI, designate the first row and
//    column, and the last row and column to be printed.
//
//    Input, string TITLE, a title.
//
{
# define INCX 5

  int i;
  int i2hi;
  int i2lo;
  int j;
  int j2hi;
  int j2lo;

  cout << "\n";
  cout << title << "\n";

  if ( m <= 0 || n <= 0 )
  {
    cout << "\n";
    cout << "  (None)\n";
    return;
  }
//
//  Print the columns of the matrix, in strips of 5.
//
  for ( j2lo = jlo; j2lo <= jhi; j2lo = j2lo + INCX )
  {
    j2hi = j2lo + INCX - 1;
    if ( n < j2hi )
    {
      j2hi = n;
    }
    if ( jhi < j2hi )
    {
      j2hi = jhi;
    }
    cout << "\n";
//
//  For each column J in the current range...
//
//  Write the header.
//
    cout << "  Col:    ";
    for ( j = j2lo; j <= j2hi; j++ )
    {
      cout << setw(7) << j - 1 << "       ";
    }
    cout << "\n";
    cout << "  Row\n";
    cout << "\n";
//
//  Determine the range of the rows in this strip.
//
    if ( 1 < ilo )
    {
      i2lo = ilo;
    }
    else
    {
      i2lo = 1;
    }
    if ( ihi < m )
    {
      i2hi = ihi;
    }
    else
    {
      i2hi = m;
    }

    for ( i = i2lo; i <= i2hi; i++ )
    {
//
//  Print out (up to) 5 entries in row I, that lie in the current strip.
//
      cout << setw(5) << i - 1 << ": ";
      for ( j = j2lo; j <= j2hi; j++ )
      {
        cout << setw(12) << a[i-1+(j-1)*m] << "  ";
      }
      cout << "\n";
    }
  }

  return;
# undef INCX
}
//****************************************************************************80

void r8mat_transpose_in_place ( int n, double a[] )

//****************************************************************************80
//
//  Purpose:
//
//    R8MAT_TRANSPOSE_IN_PLACE transposes a square R8MAT in place.
//
//  Discussion:
//
//    An R8MAT is a doubly dimensioned array of R8 values, stored as a vector
//    in column-major order.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    26 June 2008
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of rows and columns of the matrix A.
//
//    Input/output, double A[N*N], the matrix to be transposed.
//
{
  int i;
  int j;
  double t;

  for ( j = 0; j < n; j++ )
  {
    for ( i = 0; i < j; i++ )
    {
      t        = a[i+j*n];
      a[i+j*n] = a[j+i*n];
      a[j+i*n] = t;
    }
  }
  return;
}
//****************************************************************************80

void r8vec_copy ( int n, double a1[], double a2[] )

//****************************************************************************80
//
//  Purpose:
//
//    R8VEC_COPY copies an R8VEC.
//
//  Discussion:
//
//    An R8VEC is a vector of R8's.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    03 July 2005
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of entries in the vectors.
//
//    Input, double A1[N], the vector to be copied.
//
//    Output, double A2[N], the copy of A1.
//
{
  int i;

  for ( i = 0; i < n; i++ )
  {
    a2[i] = a1[i];
  }
  return;
}
//****************************************************************************80

double r8vec_max ( int n, double r8vec[] )

//****************************************************************************80
//
//  Purpose:
//
//    R8VEC_MAX returns the value of the maximum element in an R8VEC.
//
//  Discussion:
//
//    An R8VEC is a vector of R8's.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    22 August 2010
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of entries in the array.
//
//    Input, double R8VEC[N], a pointer to the first entry of the array.
//
//    Output, double R8VEC_MAX, the value of the maximum element.  This
//    is set to 0.0 if N <= 0.
//
{
  int i;
  double value;

  value = r8vec[0];

  for ( i = 1; i < n; i++ )
  {
    if ( value < r8vec[i] )
    {
      value = r8vec[i];
    }
  }
  return value;
}
//****************************************************************************80

double r8vec_min ( int n, double r8vec[] )

//****************************************************************************80
//
//  Purpose:
//
//    R8VEC_MIN returns the value of the minimum element in an R8VEC.
//
//  Discussion:
//
//    An R8VEC is a vector of R8's.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    02 July 2005
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of entries in the array.
//
//    Input, double R8VEC[N], the array to be checked.
//
//    Output, double R8VEC_MIN, the value of the minimum element.
//
{
  int i;
  double value;

  value = r8vec[0];

  for ( i = 1; i < n; i++ )
  {
    if ( r8vec[i] < value )
    {
      value = r8vec[i];
    }
  }
  return value;
}
//****************************************************************************80

double r8vec_norm ( int n, double a[] )

//****************************************************************************80
//
//  Purpose:
//
//    R8VEC_NORM returns the L2 norm of an R8VEC.
//
//  Discussion:
//
//    An R8VEC is a vector of R8's.
//
//    The vector L2 norm is defined as:
//
//      R8VEC_NORM = sqrt ( sum ( 1 <= I <= N ) A(I)^2 ).
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    01 March 2003
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of entries in A.
//
//    Input, double A[N], the vector whose L2 norm is desired.
//
//    Output, double R8VEC_NORM, the L2 norm of A.
//
{
  int i;
  double v;

  v = 0.0;

  for ( i = 0; i < n; i++ )
  {
    v = v + a[i] * a[i];
  }
  v = sqrt ( v );

  return v;
}
//****************************************************************************80

void r8vec_print ( int n, double a[], string title )

//****************************************************************************80
//
//  Purpose:
//
//    R8VEC_PRINT prints an R8VEC.
//
//  Discussion:
//
//    An R8VEC is a vector of R8's.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    16 August 2004
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Input, int N, the number of components of the vector.
//
//    Input, double A[N], the vector to be printed.
//
//    Input, string TITLE, a title.
//
{
  int i;

  cout << "\n";
  cout << title << "\n";
  cout << "\n";
  for ( i = 0; i < n; i++ )
  {
    cout << "  " << setw(8)  << i
         << ": " << setw(14) << a[i]  << "\n";
  }

  return;
}
//****************************************************************************80

void svsort ( int n, double d[], double v[] ) 

//****************************************************************************80
//
//  Purpose:
//
//    SVSORT descending sorts D and adjusts the corresponding columns of V.
//
//  Discussion:
//
//    A simple bubble sort is used on D.
//
//    In our application, D contains singular values, and the columns of V are
//    the corresponding right singular vectors.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    04 August 2016
//
//  Author:
//
//    Original FORTRAN77 version by Richard Brent.
//    C++ version by John Burkardt.
//
//  Reference:
//
//    Richard Brent,
//    Algorithms for Minimization with Derivatives,
//    Prentice Hall, 1973,
//    Reprinted by Dover, 2002.
//
//  Parameters:
//
//    Input, int N, the length of D, and the order of V.
//
//    Input/output, double D[N], the vector to be sorted.  
//    On output, the entries of D are in descending order.
//
//    Input/output, double V[N,N], an N by N array to be adjusted 
//    as D is sorted.  In particular, if the value that was in D(I) on input is
//    moved to D(J) on output, then the input column V(*,I) is moved to
//    the output column V(*,J).
//
{
  int i;
  int j1;
  int j2;
  int j3;
  double t;

  for ( j1 = 0; j1 < n - 1; j1++ )
  {
//
//  Find J3, the index of the largest entry in D[J1:N-1].
//  MAXLOC apparently requires its output to be an array.
//
    j3 = j1;
    for ( j2 = j1 + 1; j2 < n; j2++ )
    {
      if ( d[j3] < d[j2] )
      {
        j3 = j2;
      }
    }
//
//  If J1 != J3, swap D[J1] and D[J3], and columns J1 and J3 of V.
//
    if ( j1 != j3 )
    {
      t     = d[j1];
      d[j1] = d[j3];
      d[j3] = t;
      for ( i = 0; i < n; i++ )
      {
        t         = v[i+j1*n];
        v[i+j1*n] = v[i+j3*n];
        v[i+j3*n] = t;
      }
    }
  }

  return;
}


#include "includefirst.hpp"
#include "datatypes.hpp"
#include "envt.hpp"
#include "dinterpreter.hpp"

namespace lib {

  using namespace std;

  class param_for_brent 
  { 
  public: 
    EnvT* envt; 
    EnvUDT* nenvt; 
    string funcname;
    DDoubleGDL* arg;
    bool failed;
    string errmsg; 
  };  
  static param_for_brent brent_func_data;
  static double brent_fake_func(double* x, int n)
  {
    brent_func_data.failed=false;
    brent_func_data.errmsg.clear();
    // use our GDL variable for storage
    for (SizeT i = 0; i < n; ++i) (*(brent_func_data.arg))[i] = x[i];
    // executing our wrapper function with code 0
    BaseGDL* res;
    res = brent_func_data.envt->Interpreter()->call_fun(
      static_cast<DSubUD*> (brent_func_data.nenvt->GetPro())->GetTree()
      );
    Guard<BaseGDL> res_guard(res);
    if (res->N_Elements() != 1) {
      brent_func_data.failed=true;
      brent_func_data.errmsg = "user-defined function \""+brent_func_data.funcname+"\" must return a single non-string value";
      return std::numeric_limits<double>::quiet_NaN(); //ensure not used.
    }
    DDoubleGDL* dres;
    try {
      // BUT: Convert2(...) with CONVERT already deletes 'res' here if the type is changed
      dres = static_cast<DDoubleGDL*>(res->Convert2(GDL_DOUBLE, BaseGDL::CONVERT_THROWIOERROR));
    }    
    catch (GDLIOException& ex) {
     brent_func_data.failed=true;
      brent_func_data.errmsg = "failed to convert the result of your function to double";
      dres=new DDoubleGDL(dimension(n),BaseGDL::ZERO); //we do not return, dres MUST exist!
    }
    if (res != dres) {
      // prevent 'res' from being deleted again
      res_guard.Release();
      res_guard.Init(dres);
    }
    return (*dres)[0];
  }

  //Note: this brent is not from gsl
  void brent(EnvT* e) {
    // sanity check (for number of parameters)
    SizeT nParam = e->NParam();
    if (nParam != 5) e->Throw("Incorrect number of arguments.");
    // 1-st argument : initial guess vector
    BaseGDL* p0 = e->GetParDefined(0);
    BaseGDL* P = p0->Convert2(GDL_DOUBLE, BaseGDL::COPY);
    // 2nd argument : Xi

    // 3nd argument : gtol
    DDouble ftol = 0;
    e->AssureDoubleScalarPar(2,ftol);
    // 4th argument : fmin. will be set on exit.

    //5th argument: function 
    BaseGDL* fun= e->GetParDefined(4);
    DString sfun = StrUpCase((*(DStringGDL*)fun)[0]);
    if (LibFunIx(sfun) != -1) 
      e->Throw("only user-defined functions allowed (library-routine name "+sfun+" given)");
    
    static int DOUBLEIx = e->KeywordIx("DOUBLE");
    bool isDouble=(p0->Type()==GDL_DOUBLE);
    if(e->KeywordSet(DOUBLEIx)) isDouble=true;

    static int ITMAXIx = e->KeywordIx("ITMAX");
    DLong itmax=200;
    if (e->KeywordPresent(ITMAXIx)) e->AssureLongScalarKW(ITMAXIx,itmax);

    // GDL magick
    StackGuard<EnvStackT> guard(e->Interpreter()->CallStack());
    EnvUDT* newEnvfun = new EnvUDT(e->CallingNode(), funList[GDLInterpreter::GetFunIx(sfun)], (DObjGDL**) NULL);
    newEnvfun->SetNextPar(&P); 
    e->Interpreter()->CallStack().push_back(newEnvfun);

    // function parameter initialization
    brent_func_data.envt = e;
    brent_func_data.nenvt = newEnvfun;
    brent_func_data.funcname = sfun;
    brent_func_data.arg = static_cast<DDoubleGDL*> (P);

    double maxstepsize=1;
    int n=P->N_Elements();
    double* x=new double[n];
    for (int i=0; i<n; ++i) x[i]=(*(DDoubleGDL*)P)[i];
    double res = praxis ( ftol, maxstepsize, n, 0, x, brent_fake_func );
    for (int i=0; i<n; ++i) (*(DDoubleGDL*)P)[i]=x[i];
    static int ITERIx = e->KeywordIx("ITER");
    bool returnIter=e->KeywordPresent(ITERIx);
    
    if (returnIter) e->SetKW(ITERIx, new DLongGDL(1));
   
    if (isDouble) {
      e->SetPar(3, new DDoubleGDL(res));
      e->SetPar(0, P);
    }
    else {
      e->SetPar(3, new DDoubleGDL(res));
      Guard<BaseGDL> P_guard(P);
      e->SetPar(0, P->Convert2(GDL_FLOAT, BaseGDL::COPY));
    }

  }
}  
