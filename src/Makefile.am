# set the include path found by configure
INCLUDES = $(EXT_INCLUDES) $(all_includes)
SUBDIRS = antlr pro

cpp_hpp_files = allix.hpp allix.cpp prognode.hpp prognodeexpr.hpp \
		accessdesc.hpp arrayindex.hpp assocdata.cpp assocdata.hpp \
		basegdl.hpp basic_fun_cl.cpp basic_fun_cl.hpp basic_fun.cpp basic_fun.hpp \
		basic_fun_jmg.cpp basic_fun_jmg.hpp basic_op.cpp basic_op_new.cpp basic_pro.cpp basic_pro.hpp \
		basic_pro_jmg.cpp basic_pro_jmg.hpp CFMTLexer.cpp CFMTLexer.hpp CFMTTokenTypes.hpp \
		cformat.g color.cpp color.hpp convert2.cpp datalistt.hpp datatypes.cpp \
		datatypes.hpp dcommon.cpp dcommon.hpp dcompiler.cpp dcompiler.hpp default_io.cpp \
		deviceps.hpp devicesvg.hpp devicewin.hpp devicex.hpp dimension.hpp dinterpreter.cpp \
		dinterpreter.hpp dnode.cpp dnodefactory.hpp dnode.hpp dpro.cpp dpro.hpp \
		dstructdesc.cpp \
		dstructdesc.hpp dstructgdl.cpp dstructgdl.hpp dvar.cpp dvar.hpp envt.cpp envt.hpp \
		extrat.cpp extrat.hpp FMTIn.cpp FMTIn.hpp FMTInTokenTypes.hpp FMTLexer.cpp \
		FMTLexer.hpp fmtnode.cpp fmtnode.hpp FMTOut.cpp FMTOut.hpp FMTOutTokenTypes.hpp \
		FMTParser.cpp FMTParser.hpp FMTTokenTypes.hpp format.g format.in.g format.out.g \
		gdlc.g gdlc.i.g gdlc.tree.g gdleventhandler.cpp gdleventhandler.hpp \
		gdlexception.cpp gdlexception.hpp gdlgstream.cpp gdlgstream.hpp \
		GDLInterpreter.cpp  \
		GDLInterpreter.hpp GDLInterpreterTokenTypes.hpp GDLLexer.cpp \
		GDLLexer.hpp GDLParser.cpp GDLParser.hpp gdlpsstream.cpp gdlpsstream.hpp \
		gdlsvgstream.cpp gdlsvgstream.hpp gdlpython.cpp gdlpython.hpp \
		GDLTokenTypes.hpp GDLTreeParser.cpp GDLTreeParser.hpp GDLTreeParserTokenTypes.hpp \
		gdlwinstream.cpp gdlwinstream.hpp gdlxstream.cpp gdlxstream.hpp getfmtast.cpp \
		getfmtast.hpp graphics.cpp graphics.hpp gsl_fun.cpp gsl_fun.hpp \
		gstream.hpp ifmt.cpp \
		initct.cpp initsysvar.cpp initsysvar.hpp io.cpp io.hpp libinit_cl.cpp libinit.cpp \
		libinit_jmg.cpp \
		math_fun.cpp math_fun.hpp math_fun_jmg.cpp \
		math_fun_jmg.hpp math_utl.cpp math_utl.hpp ncdf_att_cl.cpp ncdf_cl.cpp ncdf_cl.hpp \
		ncdf_dim_cl.cpp ncdf_var_cl.cpp new.cpp new.hpp objects.cpp objects.hpp ofmt.cpp \
		math_fun_ac.hpp math_fun_ac.cpp libinit_ac.cpp \
                math_fun_gm.hpp math_fun_gm.cpp libinit_gm.cpp \
		math_fun_ng.hpp math_fun_ng.cpp libinit_ng.cpp \
		ofmt.hpp plotting.cpp plotting.hpp print.cpp print_tree.cpp print_tree.hpp \
		read.cpp real2int.hpp str.cpp str.hpp terminfo.cpp terminfo.hpp topython.cpp \
		typedefs.hpp typetraits.cpp typetraits.hpp hdf_fun.cpp hdf_fun.hpp hdf_pro.cpp \
		hdf_pro.hpp magick_cl.cpp magick_cl.hpp specializations.hpp gdlwidget.cpp \
		widget.cpp basegdl.cpp hdf5_fun.cpp hdf5_fun.hpp libinit_mes.cpp file.cpp file.hpp \
		image.cpp image.hpp gdljournal.cpp gdljournal.hpp convol.cpp convol_inc0.cpp \
		convol_inc1.cpp convol_inc2.cpp sigfpehandler.cpp sigfpehandler.hpp devicez.hpp \
		gdlzstream.cpp gdlzstream.hpp gdlwidget.hpp arrayindexlistt.hpp arrayindex.cpp \
		fftw.cpp fftw.hpp mpi.cpp mpi.hpp plot3d_nr.cpp grib.cpp grib.hpp \
                prognode.cpp prognodeexpr.cpp strassenmatrix.hpp datatypesref.cpp lapack.cpp \
                gshhs.cpp gshhs.hpp newprognode.cpp plotting_plot.cpp plotting_oplot.cpp \
                plotting_device.cpp plotting_cursor.cpp plotting_contour.cpp plotting_surface.cpp \
                plotting_axis.cpp plotting_plots.cpp plotting_xyouts.cpp plotting_polyfill.cpp \
                plotting_windows.cpp plotting_convert_coord.cpp plotting_map_proj.cpp plotting_misc.cpp \
                plotting_erase.cpp semshm.cpp
# Makefile.am Makefile.in 



# standalone program
if !BUILDASLIBRARY
bin_PROGRAMS = gdl
gdl_SOURCES = $(cpp_hpp_files) gdl.cpp lapack.cpp
# trigger basename generation
gdl_CXXFLAGS = $(AM_CXXFLAGS)
# the library search path.
gdl_LDFLAGS = $(all_libraries)
gdl_LDADD = $(top_builddir)/src/antlr/libantlr.a -L/usr/X11R6/lib64 -L/usr/X11R6/lib -lX11
endif

# build as library (for python module)
if BUILDASLIBRARY
noinst_LTLIBRARIES = libgdl.la
libgdl_la_SOURCES = $(cpp_hpp_files) pythongdl.cpp

# netCDF and HDF define some same symbols
# hence -z muldefs has to be passed to the linker
if DARWIN
libgdl_la_LDFLAGS = $(all_libraries) -Wl,-m
else
libgdl_la_LDFLAGS = $(all_libraries) -Wl,-z,muldefs
endif 

libgdl_la_LIBADD = $(top_builddir)/src/antlr/libantlr.la -L/usr/X11R6/lib64 -L/usr/X11R6/lib -lX11
endif

