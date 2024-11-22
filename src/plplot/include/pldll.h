#ifndef __PL_DLL_H
#define __PL_DLL_H

// In some cases, this header is #included without #including
// plplot_config.h which normally #defines ENABLE_DYNDRIVERS (or not).
#ifndef ENABLE_DYNDRIVERS
// Define if there is support for dynamically loaded drivers
/* #undef ENABLE_DYNDRIVERS */
#endif


#ifdef USINGDLL
  #if defined ( _WIN32 )
// Visual C/C++, Borland, MinGW and Watcom
    #if defined ( __VISUALC__ ) || defined ( _MSC_VER ) || defined ( __BORLANDC__ ) || defined ( __GNUC__ ) || defined ( __WATCOMC__ )
      #define PLDLLEXPORT    __declspec( dllexport )
      #define PLDLLIMPORT    __declspec( dllimport )
    #else
      #define PLDLLEXPORT
      #define PLDLLIMPORT
    #endif
  #elif defined ( __CYGWIN__ )
    #define PLDLLEXPORT    __declspec( dllexport )
    #define PLDLLIMPORT    __declspec( dllimport )
  #elif defined ( __GNUC__ ) && __GNUC__ > 3
// Follow ideas in http://gcc.gnu.org/wiki/Visibility for GCC version 4.x
// The following forces exported symbols specifically designated with
// PLDLLEXPORT to be visible.
    #define PLDLLEXPORT    __attribute__ ( ( visibility( "default" ) ) )
    #define PLDLLIMPORT
  #endif
#endif

// For an unknown compiler or static built we clear the macros
#ifndef PLDLLEXPORT
  #define PLDLLEXPORT
  #define PLDLLIMPORT
#endif

// The IMPEXP macros will always be set to DLLIMPORT (even for
// the static library, but DLLIMPORT is empty in this case), if
// cmake didn't set the corresponding macro xxxx_EXPORTS when the
// corresponding library is built (DLLIMPEXP is set to DLLEXPORT
// then)
#if defined ( plplot_EXPORTS )
  #define PLDLLIMPEXP    PLDLLEXPORT
  #define PLDLLIMPEXP_DATA( type )    PLDLLEXPORT type
#else
  #define PLDLLIMPEXP    PLDLLIMPORT
  #define PLDLLIMPEXP_DATA( type )    PLDLLIMPORT type
#endif

// for dynamic drivers set the macros correctly. If a shared library is built,
// but dyanmic drivers disabled, the driver dll macros are the same as the
// plplot dll macros
#ifdef ENABLE_DYNDRIVERS
  #if defined ( aqt_EXPORTS ) ||     \
    defined ( cairo_EXPORTS ) ||     \
    defined ( cgm_EXPORTS ) ||       \
    defined ( gd_EXPORTS ) ||        \
    defined ( mem_EXPORTS ) ||       \
    defined ( ntk_EXPORTS ) ||       \
    defined ( null_EXPORTS ) ||      \
    defined ( pdf_EXPORTS ) ||       \
    defined ( plmeta_EXPORTS ) ||    \
    defined ( ps_EXPORTS ) ||        \
    defined ( pstex_EXPORTS ) ||     \
    defined ( psttf_EXPORTS ) ||     \
    defined ( svg_EXPORTS ) ||       \
    defined ( tk_EXPORTS ) ||        \
    defined ( tkwin_EXPORTS ) ||     \
    defined ( wingcc_EXPORTS ) ||    \
    defined ( wxwidgets_EXPORTS ) || \
    defined ( xfig_EXPORTS ) ||      \
    defined ( xwin_EXPORTS ) ||      \
    defined ( qt_EXPORTS ) ||        \
    defined ( wingdi_EXPORTS )
    #define PLDLLIMPEXP_DRIVER    PLDLLEXPORT
    #define PLDLLIMPEXP_DRIVER_DATA( type )    PLDLLEXPORT type
  #else
    #define PLDLLIMPEXP_DRIVER    PLDLLIMPORT
    #define PLDLLIMPEXP_DRIVER_DATA( type )    PLDLLIMPORT type
  #endif
#else
  #define PLDLLIMPEXP_DRIVER    PLDLLIMPEXP
  #define PLDLLIMPEXP_DRIVER_DATA( type )    PLDLLIMPEXP_DATA( type )
#endif

#if defined ( plplotcxx_EXPORTS )
  #define PLDLLIMPEXP_CXX    PLDLLEXPORT
  #define PLDLLIMPEXP_CXX_DATA( type )    PLDLLEXPORT type
#else
  #define PLDLLIMPEXP_CXX    PLDLLIMPORT
  #define PLDLLIMPEXP_CXX_DATA( type )    PLDLLIMPORT type
#endif

#if defined ( plplotfortranc_EXPORTS )
  #define PLDLLIMPEXP_FORTRANC    PLDLLEXPORT
  #define PLDLLIMPEXP_FORTRANC_DATA( type )    PLDLLEXPORT type
#else
  #define PLDLLIMPEXP_FORTRANC    PLDLLIMPORT
  #define PLDLLIMPEXP_FORTRANC_DATA( type )    PLDLLIMPORT type
#endif

#if defined ( plplotwxwidgets_EXPORTS )
  #define PLDLLIMPEXP_WX    PLDLLEXPORT
  #define PLDLLIMPEXP_WX_DATA( type )    PLDLLEXPORT type
#else
  #define PLDLLIMPEXP_WX    PLDLLIMPORT
  #define PLDLLIMPEXP_WX_DATA( type )    PLDLLIMPORT type
#endif

// Note for the case when ENABLE_DYNDRIVERS is not defined, the
// libplplot build specifically includes the tclmatrix code (see
// cmake/modules/tk.cmake).
#if defined ( tclmatrix_EXPORTS ) || \
    ( !defined ( ENABLE_DYNDRIVERS ) && defined ( plplot_EXPORTS ) )
  #define PLDLLIMPEXP_TCLMAT    PLDLLEXPORT
  #define PLDLLIMPEXP_TCLMAT_DATA( type )    PLDLLEXPORT type
#else
  #define PLDLLIMPEXP_TCLMAT    PLDLLIMPORT
  #define PLDLLIMPEXP_TCLMAT_DATA( type )    PLDLLIMPORT type
#endif

// Note for the case when ENABLE_DYNDRIVERS is not defined, the
// libplplot build specifically includes the plplottcltk code (see
// cmake/modules/tk.cmake).
#if defined ( plplottcltk_Main_EXPORTS ) || \
    defined ( plplottcltk_EXPORTS ) ||      \
    ( !defined ( ENABLE_DYNDRIVERS ) && defined ( plplot_EXPORTS ) )
  #define PLDLLIMPEXP_TCLTK    PLDLLEXPORT
  #define PLDLLIMPEXP_TCLTK_DATA( type )    PLDLLEXPORT type
#else
  #define PLDLLIMPEXP_TCLTK    PLDLLIMPORT
  #define PLDLLIMPEXP_TCLTK_DATA( type )    PLDLLIMPORT type
#endif

// Note for the case when ENABLE_DYNDRIVERS is not defined, the
// libplplot build specifically includes bindings/qt-gui/plqt.cpp (see
// cmake/modules/qt.cmake).
#if defined ( plplotqt_EXPORTS ) || \
    ( !defined ( ENABLE_DYNDRIVERS ) && defined ( plplot_EXPORTS ) )
  #define PLDLLIMPEXP_QT    PLDLLEXPORT
  #define PLDLLIMPEXP_QT_DATA( type )    PLDLLEXPORT type
#else
  #define PLDLLIMPEXP_QT    PLDLLIMPORT
  #define PLDLLIMPEXP_QT_DATA( type )    PLDLLIMPORT type
#endif

#if defined ( plplot_pyqt4_EXPORTS )
  #define PLDLLIMPEXP_PYQT4    PLDLLEXPORT
  #define PLDLLIMPEXP_PYQT4_DATA( type )    PLDLLEXPORT type
#else
  #define PLDLLIMPEXP_PYQT4    PLDLLIMPORT
  #define PLDLLIMPEXP_PYQT4_DATA( type )    PLDLLIMPORT type
#endif

#if defined ( plplot_pyqt5_EXPORTS )
  #define PLDLLIMPEXP_PYQT5    PLDLLEXPORT
  #define PLDLLIMPEXP_PYQT5_DATA( type )    PLDLLEXPORT type
#else
  #define PLDLLIMPEXP_PYQT5    PLDLLIMPORT
  #define PLDLLIMPEXP_PYQT5_DATA( type )    PLDLLIMPORT type
#endif

#endif // __PL_DLL_H
