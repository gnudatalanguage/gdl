/***************************************************************************
                          ncdf_cl.hpp  -  NetCDF GDL library functions
                             -------------------
    begin                : March 14 2004
    copyright            : (C) 2004 by Christopher Lee
    email                : leec_gdl@publius.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NCDF_HPP_CL_
#define NCDF_HPP_CL_

#include <netcdfcpp.h>
#include <string>

namespace lib {

  using namespace std;
  using namespace antlr;


  void ncdf_handle_error(EnvT* e, int status, const char * function);

  template <typename T>void ncdf_handle_error(EnvT* e, int status, 
					      const char * function, T* data);

  DStringGDL ncdf_gdl_typename(nc_type type);

  void negzero_message(const char * name, int index, int set);

  void exceed_message(const char * name, int index, int set);

  string _ncdf_itos(int i);


  BaseGDL* ncdf_open(EnvT* e);/*IDL seems to segfault
				after ~40 files,GDL does
				1024 with no segfault :)*/

  BaseGDL* ncdf_create(EnvT* e);
  void ncdf_close(EnvT* e);
  BaseGDL* ncdf_inquire(EnvT* e);
  void ncdf_control(EnvT* e);




  //------------------

  void ncdf_diminq(EnvT* e);
  void ncdf_dimrename(EnvT* e);

  BaseGDL* ncdf_dimid(EnvT* e);
  BaseGDL* ncdf_dimdef(EnvT* e);
  //---------------------------


  void ncdf_varget1(EnvT* e);
  void ncdf_varget(EnvT* e);
  void ncdf_varrename(EnvT* e);
  void ncdf_varput(EnvT* e);

  BaseGDL* ncdf_varinq(EnvT* e);
  BaseGDL* ncdf_varid(EnvT* e);
  BaseGDL* ncdf_vardef(EnvT* e);

  template <typename T> void ncdf_var_handle_error(EnvT *e, int status,
						   const char *function,
						   T* data);



  //-----------------

  void ncdf_attput(EnvT* e);
  void ncdf_attget(EnvT* e);
  void ncdf_attdel(EnvT* e);
  void ncdf_attrename(EnvT* e);
  
  BaseGDL* ncdf_attinq(EnvT* e);
  BaseGDL* ncdf_attname(EnvT* e);
  BaseGDL* ncdf_attcopy(EnvT* e);


  template <typename T> void ncdf_att_handle_error(EnvT *e, int status, 
						   const char *function,
						   T* data);
  //----------------

} // namespace

#endif
