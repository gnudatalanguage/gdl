/*

The origin of this code is unclear.
It seems to have been written by
my student Ilya in July 2015 but maybe
it just come from another tuto or on the internet
or derive from another code.

I, Alain C., put it under GNU GPL V3 or later,
but if any problem, just write to me !

*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// As Gilles mentionned, we don't need a "idl_export.h"
// or a "mg_idl_export.h" (for OSX) to use that
//#include "idl_export.h"

typedef int IDL_INT;
typedef long IDL_LONG;
typedef float IDL_FLOAT;

IDL_INT add_int (int argc, void *argv[])
{
  IDL_INT a;
  IDL_INT b;
  IDL_INT retval;

  a=*((IDL_INT *)argv[0]);
  b=*((IDL_INT *)argv[1]);

  retval=a+b;
  return retval;
}

IDL_LONG add_long (int argc, void *argv[])
{
  IDL_LONG a;
  IDL_LONG b;
  IDL_LONG retval;

  a=*((IDL_LONG *)argv[0]);
  b=*((IDL_LONG *)argv[1]);

  retval=a+b;
  return retval;
}

IDL_FLOAT add_float (int argc, void *argv[])
{
  IDL_FLOAT a;
  IDL_FLOAT b;
  IDL_FLOAT retval;

  a=*((IDL_FLOAT *)argv[0]);
  b=*((IDL_FLOAT *)argv[1]);

  //  printf << a << endl;
  //  printf("ici %f", a);

  retval=a+b;
  return retval;
}

/*
void main()
{
  int a;
  long b;
  float c;
  a=add_int((int)5,(int)6);
  b=add_long((long)5,(long)6);
  c=add_float(5,6);
  c=add_float(5.5,6.6);

}

*/
