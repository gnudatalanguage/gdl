common GDLmapProjectionCommon, proj,proj_properties,required,optional,compressed_ids1,compressed_ids2,dictionary,yranoitcid,ellipsoid_idl,ellipsoid_proj,idl_ids,idl_equiv,proj_scale,proj_limits

  if (~N_ELEMENTS(proj) && $; common is not initialised
      ~keyword_set(doInitMapProjectionCommon)) $ ; and we are not initializing it
  then  gdl_map_proj_init_common ; call the procedure for initialization.
