typedef BaseGDL *GDL_VPTR;
typedef BaseGDL GDL_VARIABLE;

#define GDL_BARR_INI_ZERO BaseGDL::ZERO
#define GDL_BARR_INI_NOP  BaseGDL::NOZERO
#define GDL_Deltmp delete

template< typename T1, typename T2>
T2* GDL_GetScratch(GDL_VPTR *p, DLong nElem)
{
  BaseGDL* data = new T1( dimension( nElem), GDL_BARR_INI_ZERO);
  *p = data;
  return (T2 *) data->DataAddr();
}

template< typename T1, typename T2>
T2* GDL_MakeTempArray( DLong rank, DLong *dim, BaseGDL::InitType iT, 
		       GDL_VPTR *v)
{
  BaseGDL* data = new T1( dimension(dim, rank), iT);
  *v = data;
  return (T2 *) data->DataAddr();
}

void GDL_VarCopy( EnvT* e, DLong ipar, GDL_VPTR v_src)
{
  BaseGDL** p = &e->GetPar( ipar);
  *p = v_src;
}
