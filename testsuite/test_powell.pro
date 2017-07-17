function praxis,ftol, h0, n, prin, p, fun
   POWELL, p, identity(n_elements(p)), ftol, fmin, fun   
end

pro r8vec_print, n, x, text 
print
print,text
print
for i=0,n-1 do print,'  ',i,': ',x[i],format='(A2,I8,A2,F)'
end

pro beale_test

;
;
;  Purpose:
;
;    BEALE_TEST calls PRAXIS for the Beale function.
;
;  Licensing:
;
;    This code is distributed under the GNU LGPL license.
;
;  Modified:
;
;    04 August 2016
;
;  Author:
;
;    John Burkardt
;
  n = 2;
  x = [ 0.1, 0.1 ];

  print,"BEALE_TEST";
  print,"  The Beale function."

  t0 = 0.00001;
  h0 = 0.25;

  r8vec_print , n, x, "  Initial point:" 

  print,"  Function value = " , beale_f ( x )

  r8vec_print , n, x, "  Computed minimizer:" 
  
  print,"  Function value = " , beale_f ( x )

end
;**************************************************************************80

function  beale_f, x 

;****************************************************************************80
;
;  Purpose:
;
;    BEALE_F evaluates the Beale function.
;
;  Discussion:
;
;    The function is the sum of the squares of three functions.
;
;    This function has a valley approaching the line X(2) = 1.
;
;  Licensing:
;
;    This code is distributed under the GNU LGPL license.
;
;  Modified:
;
;    04 August 2016
;
;  Author:
;
;    John Burkardt
;
;  Reference:
;
;    E Beale,
;    On an Iterative Method for Finding a Local Minimum of a Function
;    of More than One Variable,
;    Technical Report 25, Statistical Techniques Research Group,
;    Princeton University, 1958.
;
;    Richard Brent,
;    Algorithms for Finding Zeros and Extrema of Functions Without
;    Calculating Derivatives,
;    Stanford University Technical Report STAN-CS-71-198.
;
;  Parameters:
;
;    Input,  X[N], the evaluation point.
;
;    Input, int N, the number of variables.
;
;    Output,  BEALE_F, the function value.
;

 c1 = 1.5;
 c2 = 2.25;
 c3 = 2.625;
  fx1 = c1 - x[0] * ( 1.0 -  x[1]    );
  fx2 = c2 - x[0] * ( 1.0 -  x[1]^2  );
  fx3 = c3 - x[0] * ( 1.0 -  x[1]^3  );

  value = fx1 * fx1 + fx2 * fx2 + fx3 * fx3;

  return, value
end
;****************************************************************************80

pro box_test

;****************************************************************************80
;
;  Purpose:
;
;    BOX_TEST calls PRAXIS for the Box function.
;
;  Licensing:
;
;    This code is distributed under the GNU LGPL license.
;
;  Modified:
;
;    04 August 2016
;
;  Author:
;
;    John Burkardt
;
  n = 3;
  x= [ 0.0, 10.0, 20.0 ];

  print,"BOX_TEST";
  print,"  The Box function.";

  t0 = 0.00001;
  h0 = 20.0;

  r8vec_print , n, x, "  Initial point:" 

  print,"  Function value = " , box_f ( x) 

  pr = praxis ( t0, h0, n, prin, x, "box_f" );

  r8vec_print, n, x, "  Computed minimizer:" 

  print,"  Function value = " , box_f ( x ) 

end
;****************************************************************************80

function box_f,x

;****************************************************************************80
;
;  Purpose:
;
;    BOX_F evaluates the Box function.
;
;  Discussion:
;
;    The function is formed by the sum of squares of 10 separate terms.
;
;  Licensing:
;
;    This code is distributed under the GNU LGPL license.
;
;  Modified:
;
;    04 August 2016
;
;  Author:
;
;    John Burkardt
;
;  Parameters:
;
;    Input,  X[N], the evaluation point.
;
;    Input, int N, the number of variables.
;
;    Output,  BOX_F, the function value.
;
  value = 0.0;

  for i = 1,10 do begin
    c = - ( i ) / 10.0;

    fx = exp ( c * x[0] ) - exp ( c * x[1] ) - x[2] * ( exp ( c ) - exp ( 10.0 * c ) );
   
    value = value + fx * fx;
 endfor
  return, value;
end

;****************************************************************************80

pro chebyquad_test

;****************************************************************************80
;
;  Purpose:
;
;    CHEBYQUAD_TEST calls PRAXIS for the Chebyquad function.
;
;  Licensing:
;
;    This code is distributed under the GNU LGPL license.
;
;  Modified:
;
;    04 August 2016
;
;  Author:
;
;    John Burkardt
;
  n = 8;
  x=dblarr(8);

  print
  print,"CHEBYQUAD_TEST";
  print,"  The Chebyquad function.";

  t0 = 0.00001;
  h0 = 0.1;
  prin = 0;

  for i=0,n-1 do x[i] = ( i + 1. ) /  ( n + 1. );
 
  r8vec_print , n, x, "  Initial point:"

  print,"  Function value = " , chebyquad_f ( x ) 

  pr = praxis ( t0, h0, n, prin, x, "chebyquad_f" )

  r8vec_print , n, x, "  Computed minimizer:" 

  print,"  Function value = " , chebyquad_f ( x )
end
;****************************************************************************80

function chebyquad_f,x

;****************************************************************************80
;
;  Purpose:
;
;    CHEBYQUAD_F evaluates the Chebyquad function.
;
;  Discussion:
;
;    The function is formed by the sum of squares of N separate terms.
;
;  Licensing:
;
;    This code is distributed under the GNU LGPL license.
;
;  Modified:
;
;    04 August 2016
;
;  Author:
;
;    John Burkardt
;
;  Parameters:
;
;    Input,  X[N], the evaluation point.
;
;    Input, int N, the number of variables.
;
;    Output,  CHEBYQUAD_F, the function value.
;
  n = n_elements(x)
  fvec = x*0.0;

  for j = 0L, n-1 do begin
    t1 = 1.0;
    t2 = 2.0 * x[j] - 1.0;
    t = 2.0 * t2;
    
    for i = 0L,n-1 do begin
      fvec[i] = fvec[i] + t2;
      th = t * t2 - t1;
      t1 = t2;
      t2 = th;
    endfor
  endfor

  for i = 0,n-1 do begin
    fvec[i] = fvec[i] / n ;
    if ( ( i mod 2 ) eq 1 ) then  fvec[i] = fvec[i] + 1.0 / ( i * ( i + 2 ) )
  endfor

;
;  Compute F.
;
  value = 0.0;
  for i = 0, n-1 do  value = value + fvec[i] * fvec[i];
  return, value;
end

;****************************************************************************80

pro cube_test

;****************************************************************************80
;
;  Purpose:
;
;    CUBE_TEST calls PRAXIS for the Cube function.
;
;  Licensing:
;
;    This code is distributed under the GNU LGPL license.
;
;  Modified:
;
;    04 August 2016
;
;  Author:
;
;    John Burkardt
;
  n = 2;
  x = [ -1.2, -1.0 ];

  print,"CUBE_TEST";
  print,"  The Cube function.";

  t0 = 0.00001;
  h0 = 1.0;
  prin = 0;

  r8vec_print , n, x, "  Initial point:"

  print,"  Function value = " , cube_f ( x )

  pr = praxis ( t0, h0, n, prin, x, "cube_f" );

  r8vec_print , n, x, "  Computed minimizer:"

  print,"  Function value = " , cube_f ( x)

end

;****************************************************************************80

function cube_f, x

;****************************************************************************80
;
;  Purpose:
;
;    CUBE_F evaluates the Cube function.
;
;  Discussion:
;
;    The function is the sum of the squares of two functions.
;
;  Licensing:
;
;    This code is distributed under the GNU LGPL license.
;
;  Modified:
;
;    04 August 2016
;
;  Author:
;
;    John Burkardt
;
;  Parameters:
;
;    Input,  X[N], the evaluation point.
;
;    Input, int N, the number of variables.
;
;    Output,  CUBE_F, the function value.
;
  fx1 = 10.0 * ( x[1] - x[0] * x[0] * x[0] );
  fx2 = 1.0 - x[0];

  value = fx1 * fx1 + fx2 * fx2;

  return, value;
end
;****************************************************************************80

pro helix_test

;****************************************************************************80
;
;  Purpose:
;
;    HELIX_TEST calls PRAXIS for the Fletcher-Powell Helix function.
;
;  Licensing:
;
;    This code is distributed under the GNU LGPL license.
;
;  Modified:
;
;    04 August 2016
;
;  Author:
;
;    John Burkardt
;
  n = 3;
  x = [ -1.0, 0.0, 0.0 ];

  print,"HELIX_TEST";
  print,"  The Fletcher-Powell Helix function.";

  t0 = 0.00001;
  h0 = 1.0;
  prin = 0;

  r8vec_print , n, x, "  Initial point:"

  print,"  Function value = " , helix_f ( x )

  pr = praxis ( t0, h0, n, prin, x, "helix_f" )

  r8vec_print , n, x, "  Computed minimizer:"

  print,"  Function value = " , helix_f ( x )

end
;****************************************************************************80

function  helix_f, x

;****************************************************************************80
;
;  Purpose:
;
;    HELIX_F evaluates the Helix function.
;
;  Discussion:
;
;    The function is the sum of the squares of three functions.
;
;  Licensing:
;
;    This code is distributed under the GNU LGPL license.
;
;  Modified:
;
;    04 August 2016
;
;  Author:
;
;    John Burkardt
;
;  Parameters:
;
;    Input,  X[N], the evaluation point.
;
;    Input, int N, the number of variables.
;
;    Output,  HELIX_F, the function value.
;
  r8_pi = 3.141592653589793d;
 
  r = sqrt ( x[0] * x[0] + x[1] * x[1] );

  if ( 0.0 le x[0] ) then theta = 0.5 * atan ( x[1], x[0] ) / r8_pi else if ( x[0] < 0.0 ) then  theta = 0.5 * ( atan ( x[1], x[0] ) + r8_pi ) / r8_pi;

  fx1 = 10.0 * ( x[2] - 10.0 * theta );
  fx2 = 10.0 * ( r - 1.0 );
  fx3 = x[2];

  value = fx1 * fx1 + fx2 * fx2 + fx3 * fx3;

  return, value;
end
;****************************************************************************80

pro hilbert_test

;****************************************************************************80
;
;  Purpose:
;
;    HILBERT_TEST calls PRAXIS for the Hilbert function.
;
;  Licensing:
;
;    This code is distributed under the GNU LGPL license.
;
;  Modified:
;
;    04 August 2016
;
;  Author:
;
;    John Burkardt
;
  n = 10;
  x=dblarr(n)

  print,"HILBERT_TEST";
  print,"  The Hilbert function.";

  t0 = 0.00001;
  h0 = 10.0;
  prin = 0;

  for i = 0,n-1 do   x[i] = 1.0;

  r8vec_print , n, x, "  Initial point:" 

  print,"  Function value = " , hilbert_f ( x )

  pr = praxis ( t0, h0, n, prin, x, "hilbert_f" );

  r8vec_print , n, x, "  Computed minimizer:" 

  print,"  Function value = " , hilbert_f ( x )
end

;****************************************************************************80

function  hilbert_f,x

;****************************************************************************80
;
;  Purpose:
;
;    HILBERT_F evaluates the Hilbert function.
;
;  Discussion:
;
;    The function is a positive definite quadratic  of the form
;
;      f(x) = x" A x
;
;    where A is the Hilbert matrix, A(I,J) = 1/(I+J-1).
;
;  Licensing:
;
;    This code is distributed under the GNU LGPL license.
;
;  Modified:
;
;    04 August 2016
;
;  Author:
;
;    John Burkardt
;
;  Parameters:
;
;    Input,  X[N], the evaluation point.
;
;    Input, int N, the number of variables.
;
;    Output,  HILBERT_F, the function value.
;

  value = 0.0;
  n=n_elements(x)
  for i = 0, n-1 do for j = 0,n-1 do  value = value + x[i] * x[j] / ( i + j + 1. );

  return, value;
end
;****************************************************************************80

pro powell3d_test

;****************************************************************************80
;
;  Purpose:
;
;    POWELL3D_TEST calls PRAXIS for the Powell 3D function.
;
;  Licensing:
;
;    This code is distributed under the GNU LGPL license.
;
;  Modified:
;
;    04 August 2016
;
;  Author:
;
;    John Burkardt
;
  n = 3;
  x = [ 0.0, 1.0, 2.0 ];

  print,"POWELL3D_TEST";
  print,"  The Powell 3D function.";

  t0 = 0.00001;
  h0 = 1.0;
  prin = 0;

  r8vec_print , n, x, "  Initial point:"

  print,"  Function value = " , powell3d_f ( x )

  pr = praxis ( t0, h0, n, prin, x, "powell3d_f" );

  r8vec_print , n, x, "  Computed minimizer:" 

  print,"  Function value = " , powell3d_f ( x )
end
;****************************************************************************80

 function powell3d_f,  x 

;****************************************************************************80
;
;  Purpose:
;
;    POWELL3D_F evaluates the Powell 3D function.
;
;  Licensing:
;
;    This code is distributed under the GNU LGPL license.
;
;  Modified:
;
;    04 August 2016
;
;  Author:
;
;    John Burkardt
;
;  Reference:
;
;    M J D Powell,
;    An Efficient Method for Finding the Minimum of a  of
;    Several Variables Without Calculating Derivatives,
;    Computer Journal, 
;    Volume 7, Number 2, pages 155-162, 1964.
;
;  Parameters:
;
;    Input,  X[N], the evaluation point.
;
;    Input, int N, the number of variables.
;
;    Output,  POWELL3D_F, the function value.
;
  r8_pi = 3.141592653589793d;
  value = 3.0 - 1.0 / ( 1.0 + (x[0] - x[1])^2  ) $ 
    - sin ( 0.5 * r8_pi * x[1] * x[2] ) $
    - exp ( - (( x[0] - 2.0 * x[1] + x[2] ) / x[1])^2 )

  return, value;
end
;****************************************************************************80

pro rosenbrock_test

;****************************************************************************80
;
;  Purpose:
;
;    ROSENBROCK_TEST calls PRAXIS for the Rosenbrock function.
;
;  Licensing:
;
;    This code is distributed under the GNU LGPL license.
;
;  Modified:
;
;    04 August 2016
;
;  Author:
;
;    John Burkardt
;
  n = 2;
  x = [ -1.2, 1.0 ];

  print,"ROSENBROCK_TEST";
  print,"  The Rosenbrock function.";

  t0 = 0.00001;
  h0 = 1.0;
  prin = 0;

  r8vec_print , n, x, "  Initial point:"

  print,"  Function value = " , rosenbrock_f ( x )

  pr = praxis ( t0, h0, n, prin, x, "rosenbrock_f" );

  r8vec_print , n, x, "  Computed minimizer:" 

  print,"  Function value = " , rosenbrock_f ( x )
end
;****************************************************************************80
function Rosenbrock_f, x
; Rosenbrock: f  = (1 - x1)^2+ 100*(x2 - x1^2)^2 
  u1 = 1 - x[0]
  v1 = x[1] - x[0]^2
  f = u1*u1 + 100.0*v1*v1
RETURN, f
end

;****************************************************************************80

pro singular_test

;****************************************************************************80
;
;  Purpose:
;
;    SINGULAR_TEST calls PRAXIS for the Powell Singular function.
;
;  Licensing:
;
;    This code is distributed under the GNU LGPL license.
;
;  Modified:
;
;    04 August 2016
;
;  Author:
;
;    John Burkardt
;
  n = 4;
  x = [ 3.0, -1.0, 0.0, 1.0 ];

  print,"SINGULAR_TEST";
  print,"  The Powell Singular function.";

  t0 = 0.00001;
  h0 = 1.0;
  prin = 0;

  r8vec_print , n, x, "  Initial point:"

  print,"  Function value = " , singular_f ( x )

  pr = praxis ( t0, h0, n, prin, x, "singular_f" );

  r8vec_print , n, x, "  Computed minimizer:" 

  print,"  Function value = " , singular_f ( x )
end
;****************************************************************************80

function singular_f,x

;****************************************************************************80
;
;  Purpose:
;
;    SINGULAR_F evaluates the Powell Singular function.
;
;  Licensing:
;
;    This code is distributed under the GNU LGPL license.
;
;  Modified:
;
;    04 August 2016
;
;  Author:
;
;    John Burkardt
;
;  Parameters:
;
;    Input,  X[N], the evaluation point.
;
;    Input, int N, the number of variables.
;
;    Output,  SINGULAR_F, the function value.
;
  value = 0.0;
  n=n_elements(x)

  for j = 1,n-1,4 do  BEGIN
    if ( j + 1 le n ) then xjp1 = x[j] else xjp1 = 0.0;
    if ( j + 2 le n ) then xjp2 = x[j+1] else xjp2 = 0.0;
    if ( j + 3 le n ) then xjp3 = x[j+2] else xjp3 = 0.0;

    f1 = x[j-1] + 10.0 * xjp1;

    if ( j + 1 le n ) then f2 = xjp2 - xjp3 else f2 = 0.0;

    if ( j + 2 le n ) then f3 = xjp1 - 2.0 * xjp2 else  f3 = 0.0;

    if ( j + 3 le n ) then f4 = x[j-1] - xjp3 else  f4 = 0.0;
    value = value + f1^2 +  5.0 * f2^2  + f3^4 + 10.0 * f4^4

 endfor

  return, value;
end
;****************************************************************************80

pro tridiagonal_test

;****************************************************************************80
;
;  Purpose:
;
;    TRIDIAGONAL_TEST calls PRAXIS for the Tridiagonal function.
;
;  Licensing:
;
;    This code is distributed under the GNU LGPL license.
;
;  Modified:
;
;    04 August 2016
;
;  Author:
;
;    John Burkardt
;
   n=4
   x=dblarr(4);

  print,"TRIDIAGONAL_TEST";
  print,"  The Tridiagonal function.";

  t0 = 0.00001;
  h0 = 8.0;
  prin = 0;

  r8vec_print , n, x, "  Initial point:"

  print,"  Function value = " , tridiagonal_f ( x )

  pr = praxis ( t0, h0, n, prin, x, "tridiagonal_f" );

  r8vec_print , n, x, "  Computed minimizer:" 

  print,"  Function value = " , tridiagonal_f ( x )
end
;****************************************************************************80

function  tridiagonal_f,x

;****************************************************************************80
;
;  Purpose:
;
;    TRIDIAGONAL_F evaluates the tridiagonal function.
;
;  Licensing:
;
;    This code is distributed under the GNU LGPL license.
;
;  Modified:
;
;    04 August 2016
;
;  Author:
;
;    John Burkardt
;
;  Parameters:
;
;    Input,  X[N], the evaluation point.
;
;    Input, int N, the number of variables.
;
;    Output,  TRIDIAGONAL_F, the function value.
;
  n=n_elements(x)
  value = x[0] * x[0];
  for i = 1,n-1 do  value = value + 2.0 * x[i] * x[i];
  for i = 0,n-2 do  value = value - 2.0 * x[i] * x[i+1];
  value = value - 2.0 * x[0];
  return, value;
end
;****************************************************************************80

pro watson_test

;****************************************************************************80
;
;  Purpose:
;
;    WATSON_TEST calls PRAXIS for the Watson function.
;
;  Licensing:
;
;    This code is distributed under the GNU LGPL license.
;
;  Modified:
;
;    30 July 2016
;
;  Author:
;
;    John Burkardt
;
  n = 6;
  x=dblarr(n)

  print,"WATSON_TEST";
  print,"  The Watson function.";

  t0 = 0.00001;
  h0 = 1.0;
  prin = 0;

  r8vec_print , n, x, "  Initial point:" 

  print,"  Function value = " , watson_f ( x)

  pr = praxis ( t0, h0, n, prin, x, "watson_f" );

  r8vec_print , n, x, "  Computed minimizer:" 

  print,"  Function value = " , watson_f ( x)
end
;****************************************************************************80

function watson_f,x

;****************************************************************************80
;
;  Purpose:
;
;    WATSON_F evaluates the Watson function.
;
;  Licensing:
;
;    This code is distributed under the GNU LGPL license.
;
;  Modified:
;
;    30 July 2016
;
;  Author:
;
;    John Burkardt
;
;  Parameters:
;
;    Input,  X[N], the evaluation point.
;
;    Input, int N, the number of variables.
;
;    Output,  WATSON_F, the function value.
;
  value = 0.0
  n=n_elements(x)

  for i = 1,29 do begin
    s1 = 0.0;
    d = 1.0;
    for j = 1,n-1 do begin
      s1 = s1 + j * d * x[j];
      d = d * i / 29.0;
   endfor
    s2 = 0.0;
    d = 1.0;
    for j = 0,n-1 do begin
      s2 = s2 + d * x[j];
      d = d * i / 29.0;
   endfor
    value = value + ( s1 - s2 * s2 - 1.0)^2
  endfor
  value = value + x[0] * x[0] + ( x[1] - x[0] * x[0] - 1.0)^ 2

  return, value;
end
 ;****************************************************************************80

pro wood_test

;****************************************************************************80
;
;  Purpose:
;
;    WOOD_TEST calls PRAXIS for the Wood function.
;
;  Licensing:
;
;    This code is distributed under the GNU LGPL license.
;
;  Modified:
;
;    30 July 2016
;
;  Author:
;
;    John Burkardt
;
  n = 4;
  x = [ -3.0, -1.0, -3.0, -1.0 ];

  print,"WOOD_TEST";
  print,"  The Wood function.";

  t0 = 0.00001;
  h0 = 10.0;
  prin = 0;

  r8vec_print , n, x, "  Initial point:" 

  print,"  Function value = " , wood_f ( x )

  pr = praxis ( t0, h0, n, prin, x, "wood_f" );

  r8vec_print , n, x, "  Computed minimizer:" 

  print,"  Function value = " , wood_f ( x )
end

;****************************************************************************80

function wood_f,x

;****************************************************************************80
;
;  Purpose:
;
;    WOOD_F evaluates the Wood function.
;
;  Licensing:
;
;    This code is distributed under the GNU LGPL license.
;
;  Modified:
;
;    30 July 2016
;
;  Author:
;
;    John Burkardt
;
;  Parameters:
;
;    Input,  X[N], the evaluation point.
;
;    Input, int N, the number of variables.
;
;    Output,  WOOD_F, the function value.
;

  f1 = x[1] - x[0] * x[0];
  f2 = 1.0 - x[0];
  f3 = x[3] - x[2] * x[2];
  f4 = 1.0 - x[2];
  f5 = x[1] + x[3] - 2.0;
  f6 = x[1] - x[3];

  value =  100.0 * f1 * f1  +  f2 * f2    +  90.0 * f3 * f3    +  f4 * f4    +  10.0 * f5 * f5    +   0.1 * f6 * f6

  return, value;
end

PRO TEST_POWELL
  beale_test
  box_test
  chebyquad_test
  cube_test
  helix_test
  hilbert_test
  powell3d_test
  rosenbrock_test
  singular_test
  tridiagonal_test
  watson_test
  wood_test
end
