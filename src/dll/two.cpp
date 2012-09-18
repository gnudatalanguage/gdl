#include "envt.hpp"

using namespace std;

template< typename T>
BaseGDL* two_fun_template( BaseGDL* p0)
{
  T* p0C = static_cast<T*>( p0);
  T* res = new T( p0C->Dim(), BaseGDL::NOZERO);
  SizeT nEl = p0->N_Elements();
  for( SizeT i=0; i<nEl; ++i)
    {
      (*res)[ i] = 2 * ((*p0C)[ i]);
    }
  return res;
}

extern "C" BaseGDL* two_fun( EnvT* e)
{

  SizeT nParam=e->NParam();
  if (nParam != 1) {
    cout << "TWO: Improper Number of Variables" << endl;
    return new DLongGDL( -1);
  }

  BaseGDL* p0 = e->GetPar( 0);//, "TWO");

  if( p0->Type() == GDL_DOUBLE)
    return two_fun_template< DDoubleGDL>( p0);
  else if( p0->Type() == GDL_FLOAT)
    return two_fun_template< DFloatGDL>( p0);
  else 
    {
      return new DLongGDL( -1);
    }
}

