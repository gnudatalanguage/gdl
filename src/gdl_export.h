#ifndef export_GDL_DEF
#define export_GDL_DEF
#include "idl_export.h"
#ifdef MSWIN
#define DLLEXPORT __declspec( dllexport )
#else
#define DLLEXPORT
#endif
#endif 
