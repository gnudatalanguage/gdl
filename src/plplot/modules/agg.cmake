# cmake/modules/agg.cmake
#
# Copyright (C) 2006  Werner Smekal
#
# This file is part of PLplot.
#
# PLplot is free software; you can redistribute it and/or modify
# it under the terms of the GNU Library General Public License as published
# by the Free Software Foundation; version 2 of the License.
#
# PLplot is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with the file PLplot; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
#
# Configuration for AGG support in plplot.

option(
HAVE_AGG
"Enable driver options for using AGG library for antializing"
ON
)

# Look for agg libraries
if (HAVE_AGG)
  find_package(AGG)
  if(NOT AGG_FOUND)
    message(STATUS
    "WARNING: AGG not found.  Setting HAVE_AGG to OFF."
    )
    set(HAVE_AGG OFF
    CACHE BOOL "Enable driver options for using AGG library for antializing"
    FORCE
    )
  endif(NOT AGG_FOUND)
endif (HAVE_AGG)
