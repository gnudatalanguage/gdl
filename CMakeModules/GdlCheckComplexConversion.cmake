function(gdl_check_complex_conversion)
  set(probe "${GDL_SOURCE_DIR}/CMakeModules/check_complex_conversion.cpp")
  set(probe_dir "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/GdlCheckComplexConversion")
  set(workaround "--param=vect-epilogues-nomask=0")

  if(CMAKE_CROSSCOMPILING AND NOT CMAKE_CROSSCOMPILING_EMULATOR)
    try_compile(GDL_CONVERT2_WORKAROUND_COMPILES "${probe_dir}" "${probe}"
                COMPILE_DEFINITIONS "${workaround}")
    if(GDL_CONVERT2_WORKAROUND_COMPILES)
      set_property(SOURCE convert2.cpp APPEND PROPERTY COMPILE_OPTIONS "${workaround}")
      message(STATUS "Cannot run the GCC PR 110762 probe while cross-compiling without an emulator. "
                     "Applying ${workaround} to convert2.cpp as a precaution.")
    else()
      message(WARNING "Cannot run the GCC PR 110762 probe while cross-compiling without an emulator. "
                      "The probe also failed to compile with ${workaround}.")
    endif()
    return()
  endif()

  if(CMAKE_CONFIGURATION_TYPES)
    set(configs ${CMAKE_CONFIGURATION_TYPES})
  elseif(CMAKE_BUILD_TYPE)
    set(configs "${CMAKE_BUILD_TYPE}")
  else()
    set(configs Release) # Top-level default for a fresh single-config build.
  endif()

  foreach(config IN LISTS configs)
    # The same compiler can pass at -O2 and fail at -O3.
    set(CMAKE_TRY_COMPILE_CONFIGURATION "${config}")
    set(label "GCC ${CMAKE_CXX_COMPILER_VERSION} (${config})")
    try_run(GDL_CONVERT2_RESULT GDL_CONVERT2_COMPILED "${probe_dir}" "${probe}")
    if(GDL_CONVERT2_COMPILED AND GDL_CONVERT2_RESULT STREQUAL "0")
      message(STATUS "${label} passed the complex-to-integer FE_INVALID check")
    elseif(GDL_CONVERT2_COMPILED AND GDL_CONVERT2_RESULT STREQUAL "1")
      try_run(GDL_CONVERT2_FIXED_RESULT GDL_CONVERT2_FIXED_COMPILED
              "${probe_dir}" "${probe}" COMPILE_DEFINITIONS "${workaround}")
      if(GDL_CONVERT2_FIXED_COMPILED AND GDL_CONVERT2_FIXED_RESULT STREQUAL "0")
        set_property(SOURCE convert2.cpp APPEND PROPERTY COMPILE_OPTIONS
                     "$<$<CONFIG:${config}>:${workaround}>")
        message(WARNING "${label} raises spurious FE_INVALID on complex-to-integer conversion "
                        "(GCC PR 110762). Applying ${workaround} to convert2.cpp.")
      else()
        message(WARNING "${label} failed the complex-to-integer FE_INVALID check (GCC PR 110762). "
                        "The probe did not pass with ${workaround}.")
      endif()
    else()
      message(WARNING "Could not check ${label} for GCC PR 110762 "
                      "(compiled=${GDL_CONVERT2_COMPILED}, exit=${GDL_CONVERT2_RESULT}).")
    endif()
  endforeach()
endfunction()
