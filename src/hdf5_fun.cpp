/***************************************************************************
                          hdf5_fun.cpp  -  HDF5 GDL library function
                             -------------------
    begin                : Aug 02 2004
    copyright            : (C) 2004 by Peter Messmer
    email                : messmer@users.sourceforge.net
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// the following stuff needs some cleanup in order to make it nicely fit
// into the distribution.

#ifdef HAVE_CONFIG_H
#include <config.h>
#else
// default: assume we have HDF5
#define USE_HDF5 1
#endif

#ifdef USE_HDF5

#include "includefirst.hpp"

#include "hdf5_fun.hpp"

#if defined(H5_USE_16_API) && defined(H5_NO_DEPRECATED_SYMBOLS)
#error "Can't choose old API versions when deprecated APIs are disabled"
#endif /* defined(H5_USE_16_API) && defined(H5_NO_DEPRECATED_SYMBOLS) */

#include <list>

namespace lib {

  using namespace std;

  // SA: error-handling-realted helper routines / classes
  // --------------------------------------------------------------------

  // helper routine for hdf5_error_message()
  herr_t hdf5_error_message_helper(int n, H5E_error_t *err_desc, void *msg)
  {
    // getting something better than "Inappropriate type" message
#if (H5_VERS_MAJOR < 1) || ((H5_VERS_MAJOR == 1) && (H5_VERS_MINOR <= 6))
    if (err_desc->min_num == H5E_BADTYPE)
      *static_cast<string*>(msg) = H5Eget_major(err_desc->maj_num);
    else
      *static_cast<string*>(msg) = H5Eget_minor(err_desc->min_num);
#else
    char* tmp;
    if (err_desc->min_num == H5E_BADTYPE)
      tmp = H5Eget_major(err_desc->maj_num);
    else
      tmp = H5Eget_minor(err_desc->min_num);
    *static_cast<string*>(msg) = tmp;
    free(tmp);
#endif
    return 0;
  }

  // returns a meaningful message describing last HDF5 error
  // usual usege:
  //   if (H5X_xxx(...) < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
  string hdf5_error_message(string &msg)
  {
    H5Ewalk(H5E_WALK_UPWARD, hdf5_error_message_helper, &msg);
    return msg;
  }

  // auto_ptr-like class for guarding HDF5 spaces
  // usage:
  //   hid_t h5s_id = H5Dget_space(h5d_id);
  //   if (h5s_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
  //   hdf5_space_guard h5s_id_guard = hdf5_space_guard(h5s_id);
  class hdf5_space_guard
  {
    hid_t space;
  public:
    hdf5_space_guard(hid_t space_) { space = space_; }
    ~hdf5_space_guard() { if(space) H5Sclose(space); }
  };

  // auto_ptr-like class for guarding HDF5 types
  // usage:
  //   hid_t datatype = H5Dget_type(h5d_id);
  //   if (datatype < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
  //   hdf5_type_guard datatype_guard = hdf5_type_guard(datatype);
  class hdf5_type_guard
  {
    hid_t type;
  public:
    hdf5_type_guard(hid_t type_) { type = type_; }
    ~hdf5_type_guard() { H5Tclose(type); }
  };

  // auto_ptr-like class for guarding HDF5 names
  // usage:
  //   char* member_name = H5Tget_member_name( ... );
  //   if (!member_name) { string msg; e->Throw(hdf5_error_message(msg)); }
  //   hdf5_name_guard member_name_guard = hdf5_name_guard(member_name);
  class hdf5_name_guard
  {
    char* name;
  public:
    hdf5_name_guard(char* name_) { name = name_; }
    ~hdf5_name_guard() { H5free_memory(name); }
  };

  // auto_ptr-like class for guarding HDF5 property lists
  // usage:
  //   hid_t h5p_id = H5Pcreate(...);
  //   hdf5_plist_guard h5p_id_guard = hdf5_plist_guard(h5p_id);
  class hdf5_plist_guard
  {
    hid_t plist;
  public:
    hdf5_plist_guard(hid_t plist_) { plist = plist_; }
    ~hdf5_plist_guard() { if(plist) H5Pclose(plist); }
  };

  // --------------------------------------------------------------------

  DLong mapH5DatatypesToGDL(hid_t h5type, EnvT *e){

     /* Nov 2021, Oliver Gressel <ogressel@gmail.com>
        - refactor using lists
        - add check for invalid type handles
        - test for GDL_STRING/GDL_STRUCT via H5Tget_class(h5type)
     */

    bool debug=false;

    const int matching_gdl_type[] =
        { //GDL_LDOUBLE,   /// see below
          GDL_DOUBLE, GDL_FLOAT, GDL_ULONG64, GDL_LONG64, GDL_ULONG, GDL_LONG,
          GDL_UINT, GDL_INT, GDL_BYTE, GDL_STRING };

    const char *matching_gdl_type_lbl[] =
        { //GDL_LDOUBLE,   /// see below
          "GDL_DOUBLE", "GDL_FLOAT", "GDL_ULONG64", "GDL_LONG64", "GDL_ULONG",
          "GDL_LONG", "GDL_UINT", "GDL_INT", "GDL_BYTE", "GDL_STRING" };

    //must be in order, from most complicated to simplest, string at end

    const std::list <std::list <hid_t>> type_map = {

          /* GDL_LDOUBLE */
     // { H5T_NATIVE_LDOUBLE }, //not until LDOUBLE Is handled everywhere!!

          /* GDL_DOUBLE */
        { H5T_NATIVE_DOUBLE,H5T_IEEE_F64BE,H5T_IEEE_F64LE,H5T_INTEL_F64,H5T_MIPS_F64,H5T_ALPHA_F64},

          /* GDL_FLOAT */
        { H5T_NATIVE_FLOAT,H5T_ALPHA_F32,H5T_IEEE_F32BE, H5T_IEEE_F32LE,H5T_INTEL_F32,H5T_MIPS_F32},

          /* GDL_ULONG64 */
        { H5T_NATIVE_ULLONG, H5T_ALPHA_U64, H5T_INTEL_U64, H5T_MIPS_U64,
          H5T_NATIVE_UINT64, H5T_NATIVE_UINT_FAST64, H5T_NATIVE_UINT_LEAST64,
          H5T_STD_U64BE, H5T_STD_U64LE },

          /* GDL_LONG64 */
        { H5T_NATIVE_LLONG, H5T_INTEL_B64,
          H5T_INTEL_I64, H5T_MIPS_B64,
          H5T_MIPS_I64, H5T_NATIVE_B64, H5T_NATIVE_INT64, H5T_NATIVE_INT_FAST64,
          H5T_NATIVE_INT_LEAST64, H5T_STD_B64BE, H5T_STD_B64LE, H5T_STD_I64BE,
          H5T_STD_I64LE, H5T_UNIX_D64BE, H5T_UNIX_D64LE, H5T_ALPHA_B64,
          H5T_ALPHA_I64 },

          /* GDL_ULONG */
        { H5T_NATIVE_ULONG, H5T_ALPHA_U32, H5T_INTEL_U32, H5T_MIPS_U32,
          H5T_NATIVE_UINT32, H5T_NATIVE_UINT_FAST32, H5T_NATIVE_UINT_LEAST32,
          H5T_STD_U32BE, H5T_STD_U32LE },

          /* GDL_LONG */
        { /// H5T_NATIVE_HBOOL,
          /// ^--- disabled as it matches against 'H5T_STD_U8LE' (GDL_BYTE)
          H5T_NATIVE_LONG, H5T_ALPHA_B32, 
          H5T_ALPHA_I32, H5T_INTEL_B32,
          H5T_INTEL_I32, H5T_MIPS_B32,
          H5T_MIPS_I32, H5T_NATIVE_B32, H5T_NATIVE_INT32, H5T_NATIVE_INT_FAST32,
          H5T_NATIVE_INT_LEAST32, H5T_STD_B32BE, H5T_STD_B32LE, H5T_STD_I32BE,
          H5T_STD_I32LE, H5T_UNIX_D32BE, H5T_UNIX_D32LE },

          /* GDL_UINT */
        { H5T_NATIVE_UINT, H5T_NATIVE_UINT16, H5T_NATIVE_UINT_FAST16,
          H5T_NATIVE_UINT_LEAST16, H5T_STD_U16BE, H5T_STD_U16LE, H5T_ALPHA_U16,
          H5T_INTEL_U16, H5T_MIPS_U16 },

          /* GDL_INT */
        { H5T_NATIVE_INT, H5T_NATIVE_INT16, H5T_NATIVE_INT_FAST16,
          H5T_NATIVE_INT_LEAST16, H5T_STD_B16BE, H5T_STD_B16LE, H5T_STD_I16BE,
          H5T_STD_I16LE, H5T_ALPHA_B16, H5T_ALPHA_I16, H5T_INTEL_B16,
          H5T_INTEL_I16, H5T_MIPS_B16, H5T_MIPS_I16, H5T_NATIVE_B16 },

          /* GDL_BYTE */
        { H5T_ALPHA_U8, H5T_MIPS_U8, H5T_INTEL_U8, H5T_NATIVE_UINT8,
          H5T_NATIVE_UINT_FAST8, H5T_NATIVE_UINT_LEAST8, H5T_STD_U8BE,
          H5T_STD_U8LE, H5T_NATIVE_USHORT, H5T_NATIVE_INT8, H5T_ALPHA_B8,
          H5T_ALPHA_I8, H5T_INTEL_B8, H5T_INTEL_I8, H5T_MIPS_I8, H5T_NATIVE_B8,
          H5T_NATIVE_INT_FAST8, H5T_NATIVE_INT_LEAST8, H5T_NATIVE_SHORT,
          H5T_MIPS_B8, H5T_STD_B8BE, H5T_STD_B8LE, H5T_STD_I8BE, H5T_STD_I8LE },

          /* GDL_STRING (redundant) */
        { H5T_C_S1, H5T_FORTRAN_S1, H5T_STRING, H5T_NATIVE_CHAR,
          H5T_NATIVE_SCHAR, H5T_NATIVE_UCHAR }
    };

    /* special case: compound data types */

    if (H5Tget_class(h5type)==H5T_COMPOUND)
       return GDL_STRUCT;

    /* special case: string data types */

    if (H5Tget_class(h5type)==H5T_STRING)
       return GDL_STRING;

    /* find matching GDL type */

    int idx=0;
    for (auto gdl_type: type_map) {
       for (auto hdf5_native_type: gdl_type){

          int comp = H5Tequal(h5type , hdf5_native_type);

          if(comp<0)
             { string msg; e->Throw(hdf5_error_message(msg)); }

          else if(comp>0) {
             if (debug) printf("match '%s'\n", matching_gdl_type_lbl[idx]);
             return matching_gdl_type[idx];
          }
       }
       idx++;
    }

    /* no matching type found */
    return GDL_UNDEF;
  }

  // --------------------------------------------------------------------

  hid_t hdf5_compound_create( DStructGDL*, EnvT* );

  hid_t mapGDLdatatypesToH5( BaseGDL* data, EnvT* e ) {

    hid_t native_type;

    switch ( (data[0]).Type() ) {

    /* IDL Note: If the data is an array, the datatype is constructed
       from the first element in the array. [...]  All elements of a
       string datatype will have the same length [...]  strings longer
       than the datatype length will be truncated. The size of the
       returned datatype will include a null termination [...] */

    case GDL_BYTE:    native_type = H5T_NATIVE_UINT8;  break;
    case GDL_INT:     native_type = H5T_NATIVE_INT16;  break;
    case GDL_UINT:    native_type = H5T_NATIVE_UINT16; break;
    case GDL_LONG:    native_type = H5T_NATIVE_INT32;  break;
    case GDL_ULONG:   native_type = H5T_NATIVE_UINT32; break;
    case GDL_LONG64:  native_type = H5T_NATIVE_INT64;  break;
    case GDL_ULONG64: native_type = H5T_NATIVE_UINT64; break;
    case GDL_FLOAT:   native_type = H5T_NATIVE_FLOAT;  break;
    case GDL_DOUBLE:  native_type = H5T_NATIVE_DOUBLE; break;
    case GDL_STRING:  native_type = H5T_C_S1;          break;

    case GDL_STRUCT:
      native_type = hdf5_compound_create(static_cast<DStructGDL*>(data),e);
      break;

    default:
      e->Throw("Unrecognized data type.");
    }

    /* crate datatype handle if required */
    if (!H5Iis_valid(native_type)) native_type = H5Tcopy(native_type);

    /* set size for string datatypes */
    if ( (data[0]).Type()==GDL_STRING ) {
      size_t len = strlen( (*static_cast<DStringGDL*>(data))[0].c_str() );
      if ( H5Tset_size(native_type, len+1) < 0 )
        { string msg; e->Throw(hdf5_error_message(msg)); }
    }

    return native_type;
  }

  // --------------------------------------------------------------------

  size_t tag_NBytes_0( BaseGDL* tag ) {

    /* count space for extra 0-terminators for string-type tag */
    size_t n_term=0;
    if ( tag->Type() == GDL_STRING ) {
      n_term=1;
      for(int i=0; i<tag->Dim().Rank(); i++) n_term *= tag->Dim(i);
    }

    return n_term *sizeof(char);
  }

  // --------------------------------------------------------------------

  size_t struct_NBytes_0( DStructGDL* d_struct ) {

    /* count extra 0-terminators for strings inside struct */
    SizeT nTags = d_struct->Desc()->NTags();
    size_t n_term_total=0;
    for(int t=0; t<nTags; t++)
      n_term_total += tag_NBytes_0(d_struct->GetTag(t));

    return n_term_total;
  }

  // --------------------------------------------------------------------

  hid_t hdf5_compound_create( DStructGDL* d_struct, EnvT* e ) {

    bool debug=false;
    static int indent=0; indent+=2;

    /* create HDF5 compound data type */
    hid_t cmp_id = H5Tcreate( H5T_COMPOUND, d_struct->SizeofTags()
                                          + struct_NBytes_0(d_struct) );
    size_t member_offset = 0;
    SizeT nTags = d_struct->Desc()->NTags();

    if (debug) printf( "%*screating type for struct '%s' "
                       "of size %lld with %lld members\n",
                       indent,"", d_struct->Desc()->Name().c_str(),
                       d_struct->SizeofTags(), nTags );

    for(int t=0; t<nTags; t++) {

      BaseGDL *tag = d_struct->GetTag(t);
      string tagName = (*d_struct).Desc()->TagName(t);

      hsize_t rank = tag->Dim().Rank(), dims[MAXRANK];
      for(int i=0; i<rank; i++) dims[rank-1-i] = tag->Dim(i);

      /* treat 1D data with single element as scalar */
      if (rank==1 && dims[0]==1) rank=0;

      if (debug) { /* print tag properties */
        printf( "%*stag '%s' of rank %lld, dimensions are: (",
                indent,"", tagName.c_str(), rank );
        for(int i=0; i<rank; i++)
          printf("%lld%s", tag->Dim(i), (i<rank-1) ? "," : "");
        printf("), size=%lld, offs=%ld\n", tag->NBytes(), member_offset);
      }

      /* obtain tag's HDF5 data type */
      hid_t member_type_id, elem_type_id = mapGDLdatatypesToH5(tag,e);

      if (rank>0) /* create array datatype if needed */
        member_type_id = H5Tarray_create2( elem_type_id, rank, dims );
      else
        member_type_id = H5Tcopy( elem_type_id );

      /* insert into HDF5 compound datatype */
      H5Tinsert( cmp_id, tagName.c_str(), member_offset, member_type_id );
      H5Tclose(member_type_id), H5Tclose(elem_type_id);

      member_offset += tag->NBytes() + tag_NBytes_0(tag);
    }

    indent-=2;

    return cmp_id;
  }

  // --------------------------------------------------------------------

  void hdf5_compound_gather( DStructGDL* d_struct,
                             char *buffer, hsize_t idx, EnvT* e ) {

    bool debug=false;
    static int indent=0; indent+=2;

    size_t member_offset=0;
    size_t cmp_sz=d_struct->SizeofTags()+struct_NBytes_0(d_struct);
    SizeT nTags = d_struct->Desc()->NTags();

    if (debug) printf( "%*sgathering data for struct '%s' "
                       "of size %ld with %lld members\n",
                       indent,"", d_struct->Desc()->Name().c_str(),
                       cmp_sz, nTags );

    for(int t=0; t<nTags; t++) {

      BaseGDL *tag = d_struct->GetTag(t,idx);
      string tagName = (*d_struct).Desc()->TagName(t);

      hsize_t rank = tag->Dim().Rank(), dims[MAXRANK];
      for(int i=0; i<rank; i++) dims[rank-1-i] = tag->Dim(i);

      if (debug) {
        printf( "%*stag '%s' of rank %lld, dimensions are: (",
                indent,"", tagName.c_str(), rank );
        for(int i=0; i<rank; i++)
          printf("%lld%s", tag->Dim(i), (i<rank-1) ? "," : "");
        printf( "), size=%lld, offs=%ld\n",
                tag->NBytes()+tag_NBytes_0(tag), member_offset );
      }

      if ( tag->Type() == GDL_STRUCT ) { /* nested structure */

        hdf5_compound_gather( static_cast<DStructGDL*>(tag),
                              buffer+member_offset, 0 , e );

      } else if ( tag->Type() == GDL_STRING ) { /* string data */

        size_t len=1+strlen((*static_cast<DStringGDL*>(tag))[0].c_str());
        for(int i=0; i<tag->Size(); i++)
          strncpy( &buffer[member_offset+i*len],
                   (char*)((*static_cast<DStringGDL*>(tag))[i].c_str()), len );

      } else { /* numeric data */

        memcpy( &buffer[member_offset], tag->DataAddr(), tag->NBytes() );
      }

      member_offset += tag->NBytes() + tag_NBytes_0(tag);
    }

    indent-=2;

    return;
  }

  // --------------------------------------------------------------------

  void hdf5_parse_compound( hid_t parent_type, DStructGDL* parent_struct,
                            char *raw, EnvT *e ) {

    bool debug=false;
    static int indent=0; indent+=2;

    size_t cmp_sz = H5Tget_size( parent_type );
    int idx, n_mem = H5Tget_nmembers( parent_type );

    if (debug) printf( "%*scompound datatype of size %ld with %d members\n",
                       indent,"", cmp_sz, n_mem );

    for(idx=0; idx<n_mem; idx++) {

      char type_lbl[200];

      hid_t member_class = H5Tget_member_class( parent_type, idx );
      if (member_class<0) { string msg; e->Throw(hdf5_error_message(msg)); }
      hdf5_type_guard member_class_guard = hdf5_type_guard(member_class);

      hid_t member_type = H5Tget_member_type( parent_type, idx );
      if (member_type<0) { string msg; e->Throw(hdf5_error_message(msg)); }
      hdf5_type_guard member_type_guard = hdf5_type_guard(member_type);

      char *member_name = H5Tget_member_name( parent_type, idx );
      if (!member_name) { string msg; e->Throw(hdf5_error_message(msg)); }
      hdf5_name_guard member_name_guard = hdf5_name_guard(member_name);

      size_t member_sz = H5Tget_size( member_type );
      size_t member_offs = H5Tget_member_offset( parent_type, idx );

      int member_rank=0;
      hsize_t member_dims[MAXRANK];

      switch (member_class) {

      case H5T_COMPOUND:
        {
          snprintf(type_lbl,200, "nested compound");

          DStructDesc* cmp_desc = new DStructDesc("$truct");
          DStructGDL* res = new DStructGDL(cmp_desc);

          hdf5_parse_compound( member_type, res, &raw[member_offs], e );
          parent_struct->NewTag(member_name, res);
        }
        break;

      case H5T_ARRAY:
        snprintf(type_lbl, 200, "array");

        if ((member_rank=H5Tget_array_ndims(member_type)) <0)
          { string msg; e->Throw(hdf5_error_message(msg)); }

        if (H5Tget_array_dims2(member_type, member_dims) <0)
          { string msg; e->Throw(hdf5_error_message(msg)); }

        break;

      case H5T_STRING:
        snprintf(type_lbl,200, "string");
        break;

      case H5T_INTEGER:
        snprintf(type_lbl,200, "integer");
        break;

      case H5T_FLOAT:
        snprintf(type_lbl,200, "float");
        break;

      case H5T_BITFIELD:
      case H5T_OPAQUE:
      case H5T_REFERENCE:
      case H5T_ENUM:
      case H5T_VLEN:
        e->Throw("read H5T_COMPOUND: feature not yet supported.");
        break;

      default:
        e->Throw("read H5T_COMPOUND: unknown member type.");
        break;
      }

      if (debug)
         printf( "%*sfound %s element '%s' of size %ld at offset %ld\n",
                 indent, "", type_lbl, member_name, member_sz, member_offs );

      hid_t elem_type;

      if (member_class==H5T_ARRAY)
        elem_type = H5Tget_super(member_type);
      else
        elem_type = H5Tcopy(member_type);

      hdf5_type_guard elem_type_guard = hdf5_type_guard(elem_type);

      SizeT rank_s=member_rank;
      SizeT count_s[MAXRANK];
      for(int i=0; i<rank_s; i++)
        count_s[i] = (SizeT) member_dims[member_rank-1-i];

      // create the IDL datatypes
      dimension dim(count_s, rank_s);
      BaseGDL *field=NULL;
      DLong ourType = mapH5DatatypesToGDL(elem_type,e);

      if (ourType == GDL_BYTE) {
        field = new DByteGDL(dim);
      } else if (ourType == GDL_INT) {
        field = new DIntGDL(dim);
      } else if (ourType == GDL_UINT) {
        field = new DUIntGDL(dim);
      } else if (ourType == GDL_LONG) {
        field = new DLongGDL(dim);
      } else if (ourType == GDL_ULONG) {
        field = new DULongGDL(dim);
      } else if (ourType == GDL_LONG64) {
        field = new DLong64GDL(dim);
      } else if (ourType == GDL_ULONG64) {
        field = new DULong64GDL(dim);
      } else if (ourType == GDL_FLOAT) {
        field = new DFloatGDL(dim);
      } else if (ourType == GDL_DOUBLE) {
        field = new DDoubleGDL(dim);
      } else if (ourType == GDL_STRING) {

        // string length (terminator included)
        SizeT str_len = H5Tget_size(elem_type);

        // number of array elements
        SizeT num_elems=member_sz/str_len;

        // allocate string buffer
        char* name = static_cast<char*>(calloc(member_sz,sizeof(char)));
        if (name == NULL) e->Throw("Failed to allocate memory!");
        hdf5_name_guard name_guard = hdf5_name_guard(name);

        // create GDL variable
        dimension flat_dim(&num_elems, 1);
        BaseGDL *str_arr = new DStringGDL(flat_dim);

        // assign array pointers
        for (size_t i=0; i<num_elems; i++) {
          strncpy(name+str_len*i,&raw[member_offs+str_len*i],str_len);
          (*(static_cast<DStringGDL*> (str_arr)))[i] = name+str_len*i;
        }

        // re-shape array & add as tag
        (static_cast<BaseGDL*>(str_arr))->SetDim(dim);
        parent_struct->NewTag( member_name, str_arr );
      }

      if(field) {
        parent_struct->NewTag( member_name, field );
        memcpy( field->DataAddr(), &raw[member_offs], member_sz*sizeof(char) );
      }
    }

    indent-=2;
    return;
  }

  // --------------------------------------------------------------------

  void hdf5_basic_read( hid_t loc_id, hid_t datatype,
                        hid_t ms_id, hid_t fs_id, void* raw, EnvT* e ) {

    herr_t status;

    switch( H5Iget_type(loc_id) ) {

    case H5I_DATASET:
      status = H5Dread(loc_id, datatype, ms_id, fs_id, H5P_DEFAULT, raw);
      break;

    case H5I_ATTR:
      status = H5Aread(loc_id, datatype, raw);
      break;

    default:
      break;
    }
    if (status < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    return;
  }

  // --------------------------------------------------------------------

  void hdf5_unified_write( hid_t loc_id, BaseGDL* data,
                           hid_t ms_id, hid_t fs_id, EnvT* e ) {

    /* Dec 2021, Oliver Gressel <ogressel@gmail.com>
       - deduplicate implementations for writing data / attributes
    */


    /* --- obtain datatype handle --- */

    hid_t type_id;

    switch( H5Iget_type(loc_id) ) {
    case H5I_DATASET: type_id = H5Dget_type(loc_id); break;
    case H5I_ATTR:    type_id = H5Aget_type(loc_id); break;
    default:          e->Throw("unsupported use for hdf5_unified_write\n");
    }
    if (type_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    hdf5_type_guard type_id_guard = hdf5_type_guard(type_id);


    /* --- obtain the elementary datatype --- */

    hid_t elem_type_id;
    if (H5Tget_class(type_id)==H5T_ARRAY)
      elem_type_id = H5Tget_super(type_id);
    else
      elem_type_id = H5Tcopy(type_id);
    hdf5_type_guard elem_type_guard = hdf5_type_guard(elem_type_id);


    /* --- assert contiguous write buffer --- */

    char *buffer=NULL;
    void *data_addr=NULL;


    if (H5Tget_class(elem_type_id)==H5T_COMPOUND) {         /* --- struct */

      DStructGDL* d_struct = static_cast<DStructGDL*>(data);
      hsize_t n_elem=d_struct->N_Elements(),
              len=d_struct->SizeofTags()+struct_NBytes_0(d_struct);

      data_addr = buffer = static_cast<char*>(calloc(n_elem*len,sizeof(char)));
      if (buffer == NULL) e->Throw("Failed to allocate memory!");

      for (hsize_t idx=0; idx<n_elem; idx++)
         hdf5_compound_gather(d_struct, &buffer[idx*len], idx, e);

    } else if (H5Tget_class(elem_type_id)==H5T_STRING) {    /* --- string */

      size_t n_elem=data->Size(), len=H5Tget_size(elem_type_id);

      data_addr = buffer = static_cast<char*>(calloc(n_elem*len,sizeof(char)));
      if (buffer == NULL) e->Throw("Failed to allocate memory!");

      for(int i=0; i<n_elem; i++)
        strncpy( &buffer[i*len],
                 (*static_cast<DStringGDL*>(data))[i].c_str(), len );


    } else data_addr = data->DataAddr();                    /* --- numeric */


    /* --- write dataset/attribute to file --- */

    herr_t status;

    switch( H5Iget_type(loc_id) ) {

    case H5I_DATASET:
      status = H5Dwrite(loc_id, type_id, ms_id, fs_id, H5P_DEFAULT, data_addr);
      break;

    case H5I_ATTR:
      status = H5Awrite(loc_id, type_id, data_addr);
      break;

    default:
      break;
    }

    if (status < 0) { string msg; e->Throw(hdf5_error_message(msg)); }


    /* --- free resources --- */

    if(buffer) free(buffer), buffer=NULL;

    return;
  }

  // --------------------------------------------------------------------

  BaseGDL* hdf5_unified_read( hid_t loc_id,
                              hid_t ms_id, hid_t fs_id, EnvT* e ) {

    /* Nov 2021, Oliver Gressel <ogressel@gmail.com>
       - deduplicate implementations for reading data / attributes
    */

    bool debug=false;
    hid_t status=0;


    /* --- obtain the datatype handle --- */

    hid_t datatype;

    switch( H5Iget_type(loc_id) ) {
    case H5I_DATASET: datatype = H5Dget_type(loc_id); break;
    case H5I_ATTR:    datatype = H5Aget_type(loc_id); break;
    default:          e->Throw("unsupported use for hdf5_unified_read\n");
    }
    if (datatype < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    hdf5_type_guard datatype_guard = hdf5_type_guard(datatype);


    /* --- for array datatypes, determine rank+dimension of the element --- */

    hid_t elem_dtype;
    int elem_rank=0;
    hsize_t elem_dims[MAXRANK];

    if ( H5Tget_class(datatype)==H5T_ARRAY ) {

       if ( (elem_rank=H5Tget_array_ndims(datatype)) <0 )
          { string msg; e->Throw(hdf5_error_message(msg)); }

       if ( H5Tget_array_dims2(datatype, elem_dims) <0 )
          { string msg; e->Throw(hdf5_error_message(msg)); }

       if (debug) {
         cout << "array datatype of rank: " << elem_rank << endl;
         if (elem_rank>0) {
           cout << "dimensions are: ";
           for(int i=0; i<elem_rank; i++) cout << elem_dims[i] << ",";
           cout << endl;
         }
       }

       elem_dtype = H5Tget_super(datatype);
       if (elem_dtype < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    } else {

       elem_dtype = H5Tcopy(datatype);
       if (elem_dtype < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
    }

    hdf5_type_guard elem_dtype_guard = hdf5_type_guard(elem_dtype);


    /* --- determine the rank and dimension of the dataspace --- */

    int data_rank;
    hsize_t data_dims[MAXRANK];

    if ( (data_rank = H5Sget_simple_extent_ndims(ms_id)) < 0 )
      { string msg; e->Throw(hdf5_error_message(msg)); }

    if ( H5Sget_simple_extent_dims(ms_id, data_dims, NULL) < 0 )
      { string msg; e->Throw(hdf5_error_message(msg)); }

    if (debug) {
      cout << "data rank is: " << data_rank << endl;
      if (data_rank>0) {
        cout << "dimensions are: ";
        for(int i=0; i<data_rank; i++) cout << data_dims[i] << ",";
        cout << endl;
      }
    }

    // dimensions for GDL variable
    SizeT count_s[MAXRANK];
    SizeT rank_s;

    rank_s = (SizeT) (elem_rank + data_rank);

    // need to reverse indices for column major format
    for(int i=0; i<elem_rank; i++)
      count_s[i] = (SizeT)elem_dims[elem_rank - 1  - i ];

    for(int i=elem_rank; i<elem_rank+data_rank; i++)
      count_s[i] = (SizeT)data_dims[elem_rank + data_rank - 1  - i ];

    // create the GDL datatypes
    dimension dim(count_s, rank_s);

    BaseGDL *res;
    hsize_t type;
    DLong ourType = mapH5DatatypesToGDL(elem_dtype,e);

    if (debug)  cout << "ourType : " << ourType  << endl;

    if (ourType == GDL_BYTE) {
      res = new DByteGDL(dim);
      type = H5T_NATIVE_UINT8;

    } else if (ourType == GDL_INT) {
      res = new DIntGDL(dim);
      type = H5T_NATIVE_INT16;

    } else if (ourType == GDL_UINT) {
      res = new DUIntGDL(dim);
      type = H5T_NATIVE_UINT16;

    } else if (ourType == GDL_LONG) {
      res = new DLongGDL(dim);
      type = H5T_NATIVE_INT32;

    } else if (ourType == GDL_ULONG) {
      res = new DULongGDL(dim);
      type = H5T_NATIVE_UINT32;

    } else if (ourType == GDL_LONG64) {
      res = new DLong64GDL(dim);
      type = H5T_NATIVE_INT64;

    } else if (ourType == GDL_ULONG64) {
      res = new DULong64GDL(dim);
      type = H5T_NATIVE_UINT64;

    } else if (ourType == GDL_FLOAT) {
      res = new DFloatGDL(dim);
      type = H5T_NATIVE_FLOAT;

    } else if (ourType == GDL_DOUBLE) {
      res = new DDoubleGDL(dim);
      type = H5T_NATIVE_DOUBLE;

    } else if (ourType == GDL_STRING) {

      bool isVarLenStr = (H5Tis_variable_str(elem_dtype) > 0);
      if (debug) printf(isVarLenStr ? "variable-length string dataset\n"
                                    : "fixed-length string dataset\n");

      // total number of array elements
      SizeT num_elems=1;
      for(int i=0; i<rank_s; i++) num_elems *= count_s[i];

      // create GDL variable
      dimension flat_dim(&num_elems, 1);
      res = new DStringGDL(flat_dim);

      if (isVarLenStr) {        // variable-length strings

        // allocate memory for reading the strings
        char **raw = (char **) malloc(num_elems * sizeof(char *));
        hdf5_basic_read( loc_id, datatype, ms_id, fs_id, &raw[0], e );

        // assign array pointers
        for (size_t i=0; i<num_elems; i++) {

          int len = strlen(raw[i]);
          char *str = (char*) malloc((len+1)*sizeof(char));
          strncpy(str, raw[i], len+1);
          if(debug) printf("str[%ld]='%s' is of length %d\n", i, str, len);
          (*(static_cast<DStringGDL*> (res)))[i] = str;
        }

        // reclaim data buffers
        H5Dvlen_reclaim (datatype, ms_id, H5P_DEFAULT, &raw[0]);

      } else {                  // fixed-length strings

        // string length (terminator included)
        SizeT str_len = H5Tget_size(elem_dtype);

        // allocate & read raw buffer
        char* raw = (char*) malloc(num_elems*str_len*sizeof(char));
        hdf5_name_guard raw_guard = hdf5_name_guard(raw);
        hdf5_basic_read( loc_id, datatype, ms_id, fs_id, raw, e );

        // assign array pointers
        for (size_t i=0; i<num_elems; i++)
          (*(static_cast<DStringGDL*> (res)))[i] = raw + str_len*i;
      }

      // re-shape array to match dataset
      (static_cast<BaseGDL*>(res))->SetDim(dim);

      return res;

    } else if (ourType == GDL_STRUCT) {

       if (debug) printf("compound dataset\n");

       // get total number of array elements and the size of each
       SizeT num_elems=1;
       for(int i=0; i<rank_s; i++) num_elems *= count_s[i];
       size_t cmp_sz = H5Tget_size(datatype);
       if (cmp_sz < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
       // Allocate byte array to read the raw data
       char* raw = (char*) malloc(num_elems*cmp_sz*sizeof(char));
       // read raw-data for compound dataset
       hdf5_basic_read( loc_id, datatype, ms_id, fs_id, raw, e );

       // Now parse the data into a single gdl structure
       DStructDesc* cmp_desc1 = new DStructDesc("$truct");
       DStructGDL* res1 = new DStructGDL(cmp_desc1);
       hdf5_parse_compound( datatype, res1, raw, e );
       // Now can make array to hold all elements (using the structure description defined by parsing the first element)
       dimension flat_dim(&num_elems, 1);
       DStructGDL* res = new DStructGDL(res1->Desc(),flat_dim);
       // Also get the number of tags (will need to copy each tag)
       int NTags=res1->Desc()->NTags();
//printf("n_elems: %d; n_tags %d\n",(int)num_elems,NTags);

       // If there are more elements then parse each element in turn and copy data into res
       // (which is declared as a flat struct array)
       // Each element is treated as a separate struct to be constructed by hdf5_parse_compound
       // It is assumed all the structs will have identical tags and datasize (this should be safe assumption for compound array)
       for (size_t i=0; i<num_elems; i++) {
         DStructDesc* cmp_desci = new DStructDesc("$truct");
         DStructGDL* resi = new DStructGDL(cmp_desci);
         hdf5_parse_compound( datatype, resi, raw+cmp_sz*i, e );
         for( size_t itag=0; itag<NTags; itag++) {
            *(res->GetTag(itag,i)) = *(resi->GetTag(itag));
         }
       }
       // re-shape res to have the intended dimensions array to match dataset
       (static_cast<BaseGDL*>(res))->SetDim(dim);
       // Free the raw data
       free(raw);

       return res;

    } else {

      e->Throw("Unsupported data format" + i2s(elem_dtype));

    }

    /* --- read regular datatypes (i.e., non-string/struct) --- */

    if (elem_rank>0) type = H5Tarray_create2( type, elem_rank, elem_dims );
    else type = H5Tcopy(type);
    hdf5_type_guard type_guard = hdf5_type_guard(type);

    hdf5_basic_read( loc_id, type, ms_id, fs_id, res->DataAddr(), e );

    return res;
  }

  // --------------------------------------------------------------------

  BaseGDL* h5_get_libversion_fun( EnvT* e)
  {
    unsigned int majnum, minnum, relnum;
    if (H5get_libversion(&majnum, &minnum, &relnum) < 0)
      { string msg; e->Throw(hdf5_error_message(msg)); }
    return new DStringGDL(i2s(majnum) + "." + i2s(minnum) + "." + i2s(relnum));
  }

  /* AC 2019 Jan : managing a major change since Release 1.10.0. !!

https://support.hdfgroup.org/HDF5/doc/ADGuide/Changes.html

This section lists interface-level changes and other user-visible
changes in behavior in the transition from HDF5 Release 1.8.16 to
Release 1.10.0.  Changed Type

hid_t
    Changed from a 32-bit to a 64-bit value.
  */

   hid_t hdf5_input_conversion_kw( EnvT* e, int position)
  {

    hid_t hdf5_id;

#if (H5_VERS_MAJOR>1)||((H5_VERS_MAJOR==1)&&(H5_VERS_MINOR>=10))
        DLong64 temp=0;
    e->AssureLongScalarKW(position, temp);
        hdf5_id=temp;
#else
    e->AssureLongScalarKW(position, hdf5_id);
#endif
    return  hdf5_id;
  }

  hid_t hdf5_input_conversion( EnvT* e, int position)
  {

    hid_t hdf5_id;

#if (H5_VERS_MAJOR>1)||((H5_VERS_MAJOR==1)&&(H5_VERS_MINOR>=10))
        DLong64 temp=0;
    e->AssureLongScalarPar(position, temp);
        hdf5_id=temp;
#else
    e->AssureLongScalarPar(position, hdf5_id);
#endif
    return  hdf5_id;
  }

  BaseGDL* hdf5_output_conversion( hid_t h5type)
  {

#if (H5_VERS_MAJOR>1)||((H5_VERS_MAJOR==1)&&(H5_VERS_MINOR>=10))
    return new DLong64GDL(h5type);
#else
    return new DLongGDL(h5type);
#endif

  }

  BaseGDL* h5f_is_hdf5_fun( EnvT* e)
  {
    DString h5fFilename;
    e->AssureScalarPar<DStringGDL>( 0, h5fFilename);
    WordExp( h5fFilename);
    htri_t code;
    code = H5Fis_hdf5(h5fFilename.c_str());
    if (code <= 0) return new DLongGDL(0); else  return new DLongGDL(1);
  }


  BaseGDL* h5f_create_fun( EnvT* e)
  {

    DString h5fFilename;
    e->AssureScalarPar<DStringGDL>( 0, h5fFilename);
    WordExp( h5fFilename);

    hid_t h5f_id;
    h5f_id = H5Fcreate( h5fFilename.c_str(), H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);

    if (h5f_id < 0)
      {
        string msg;
        e->Throw(hdf5_error_message(msg));
      }

    return hdf5_output_conversion( h5f_id );
  }

  BaseGDL* h5f_open_fun( EnvT* e)
  {

    /* mandatory 'Filename' paramter */
    DString h5fFilename;
    e->AssureScalarPar<DStringGDL>( 0, h5fFilename);
    WordExp( h5fFilename);

    /* optional keyword 'WRITE' parameter */
    static int writeIx = e->KeywordIx("WRITE");
    hbool_t write = e->KeywordSet(writeIx);

    hid_t h5f_id = H5Fopen( h5fFilename.c_str(),
                            (write) ? H5F_ACC_RDWR
                                    : H5F_ACC_RDONLY, H5P_DEFAULT );
    if (h5f_id < 0)
      { string msg; e->Throw(hdf5_error_message(msg)); }

    return hdf5_output_conversion( h5f_id );
  }

  BaseGDL* h5g_open_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(2);

    hid_t h5f_id = hdf5_input_conversion(e, 0);

    DString h5gGroupname;
    e->AssureScalarPar<DStringGDL>( 1, h5gGroupname);

    hid_t h5g_id;
    h5g_id = H5Gopen(h5f_id, h5gGroupname.c_str());
    if (h5g_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    return hdf5_output_conversion( h5g_id );

  }


  void h5g_set_comment_pro( EnvT* e)
  {
    SizeT nParam=e->NParam(3);

    /* mandatory 'Loc_id' parameter */
    hid_t loc_id = hdf5_input_conversion(e,0);

    /* mandatory 'Name' parameter */
    DString name;
    e->AssureScalarPar<DStringGDL>(1, name);

    /* mandatory 'Comment' parameter */
    DString comment;
    e->AssureScalarPar<DStringGDL>(2, comment);

    if ( H5Gset_comment( loc_id, name.c_str(), comment.c_str() ) < 0 )
      { string msg; e->Throw(hdf5_error_message(msg)); }
  }


  void h5g_link_pro( EnvT* e)
  {
    /* Dec 2022, Oliver Gressel <ogressel@gmail.com>
    */
    SizeT nParam=e->NParam(3);

    /* mandatory 'Loc_id' parameter */
    hid_t loc_id = hdf5_input_conversion(e,0);

    /* mandatory 'Current_Name' parameter */
    DString current_name;
    e->AssureScalarPar<DStringGDL>(1, current_name);

    /* mandatory 'New_Name' parameter */
    DString new_name;
    e->AssureScalarPar<DStringGDL>(2, new_name);

    /* optional 'SOFTLINK' keyword parameter */
    static int soft_idx = e->KeywordIx("SOFTLINK");
    hbool_t soft_link = e->KeywordSet(soft_idx);
    H5L_type_t type = (soft_link) ? H5G_LINK_SOFT : H5G_LINK_HARD;

    /* optional 'NEW_LOC_ID' keyword parameter */
    static int new_idx = e->KeywordIx("NEW_LOC_ID");

    /* create the link */
    if (e->GetKW(new_idx)!=NULL) {

       hid_t new_loc_id = hdf5_input_conversion_kw(e,new_idx);

       if ( H5Glink2(loc_id, current_name.c_str(), type,
                     new_loc_id, new_name.c_str()) < 0 )
          { string msg; e->Throw(hdf5_error_message(msg)); }

    } else {

       if ( H5Glink(loc_id, type, current_name.c_str(), new_name.c_str()) < 0 )
          { string msg; e->Throw(hdf5_error_message(msg)); }
    }

  }


  void h5g_unlink_pro( EnvT* e)
  {
    /* Dec 2022, Oliver Gressel <ogressel@gmail.com>
    */
    SizeT nParam=e->NParam(2);

    /* mandatory 'Loc_id' parameter */
    hid_t loc_id = hdf5_input_conversion(e,0);

    /* mandatory 'Name' parameter */
    DString name;
    e->AssureScalarPar<DStringGDL>(1, name);

    if ( H5Gunlink(loc_id, name.c_str()) < 0 )
       { string msg; e->Throw(hdf5_error_message(msg)); }
  }


  BaseGDL* h5g_get_linkval_fun( EnvT* e)
  {
    /* Dec 2022, Oliver Gressel <ogressel@gmail.com>
    */
    SizeT nParam=e->NParam(2);

    /* mandatory 'Loc_id' parameter */
    hid_t loc_id = hdf5_input_conversion(e, 0);

    /* mandatory 'Name' parameter */
    DString name;
    e->AssureScalarPar<DStringGDL>( 1, name);

    /* check link type & query the string length */
    H5L_info_t link_info;
    if ( H5Lget_info(loc_id, name.c_str(), &link_info, H5P_DEFAULT) < 0 )
       { string msg; e->Throw(hdf5_error_message(msg)); }

    if ( link_info.type != H5L_TYPE_SOFT )
       e->Throw("Input argument is not a symbolic link");

    /* allocate string buffer */
    size_t len=link_info.u.val_size;
    char* linkval = static_cast<char*>(calloc(len+1,sizeof(char)));
    if (linkval == NULL) e->Throw("Failed to allocate memory!");
    hdf5_name_guard linkval_guard = hdf5_name_guard(linkval);

    /* obtain the value string */
    if( H5Gget_linkval(loc_id, name.c_str(), len+1, linkval) < 0)
       { string msg; e->Throw(hdf5_error_message(msg)); }

    return new DStringGDL(linkval);
  }


  void h5g_move_pro( EnvT* e)
  {
    /* Dec 2022, Oliver Gressel <ogressel@gmail.com>
    */
    SizeT nParam=e->NParam(3);

    /* mandatory 'Loc_id' parameter */
    hid_t loc_id = hdf5_input_conversion(e,0);

    /* mandatory 'Src_Name' parameter */
    DString src_name;
    e->AssureScalarPar<DStringGDL>(1, src_name);

    /* mandatory 'Dst_Name' parameter */
    DString dst_name;
    e->AssureScalarPar<DStringGDL>(2, dst_name);

    /* optional 'NEW_LOC_ID' keyword parameter */
    static int new_idx = e->KeywordIx("NEW_LOC_ID");

    /* perform the move/rename operation */
    if (e->GetKW(new_idx)!=NULL) {

       hid_t src_loc_id = loc_id;
       hid_t dst_loc_id = hdf5_input_conversion_kw(e,new_idx);

       if ( H5Gmove2( src_loc_id, src_name.c_str(),
                      dst_loc_id, dst_name.c_str() ) < 0 )
          { string msg; e->Throw(hdf5_error_message(msg)); }

    } else {

       if ( H5Gmove(loc_id, src_name.c_str(), dst_name.c_str()) < 0 )
          { string msg; e->Throw(hdf5_error_message(msg)); }
    }

  }


  BaseGDL* h5g_get_comment_fun( EnvT* e)
  {
    /* Dec 2022, Oliver Gressel <ogressel@gmail.com>
    */
    SizeT nParam=e->NParam(2);

    /* mandatory 'Loc_id' parameter */
    hid_t loc_id = hdf5_input_conversion(e, 0);

    /* mandatory 'Name' parameter */
    DString name;
    e->AssureScalarPar<DStringGDL>( 1, name);

    /* query the string length */
    ssize_t len = H5Gget_comment(loc_id, name.c_str(), 0, NULL);
    if( len < 0 ) { string msg; e->Throw(hdf5_error_message(msg)); }

    /* allocate string buffer */
    char* comment_str = static_cast<char*>(calloc(len+1,sizeof(char)));
    if (comment_str == NULL) e->Throw("Failed to allocate memory!");
    hdf5_name_guard comment_str_guard = hdf5_name_guard(comment_str);

    /* obtain the comment string */
    if( H5Gget_comment(loc_id, name.c_str(), len+1, comment_str) < 0)
       { string msg; e->Throw(hdf5_error_message(msg)); }

    return new DStringGDL(comment_str);

  }


  BaseGDL* h5g_create_fun( EnvT* e)
  {
    /* Dec 2022, Oliver Gressel <ogressel@gmail.com>
    */
    SizeT nParam=e->NParam(2);

    hid_t h5f_id = hdf5_input_conversion(e, 0);

    DString h5gGroupname;
    e->AssureScalarPar<DStringGDL>( 1, h5gGroupname);

    size_t size_hint=-1; /* use default size for name heap */

    hid_t h5g_id;
    h5g_id = H5Gcreate(h5f_id, h5gGroupname.c_str(), size_hint);
    if (h5g_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    return hdf5_output_conversion( h5g_id );

  }


  static herr_t count_members( hid_t loc_id, const char *name, void *data ) {
     (*(size_t*)data)++;
     return 0;
  }

  BaseGDL* h5g_get_nmembers_fun( EnvT* e)
  {
    /* Dec 2022, Oliver Gressel <ogressel@gmail.com>
    */
    SizeT nParam=e->NParam(2);

    hid_t h5f_id = hdf5_input_conversion(e, 0);

    DString h5gGroupname;
    e->AssureScalarPar<DStringGDL>( 1, h5gGroupname);

    /* use 'H5Giterate()' to count members (as per IDL manual) */
    size_t nmembers=0;
    if ( H5Giterate( h5f_id, h5gGroupname.c_str(),
                     NULL, count_members, &nmembers ) < 0 )
       { string msg; e->Throw(hdf5_error_message(msg)); }

    return new DLongGDL( nmembers );

  }


  static herr_t get_len( hid_t loc_id, const char *name, void *data ) {
    static_cast<size_t*>(data)[0] = (name) ? strlen(name) : 0;
    return 1;
  }

  static herr_t get_name( hid_t loc_id, const char *name, void *data ) {
    strcpy(static_cast<char*>(data),name);
    return 1;
  }

  BaseGDL* h5g_get_member_name_fun( EnvT* e)
  {
    /* Dec 2022, Oliver Gressel <ogressel@gmail.com>
    */
    SizeT nParam=e->NParam(3);

    /* mandatory 'Loc_id' parameter */
    hid_t loc_id = hdf5_input_conversion(e, 0);

    /* mandatory 'Name' parameter */
    DString name;
    e->AssureScalarPar<DStringGDL>( 1, name);

    /* mandatory 'Index' parameter */
    DLong index;
    e->AssureLongScalarKW(2, index);

    /* use 'H5Giterate()' to get member name length */
    size_t name_len;
    if ( H5Giterate( loc_id, name.c_str(), &index, get_len, &name_len ) < 0 )
       { string msg; e->Throw(hdf5_error_message(msg)); }
    index--;

    /* allocate string buffer */
    char* member_name=static_cast<char*>(calloc(name_len+1,sizeof(char)));
    hdf5_name_guard member_name_guard = hdf5_name_guard(member_name);

    /* use 'H5Giterate()' to get member name (as per IDL manual) */
    if ( H5Giterate( loc_id, name.c_str(), &index, get_name, member_name ) < 0 )
       { string msg; e->Throw(hdf5_error_message(msg)); }

    return new DStringGDL(member_name);

  }


  BaseGDL* h5g_get_objinfo_fun( EnvT* e)
  {
     /* Nov 2021, Oliver Gressel <ogressel@gmail.com>
        - add basic functionality
     */

    SizeT nParam=e->NParam(2);

    /* mandatory 'Loc_id' parameter */
    hid_t h5f_id = hdf5_input_conversion(e, 0);

    /* mandatory 'Name' parameter */
    DString h5g_name;
    e->AssureScalarPar<DStringGDL>( 1, h5g_name);

    /* keyword 'FOLLOW_LINK' parameter */
    static int followIx = e->KeywordIx("FOLLOW_LINK");
    hbool_t follow_link = e->KeywordSet(followIx);

    /* execute the HDF5 library function */
    H5G_stat_t statbuf;
    if ( H5Gget_objinfo(h5f_id, h5g_name.c_str(), follow_link, &statbuf) < 0 )
      { string msg; e->Throw(hdf5_error_message(msg)); }

    DULong fileno[2] = { static_cast<DULong>(statbuf.fileno[0]),
                         static_cast<DULong>(statbuf.fileno[1]) };
    DULong objno[2] = { static_cast<DULong>(statbuf.objno[0]),
                        static_cast<DULong>(statbuf.objno[1]) };

    /* create return object */
    DStructDesc* res_desc = new DStructDesc("H5G_STAT");
    DStructGDL* res = new DStructGDL(res_desc);

    /* populate the GDL structure */
    res->NewTag("FILENO", new DULongGDL(fileno,2));
    res->NewTag("OBJNO", new DULongGDL(objno,2));
    res->NewTag("NLINK", new DULongGDL(statbuf.nlink));

    switch(statbuf.type) {
    case H5G_UNKNOWN: res->NewTag("TYPE", new DStringGDL("UNKNOWN")); break;
    case H5G_GROUP:   res->NewTag("TYPE", new DStringGDL("GROUP"));   break;
    case H5G_DATASET: res->NewTag("TYPE", new DStringGDL("DATASET")); break;
    case H5G_TYPE:    res->NewTag("TYPE", new DStringGDL("TYPE"));    break;
    case H5G_LINK:    res->NewTag("TYPE", new DStringGDL("LINK"));    break;
    default:          e->Throw("type error");
    }

    res->NewTag("MTIME", new DULongGDL(statbuf.mtime));
    res->NewTag("LINKLEN", new DULongGDL(statbuf.linklen));

    return res;

  }


  BaseGDL* h5g_get_num_objs_fun( EnvT* e)
  {
    /* Dec 2022, Oliver Gressel <ogressel@gmail.com> */

    SizeT nParam=e->NParam(1);

    /* mandatory 'Loc_id' parameter */
    hid_t loc_id = hdf5_input_conversion(e, 0);

    hsize_t num;
    if ( H5Gget_num_objs(loc_id, &num) < 0 )
      { string msg; e->Throw(hdf5_error_message(msg)); }

    return new DLongGDL( num );

  }


  BaseGDL* h5g_get_obj_name_by_idx_fun( EnvT* e)
  {
    /* Dec 2022, Oliver Gressel <ogressel@gmail.com> */

    SizeT nParam=e->NParam(2);

    /* mandatory 'Loc_id' parameter */
    hid_t loc_id = hdf5_input_conversion(e, 0);

    /* mandatory 'Index' parameter */
    DLong index;
    e->AssureLongScalarPar(1, index);

    ssize_t len =
       H5Gget_objname_by_idx(loc_id, index, NULL, 0);
    if( len < 0 )
       { string msg; e->Throw(hdf5_error_message(msg)); }
    else if ( len == 0 )
       { /* FIXME: handle the no-name-associated case */ }

    /* allocate string buffer */
    char* obj_name = static_cast<char*>(calloc(len+1,sizeof(char)));
    if (obj_name == NULL) e->Throw("Failed to allocate memory!");
    hdf5_name_guard obj_name_guard = hdf5_name_guard(obj_name);

    /* obtain the member name */
    if( H5Gget_objname_by_idx(loc_id, index, obj_name, len+1) < 0)
       { string msg; e->Throw(hdf5_error_message(msg)); }

    return new DStringGDL(obj_name);

  }


  BaseGDL* h5d_open_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(2);

    hid_t h5f_id = hdf5_input_conversion(e,0);

    DString h5dDatasetname;
    e->AssureScalarPar<DStringGDL>( 1, h5dDatasetname);

    hid_t h5d_id= H5Dopen( h5f_id, h5dDatasetname.c_str() );

    if (h5d_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    return hdf5_output_conversion( h5d_id );

  }


  BaseGDL* h5a_create_fun( EnvT* e)
  {
    /* Dec 2021, Oliver Gressel <ogressel@gmail.com>
       - implement basic functionality
    */

    SizeT nParam=e->NParam(4);

    /* mandatory 'Loc_id' parameter */
    hid_t loc_id = hdf5_input_conversion(e,0);

    /* mandatory 'Name' paramter */
    DString dset_name;
    e->AssureScalarPar<DStringGDL>(1,dset_name);

    /* mandatory 'Datatype_id' paramter */
    hid_t type_id = hdf5_input_conversion(e,2);
    if (H5Iis_valid(type_id) <= 0)
      e->Throw("not a datatype: Object ID:" + i2s( type_id ));

    /* mandatory 'Dataspace_id' paramter */
    hid_t space_id = hdf5_input_conversion(e,3);
    if (H5Iis_valid(space_id) <= 0)
      e->Throw("not a dataspace: Object ID:" + i2s( space_id ));

    hid_t h5a_id = H5Acreate( loc_id, dset_name.c_str(),
                              type_id, space_id, H5P_DEFAULT );
    if (h5a_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    return hdf5_output_conversion( h5a_id );
  }


  void h5a_write_pro(EnvT* e) {

    /* Dec 2021, Oliver Gressel <ogressel@gmail.com>
       - implement basic support for writing HDF5 attributes
    */

    SizeT nParam = e->NParam(2);

    hid_t dset_id = hdf5_input_conversion(e,0);
    BaseGDL* data = e->GetParDefined(1);

    /* --- write the dataset to file ---*/

    hdf5_unified_write( dset_id, data, H5I_BADID, H5I_BADID, e );

    return;
  }


  void h5a_delete_pro(EnvT* e) {

    /* Dec 2021, Oliver Gressel <ogressel@gmail.com>
       - implement support for deleting HDF5 attributes
    */

    SizeT nParam=e->NParam(2);

    hid_t loc_id = hdf5_input_conversion(e,0);

    DString attr_name;
    e->AssureScalarPar<DStringGDL>( 1, attr_name );

    if (H5Adelete( loc_id, attr_name.c_str() ) < 0)
      e->Throw( "unable to delete attribute: (Object ID:"+i2s(loc_id)+
                ", Object Name:\""+attr_name.c_str()+"\")" );
    return;
  }


  BaseGDL* h5a_open_idx_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(2);

    hid_t h5f_id = hdf5_input_conversion(e,0);
    hid_t attr_idx = hdf5_input_conversion(e,1);

    hid_t h5a_id = H5Aopen_idx(h5f_id, attr_idx);
    if (h5a_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    return hdf5_output_conversion( h5a_id );
  }


  BaseGDL* h5a_get_name_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);

    hid_t h5a_id = hdf5_input_conversion(e,0);

    // querying for the length of the name
    char tmp;
    ssize_t len = H5Aget_name(h5a_id, 1, &tmp);
    if (len < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    // acquiring the name
    len++;
    char* name = static_cast<char*>(malloc(len * sizeof(char)));
    if (name == NULL) e->Throw("Failed to allocate memory!");
    if (H5Aget_name(h5a_id, len, name) < 0)
      {
        free(name);
        { string msg; e->Throw(hdf5_error_message(msg)); }
        return NULL;
      }
    DStringGDL* ret = new DStringGDL(name);
    free(name);
    return ret;
  }


  BaseGDL* h5a_get_type_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);

    hid_t h5a_id = hdf5_input_conversion(e,0);

    hid_t h5a_type_id;
    h5a_type_id = H5Aget_type( h5a_id );
    if (h5a_type_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    return hdf5_output_conversion( h5a_type_id);
  }


  BaseGDL* h5a_open_name_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(2);

    hid_t h5f_id = hdf5_input_conversion(e,0);

    DString h5aAttrname;
    e->AssureScalarPar<DStringGDL>( 1, h5aAttrname);

    hid_t h5a_id = H5Aopen_name(h5f_id, h5aAttrname.c_str());
    if (h5a_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    return hdf5_output_conversion( h5a_id);
  }


  BaseGDL* h5d_get_space_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);

    hid_t h5d_id = hdf5_input_conversion(e,0);

    hid_t h5d_space_id = H5Dget_space( h5d_id );
    if (h5d_space_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    return hdf5_output_conversion( h5d_space_id );
  }

  BaseGDL* h5a_get_space_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);

    hid_t h5a_id=hdf5_input_conversion(e,0);

    hid_t h5a_space_id = H5Aget_space( h5a_id );
    if (h5a_space_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    return hdf5_output_conversion( h5a_space_id );

  }

  BaseGDL* h5a_get_num_attrs_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);

    hid_t loc_id=hdf5_input_conversion(e,0);

    int num = H5Aget_num_attrs( loc_id );
    if (num < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    // following the doc., should return a "int"
    return new DLongGDL( num );
  }


  BaseGDL* h5d_get_type_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);

    hid_t h5d_id=hdf5_input_conversion(e,0);

    hid_t h5d_type_id = H5Dget_type( h5d_id );
    if (h5d_type_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    return hdf5_output_conversion( h5d_type_id );
  }


  BaseGDL* h5t_get_size_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);

    hid_t h5t_id=hdf5_input_conversion(e,0);

    // following the doc., should return a "size_t"
    size_t size = H5Tget_size( h5t_id );
    if (size == 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    return new DLongGDL( size );

  }


  BaseGDL* h5t_array_create_fun( EnvT* e)
  {
    bool debug=false;
    SizeT nParam=e->NParam(2);

    int rank;
    hsize_t dims[MAXRANK];

    /* mandatory 'Datatype_id' parameter */
    hid_t h5t_id=hdf5_input_conversion(e,0);

    /* mandatory 'Dimensions' parameter */
    DUIntGDL* dimPar = e->GetParAs<DUIntGDL>(1);
    SizeT nDim = dimPar->N_Elements();

    if (nDim == 0)
      e->Throw("Variable is undefined: "+ e->GetParString(0));
    else
       rank=nDim;

    for(int i=0; i<rank; i++) dims[i] = (hsize_t)(*dimPar)[rank-1-i];

    /* create the array datatype */
    hid_t dtype_id = H5Tarray_create( h5t_id, rank, dims, NULL );
    if (dtype_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    return hdf5_output_conversion( dtype_id );
  }


  BaseGDL* h5t_idl_create_fun( EnvT* e)
  {
    /* Dec 2021, Oliver Gressel <ogressel@gmail.com>
       - implement rudimentary functionality
         (FIXME: add GDL_STRUCT case + keyword functionality)
    */

    SizeT nParam=e->NParam(1);


    /* --- obtain input parameters --- */

    /* mandatory 'Data' parameter */
    BaseGDL *data = e->GetParDefined(0);

    /* optional 'MEMBER_NAMES' keyword parameter */
    static int memNmIx = e->KeywordIx("MEMBER_NAMES");
    if (e->GetKW(memNmIx)!=NULL) e->Throw("KW 'MEMBER_NAMES' not implemented.");

    /* optional 'OPAQUE' keyword parameter */
    static int opaqueIx = e->KeywordIx("OPAQUE");
    if (e->GetKW(opaqueIx)!=NULL) e->Throw("KW 'OPAQUE' not implemented.");


    /* --- determine HDF5 type and return datatype handle --- */

    hid_t native_type = mapGDLdatatypesToH5( data, e );

    return hdf5_output_conversion( native_type );
  }


  BaseGDL* h5s_get_simple_extent_ndims_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);

    hid_t h5s_id=hdf5_input_conversion(e,0);

    int rank = H5Sget_simple_extent_ndims(h5s_id);
    if (rank < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    return new DLongGDL(rank);
  }


  BaseGDL* h5s_get_simple_extent_dims_fun( EnvT* e)
  {
    SizeT nParam=e->NParam(1);
    hsize_t dims_out[MAXRANK];

    hid_t h5s_id=hdf5_input_conversion(e,0);

    int rank = H5Sget_simple_extent_ndims(h5s_id);
    if (rank < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    if (H5Sget_simple_extent_dims(h5s_id, dims_out, NULL) < 0)
      { string msg; e->Throw(hdf5_error_message(msg)); }

    if(rank == 0) return new DLongGDL(rank);

    dimension dim(rank);
    DLongGDL* d = new DLongGDL(dim);

    for(int i=0; i<rank; i++)
      (*d)[i] = dims_out[rank - 1 - i];

    return d;
  }


  BaseGDL* h5s_create_scalar_fun( EnvT* e)
  {
    /* create a scalar dataspace */
    hid_t space_id = H5Screate(H5S_SCALAR);
    if (space_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    return hdf5_output_conversion( space_id );
  }


  BaseGDL* h5s_create_simple_fun( EnvT* e)
  {
    bool debug=false;
    SizeT nParam=e->NParam(1);

    int rank;
    hsize_t curr_dims[MAXRANK], max_dims[MAXRANK];
    hsize_t *p_max_dims=&max_dims[0];

    /* mandatory 'Dimensions' parameter */
    DUIntGDL* dimPar = e->GetParAs<DUIntGDL>(0);
    SizeT nDim = dimPar->N_Elements();

    if (nDim == 0)
      e->Throw("Variable is undefined: "+ e->GetParString(0));
    else
       rank=nDim;

    if (debug) printf("dataspace rank=%d\n", rank);

    for(int i=0; i<rank; i++) curr_dims[i] = (hsize_t)(*dimPar)[rank-1-i];

    /* keyword 'max_dimensions' paramter */
    static int maxDimIx = e->KeywordIx("MAX_DIMENSIONS");
    if (e->GetKW(maxDimIx) != NULL) {

      DIntGDL* maxDimKW = e->IfDefGetKWAs<DIntGDL>(maxDimIx);
      SizeT nMaxDim = maxDimKW->N_Elements();

      if (nMaxDim == 0)
        e->Throw("Variable is undefined: "+ e->GetParString(maxDimIx));
      else if(nMaxDim != rank)
        e->Throw("Number of elements in MAX_DIMENSIONS must equal dataspace dimensions.");

      for(int i=0; i<rank; i++) {

         max_dims[i] = (hsize_t)(*maxDimKW)[rank-1-i];

         if ( max_dims[i]>0 && max_dims[i]<curr_dims[i] )
            e->Throw("H5S_CREATE_SIMPLE: maxdims is smaller than dims");
      }

    } else
       p_max_dims=NULL;

    /* debug output */
    if (debug) {
      const hsize_t *par[2] = { curr_dims, max_dims };
      const char *parnm[2] = { "curr_dims", "max_dims" };

      for(int i=0; i<1+(p_max_dims!=NULL); i++) {
        printf(" %s=[",parnm[i]);
        for(int j=0; j<rank; j++)
          printf("%lld%s",par[i][j], (j==rank-1) ? "];" : ",");
      }
      printf("\n");
    }

    /* create the simple dataspace */
    hid_t space_id = H5Screate_simple( rank, curr_dims, p_max_dims );
    if (space_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    return hdf5_output_conversion( space_id );
  }


  void h5s_select_hyperslab_pro( EnvT* e )
  {
    bool debug=false;
    SizeT nParam=e->NParam(3);
    hsize_t start[MAXRANK], count[MAXRANK], block[MAXRANK], stride[MAXRANK];


    /* mandatory 'Dataspace_id' parameter */
    hid_t h5s_id=hdf5_input_conversion(e,0);
    int rank=H5Sget_simple_extent_ndims(h5s_id);

    if (H5Iis_valid(h5s_id) <= 0)
      e->Throw("not a dataspace: Object ID:" + i2s( h5s_id ));

    if (debug) printf("dataspace rank=%d;", rank);


    /* mandatory 'Start' parameter */
    DUIntGDL* startPar = e->GetParAs<DUIntGDL>(1);
    SizeT nStart = startPar->N_Elements();

    if (nStart == 0)
      e->Throw("Variable is undefined: "+ e->GetParString(1));
    else if (nStart != rank)
      e->Throw("Number of elements in Start must equal dataspace dimensions. ");

    for(int i=0; i<rank; i++) start[i] = (hsize_t)(*startPar)[rank-1-i];


    /* mandatory 'Count' parameter */
    DUIntGDL* countPar = e->GetParAs<DUIntGDL>(2);
    SizeT nCount = countPar->N_Elements();

    if (nCount == 0)
      e->Throw("Variable is undefined: "+ e->GetParString(2));
    else if (nCount != rank)
      e->Throw("Number of elements in Count must equal dataspace dimensions. ");

    for(int i=0; i<rank; i++) count[i] = (hsize_t)(*countPar)[rank-1-i];


    /* keyword 'block' parameter */
    static int blockIx = e->KeywordIx("BLOCK");
    if (e->GetKW(blockIx) != NULL) {

      DUIntGDL* blockKW = e->IfDefGetKWAs<DUIntGDL>(blockIx);
      SizeT nBlock = blockKW->N_Elements();

      if (nBlock == 0)
        e->Throw("Variable is undefined: "+ e->GetParString(blockIx));
      else if (nBlock != rank)
        e->Throw("Number of elements in BLOCK must equal dataspace dimensions. ");

      for(int i=0; i<rank; i++) block[i] = (hsize_t)(*blockKW)[rank-1-i];

    } else
      for(int i=0; i<rank; i++) block[i] = 1;


    /* keyword 'reset' parameter */
    static int resetIx = e->KeywordIx("RESET");
    bool reset = e->KeywordSet(resetIx);


    /* keyword 'stride' parameter */
    static int strideIx = e->KeywordIx("STRIDE");
    if (e->GetKW(strideIx) != NULL) {

      DUIntGDL* strideKW = e->IfDefGetKWAs<DUIntGDL>(strideIx);
      SizeT nStride = strideKW->N_Elements();

      if (nStride == 0)
        e->Throw("Variable is undefined: "+ e->GetParString(strideIx));
      else if (nStride != rank)
        e->Throw("Number of elements in STRIDE must equal dataspace dimensions. ");

      for(int i=0; i<rank; i++) stride[i] = (hsize_t)(*strideKW)[rank-1-i];

    } else
      for(int i=0; i<rank; i++) stride[i] = 1;


    /* debug output */
    if (debug) {
      const hsize_t *par[4] = { start, count, block, stride };
      const char *parnm[4] = { "start", "count", "block", "stride" };

      for(int i=0; i<4; i++) {
        printf(" %s=[",parnm[i]);
        for(int j=0; j<rank; j++)
          printf("%lld%s",par[i][j], (j==rank-1) ? "];" : ",");
      }
      printf("\n");
    }


    /* call library function */
    if (H5Sselect_hyperslab( h5s_id,
                             (reset) ? H5S_SELECT_SET : H5S_SELECT_OR,
                             start, stride, count, block )  < 0)
      { string msg; e->Throw(hdf5_error_message(msg)); }

    /* FIXME -- implement this special behaviour? (from IDL documentation)
      "Note: If all of the elements in the selected hyperslab region are
       already selected, then a new hyperslab region is not created." */

    return;
  }


  BaseGDL* h5a_read_fun( EnvT* e)
  {

     /* Jun 2021, Oliver Gressel <ogressel@gmail.com>
        - add support for attributes of type 'H5T_ARRAY'

        Nov 2021, Oliver Gressel <ogressel@gmail.com>
        - use 'hdf5_unified_read()' function, enabling 'H5T_COMPOUND' support
     */

    bool debug=false;

    SizeT nParam=e->NParam(1);
    hsize_t dims_out[MAXRANK];
    hsize_t elem_dims[MAXRANK];

    hid_t h5a_id = hdf5_input_conversion(e,0);

    hid_t h5s_id = H5Aget_space(h5a_id);

    if (h5s_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
    hdf5_space_guard h5s_id_guard = hdf5_space_guard(h5s_id);

    /* --- read the dataset into GDL --- */

    BaseGDL* res = hdf5_unified_read( h5a_id, h5s_id, H5I_BADID, e );

    return res;
  }


  /**
   * h5d_read_fun
   * CAUTION: compatibility only fractional
   */
  BaseGDL* h5d_read_fun(EnvT* e) {

    /* Jul 2021, Oliver Gressel <ogressel@gmail.com>
       - add support for datasets of type 'H5T_ARRAY'

       Oct 2021, Oliver Gressel <ogressel@gmail.com>
       - allow for hyperslab selection via passing keyword parameters
         'FILE_SPACE' and 'MEMORY_SPACE', respectively

       Oct 2021, Oliver Gressel <ogressel@gmail.com>
       - allow for datasets of type 'H5T_COMPOUND' mapping to GDL structures
         (FIXME: ignores the IDL keyword 'Datatype_id' meant for partial reads)
    */

    bool debug = false;

    SizeT nParam = e->NParam(1);
    hsize_t dims_out[MAXRANK];
    hsize_t elem_dims[MAXRANK];

    hid_t h5d_id = hdf5_input_conversion(e,0);


    /* --- keyword 'FILE_SPACE' --- */

    hid_t kw_filespace_id, filespace_id;

    static int filespaceIx = e->KeywordIx("FILE_SPACE");

    if(e->KeywordSet(filespaceIx)) {    /* use keyword parameter */

       if (debug) printf("using keyword 'file_space'\n");
       kw_filespace_id = hdf5_input_conversion_kw(e,filespaceIx);

       if (H5Iis_valid(kw_filespace_id) <= 0)
          e->Throw("not a dataspace: Object ID:" + i2s( kw_filespace_id ));
       else
          filespace_id = H5Scopy(kw_filespace_id);

    } else {                            /* obtain from dataset */

       filespace_id = H5Dget_space(h5d_id);
       if (filespace_id < 0) {
          string msg;
          e->Throw(hdf5_error_message(msg));
       }
    }

    hdf5_space_guard filespace_id_guard = hdf5_space_guard(filespace_id);


    /* --- keyword 'MEMORY_SPACE' --- */

    hid_t kw_memspace_id, memspace_id;

    static int memspaceIx = e->KeywordIx("MEMORY_SPACE");

    if(e->KeywordSet(memspaceIx)) {     /* use keyword parameter */

       if (debug) printf("using keyword 'memory_space'\n");
       kw_memspace_id = hdf5_input_conversion_kw(e,memspaceIx);

       if (H5Iis_valid(kw_memspace_id) <= 0)
          e->Throw("not a dataspace: Object ID:" + i2s( kw_memspace_id ));
       else
          memspace_id = H5Scopy(kw_memspace_id);

    } else {                            /* same as file space */

       memspace_id = H5Scopy(filespace_id);
       if (memspace_id < 0) {
          string msg;
          e->Throw(hdf5_error_message(msg));
       }
    }

    hdf5_space_guard memspace_id_guard = hdf5_space_guard(memspace_id);


    /* --- read the dataset into GDL --- */

    BaseGDL* res = hdf5_unified_read( h5d_id, memspace_id, filespace_id, e );

    return res;
  }


  void h5d_write_pro(EnvT* e) {

    /* Dec 2021, Oliver Gressel <ogressel@gmail.com>
       - implement very basic support for writing HDF5 datasets
    */

    bool debug=false;

    SizeT nParam = e->NParam(2);

    hid_t dset_id = hdf5_input_conversion(e,0);
    BaseGDL* data = e->GetParDefined(1);


    /* --- optionial keyword 'MEMORY_SPACE' --- */

    hid_t kw_memspace_id, memspace_id;
    static int memspaceIx = e->KeywordIx("MEMORY_SPACE");

    if(e->KeywordSet(memspaceIx)) {     /* use keyword parameter */

      if (debug) printf("using keyword 'memory_space'\n");
      kw_memspace_id = hdf5_input_conversion_kw(e,memspaceIx);

      if (H5Iis_valid(kw_memspace_id) <= 0)
        e->Throw("not a dataspace: Object ID:" + i2s( kw_memspace_id ));
      else
        memspace_id = H5Scopy(kw_memspace_id);

    } else memspace_id = H5S_ALL;

    hdf5_space_guard memspace_id_guard = hdf5_space_guard(memspace_id);


    /* --- optional keyword 'FILE_SPACE' --- */

    hid_t kw_filespace_id, filespace_id;
    static int filespaceIx = e->KeywordIx("FILE_SPACE");

    if(e->KeywordSet(filespaceIx)) {    /* use keyword parameter */

      if (debug) printf("using keyword 'file_space'\n");
      kw_filespace_id = hdf5_input_conversion_kw(e,filespaceIx);

      if (H5Iis_valid(kw_filespace_id) <= 0)
        e->Throw("not a dataspace: Object ID:" + i2s( kw_filespace_id ));
      else
        filespace_id = H5Scopy(kw_filespace_id);

    } else filespace_id = H5S_ALL;

    hdf5_space_guard filespace_id_guard = hdf5_space_guard(filespace_id);


    /* --- write the dataset to file ---*/

    hdf5_unified_write( dset_id, data, memspace_id, filespace_id, e );

    return;
  }


  void h5s_close_pro( EnvT* e)
  {
    SizeT nParam=e->NParam(1);

    hid_t h5s_id = hdf5_input_conversion(e,0);

    if (H5Sclose(h5s_id) < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
  }


  BaseGDL* h5d_create_fun( EnvT* e)
  {
    /* Nov 2021, Oliver Gressel <ogressel@gmail.com>
       - implement rudimentary functionality
       - FIXME: add optional keyword parameters [, GZIP=value [, /SHUFFLE]]
    */

    SizeT nParam=e->NParam(4);


    /* --- dataset creation property list --- */

    hid_t plist_id = H5Pcreate(H5P_DATASET_CREATE);
    hdf5_plist_guard plist_guard = hdf5_plist_guard(plist_id);


    /* --- mandatory parameters --- */

    /* 'Loc_id' parameter */
    hid_t loc_id = hdf5_input_conversion(e,0);

    /* 'Name' parameter */
    DString dset_name;
    e->AssureScalarPar<DStringGDL>(1,dset_name);

    /* 'Datatype_id' parameter */
    hid_t type_id = hdf5_input_conversion(e,2);
    if (H5Iis_valid(type_id) <= 0)
      e->Throw("not a datatype: Object ID:" + i2s( type_id ));

    /* 'Dataspace_id' parameter */
    hid_t space_id = hdf5_input_conversion(e,3);
    if (H5Iis_valid(space_id) <= 0)
      e->Throw("not a dataspace: Object ID:" + i2s( space_id ));


    /* --- optional keyword 'CHUNK_DIMENSIONS' paramter --- */

    static int chunkDimIx = e->KeywordIx("CHUNK_DIMENSIONS");
    if (e->GetKW(chunkDimIx) != NULL) {

      DUIntGDL* chunkDimKW = e->IfDefGetKWAs<DUIntGDL>(chunkDimIx);
      SizeT nChunkDim = chunkDimKW->N_Elements();

      int rank;
      hsize_t dims[MAXRANK], chunk_dims[MAXRANK];

      if ( (rank = H5Sget_simple_extent_ndims(space_id)) < 0 )
        { string msg; e->Throw(hdf5_error_message(msg)); }

      if (nChunkDim == 0)
        e->Throw("Variable is undefined: "+ e->GetParString(chunkDimIx));
      else if(nChunkDim != rank)
        e->Throw("Number of elements in CHUNK_DIMENSIONS must equal dataspace.");

      if ( H5Sget_simple_extent_dims(space_id, dims, NULL) < 0 )
        { string msg; e->Throw(hdf5_error_message(msg)); }

      for(int i=0; i<rank; i++) {
        chunk_dims[i] = (hsize_t)(*chunkDimKW)[rank-1-i];

        if (chunk_dims[i]>dims[i])
          e->Throw("CHUNK_DIMENSION["+i2s(rank-1-i)+"] exceeds dimension");
      }

      H5Pset_chunk(plist_id, rank, chunk_dims);
    }


    /* --- create the dataset identifier --- */

    hid_t h5d_id = H5Dcreate( loc_id, dset_name.c_str(),
                              type_id, space_id, plist_id );
    if (h5d_id < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    return hdf5_output_conversion( h5d_id );
  }


  void h5d_extend_pro( EnvT* e)
  {
    SizeT nParam=e->NParam(2);

    int rank, curr_rank;
    hsize_t size[MAXRANK], curr_size[MAXRANK];

    /* mandatory 'Dataset_id' parameter */
    hid_t h5d_id = hdf5_input_conversion(e,0);

    /* mandatory 'Size' parameter */
    DUIntGDL* sizePar = e->GetParAs<DUIntGDL>(1);
    SizeT nDim = sizePar->N_Elements();

    if (nDim == 0)
      e->Throw("Variable is undefined: "+ e->GetParString(1));
    else
      rank=nDim;

    /* reverse dimensions */
    for(int i=0; i<rank; i++) size[i] = (hsize_t)(*sizePar)[rank-1-i];

    /* extend the dataset dimensions */
    if( H5Dset_extent( h5d_id, size ) < 0 )
      e->Throw("unable to extend dataset: Object ID:"+i2s(h5d_id));

    return;
  }


  BaseGDL* h5d_get_storage_size_fun( EnvT* e)
  {
    SizeT nParam = e->NParam(1);
    hid_t dset_id = hdf5_input_conversion(e,0);

    hsize_t storage_size = H5Dget_storage_size( dset_id );
    if (storage_size < 0) { string msg; e->Throw(hdf5_error_message(msg)); }

    return new DULong64GDL( storage_size );
  }


  void h5d_close_pro( EnvT* e)
  {
    SizeT nParam=e->NParam(1);

    hid_t h5d_id = hdf5_input_conversion(e,0);

    if (H5Dclose(h5d_id) < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
  }


  void h5f_close_pro( EnvT* e)
  {
    SizeT nParam=e->NParam(1);

    hid_t h5f_id = hdf5_input_conversion(e,0);

    if (H5Fclose(h5f_id) < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
  }


  void h5t_close_pro( EnvT* e)
  {
    SizeT nParam=e->NParam(1);

    hid_t h5t_id = hdf5_input_conversion(e,0);

    if (H5Tclose(h5t_id) < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
  }


  void h5g_close_pro( EnvT* e)
  {
    SizeT nParam=e->NParam(1);

    hid_t h5g_id = hdf5_input_conversion(e,0);

    if (H5Gclose(h5g_id) < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
  }


  void h5a_close_pro( EnvT* e)
  {
    SizeT nParam=e->NParam(1);

    hid_t h5a_id = hdf5_input_conversion(e,0);

    if (H5Aclose(h5a_id) < 0) { string msg; e->Throw(hdf5_error_message(msg)); }
  }


  BaseGDL* h5i_get_file_id_fun( EnvT* e)
  {
    /* Dec 2022, Oliver Gressel <ogressel@gmail.com>
    */
    SizeT nParam=e->NParam(1);

    /* mandatory 'Loc_id' parameter */
    hid_t loc_id = hdf5_input_conversion(e, 0);

    /* query the file ID */
    hid_t h5f_id;
    if ( (h5f_id = H5Iget_file_id(loc_id))  < 0 )
      { string msg; e->Throw(hdf5_error_message(msg)); }

    return hdf5_output_conversion( h5f_id );
  }


  BaseGDL* h5i_get_type_fun( EnvT* e)
  {
    /* Dec 2022, Oliver Gressel <ogressel@gmail.com>
    */
    SizeT nParam=e->NParam(1);

    /* mandatory 'Loc_id' parameter */
    hid_t loc_id = hdf5_input_conversion(e, 0);

    /* query the identifier type */
    switch( H5Iget_type(loc_id) ) {

    case H5I_FILE:      return new DStringGDL("FILE");      break;
    case H5I_GROUP:     return new DStringGDL("GROUP");     break;
    case H5I_DATATYPE:  return new DStringGDL("DATATYPE");  break;
    case H5I_DATASPACE: return new DStringGDL("DATASPACE"); break;
    case H5I_DATASET:   return new DStringGDL("DATASET");   break;
    case H5I_ATTR:      return new DStringGDL("ATTR");      break;
    case H5I_BADID:     return new DStringGDL("BADID");     break;
    default: e->Throw("unsupported H5I type\n");
    }

    return NULL;
  }

} // namespace

#endif
