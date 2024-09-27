# Version data that need review and possible modification for each release.

set(PLPLOT_VERSION_MAJOR 5)
set(PLPLOT_VERSION_MINOR 15)
set(PLPLOT_VERSION_PATCH 0)

# Overall PLplot version number.
set(PLPLOT_VERSION ${PLPLOT_VERSION_MAJOR}.${PLPLOT_VERSION_MINOR}.${PLPLOT_VERSION_PATCH})

# # CPack version numbers for release tarball name.
# set(CPACK_PACKAGE_VERSION_MAJOR ${PLPLOT_VERSION_MAJOR})
# set(CPACK_PACKAGE_VERSION_MINOR ${PLPLOT_VERSION_MINOR})
# set(CPACK_PACKAGE_VERSION_PATCH ${PLPLOT_VERSION_PATCH})
# 
# # PLplot library version information.
# 
# # Use git log and git diff with appropriate limits on
# # the list of revisions and in appropriate directories for
# # each of the libraries below to determine an overview of the source code
# # changes have been made in the current release cycle.
# 
# # Then use that information and the following semantic versioning rules
# # (see <https://semver.org/> for details) to update the PLplot
# # library version information:
# # (1) If a backwards incompatible API change has been made in the library
# #     API (e.g., if old compiled and linked applications will no longer work)
# #     then increment SOVERSION and zero the corresponding minor and patch
# #     numbers just before release.
# # (2) If the library changes are limited to additions to the API, then
# #     then leave SOVERSION alone, increment the minor number and zero the
# #     patch number just before release.
# # (3) If the library changes are limited to implementation changes with
# #     no API changes at all, then leave SOVERSION and minor number alone, and
# #     increment the patch number just before the release.
# # (4) If there are no library source code changes at all, then leave all
# #     library version numbers the same for the release.
# 
# # N.B. all these variables must include the exact library name
# # so that the configure_library_build function works correctly.
# 
# # Libraries with source code in the lib subdirectory tree.
# set(nistcd_SOVERSION 0)
# set(nistcd_VERSION ${nistcd_SOVERSION}.0.1)
# 
# set(csirocsa_SOVERSION 0)
# set(csirocsa_VERSION ${csirocsa_SOVERSION}.0.1)
# 
# set(csironn_SOVERSION 0)
# set(csironn_VERSION ${csironn_SOVERSION}.0.2)
# 
# set(qsastime_SOVERSION 0)
# set(qsastime_VERSION ${qsastime_SOVERSION}.0.1)
# 
# Library with source code in the src subdirectory and headers in the include subdirectory.
# 
# set(plplot_SOVERSION 17)
# set(plplot_VERSION ${plplot_SOVERSION}.0.0)
# 
# # Libraries with source code in the bindings subdirectory tree.
# 
# set(plplotada_SOVERSION 4)
# set(plplotada_VERSION ${plplotada_SOVERSION}.0.0)
# 
# set(plplotcxx_SOVERSION 15)
# set(plplotcxx_VERSION ${plplotcxx_SOVERSION}.0.0)
# 
# set(plplotdmd_SOVERSION 4)
# set(plplotdmd_VERSION ${plplotdmd_SOVERSION}.0.0)
# 
# set(plplotfortran_SOVERSION 0)
# set(plplotfortran_VERSION ${plplotfortran_SOVERSION}.2.0)
# 
# set(plplotqt_SOVERSION 2)
# set(plplotqt_VERSION ${plplotqt_SOVERSION}.0.3)
# 
# set(tclmatrix_SOVERSION 10)
# set(tclmatrix_VERSION ${tclmatrix_SOVERSION}.3.0)
# 
# set(plplottcltk_SOVERSION 14)
# set(plplottcltk_VERSION ${plplottcltk_SOVERSION}.1.0)
# 
# set(plplottcltk_Main_SOVERSION 1)
# set(plplottcltk_Main_VERSION ${plplottcltk_Main_SOVERSION}.0.1)
# 
# set(plplotwxwidgets_SOVERSION 1)
# set(plplotwxwidgets_VERSION ${plplotwxwidgets_SOVERSION}.2.2)
# 
# set(smokeplplotqt_SOVERSION 0)
# set(smokeplplotqt_VERSION ${smokeplplotqt_SOVERSION}.0.0)
# 
