# cmake/modules/freetype.cmake
#
# Copyright (C) 2006  Andrew Ross
# Copyright (C) 2014 Alan W. Irwin
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
# Configuration for the freetype support in plplot.

set (WITH_FREETYPE ON)
# Look for freetype libraries
if (WITH_FREETYPE)
  find_package(Freetype)
  if (FREETYPE_FOUND)
    message(STATUS "FREETYPE_INCLUDE_DIRS = ${FREETYPE_INCLUDE_DIRS}")
    string(REGEX REPLACE ";" " -I" FREETYPE_INCLUDE_CFLAGS "-I${FREETYPE_INCLUDE_DIRS}")
    message(STATUS "FREETYPE_CFLAGS = ${FREETYPE_INCLUDE_CFLAGS}")

    message(STATUS "FREETYPE_LIBRARIES = ${FREETYPE_LIBRARIES}")
  else (FREETYPE_FOUND)
    set(WITH_FREETYPE OFF
      CACHE BOOL "Enable driver options for using freetype library for fonts"
      FORCE
      )
  endif (FREETYPE_FOUND)
endif (WITH_FREETYPE)

if (WITH_FREETYPE)

  if(WIN32_OR_CYGWIN)
    set(
      PL_FREETYPE_FONT_PATH
      "c:/windows/fonts"
      CACHE PATH "Path for TrueType fonts"
      )
  else(WIN32_OR_CYGWIN)
    set(
      PL_FREETYPE_FONT_PATH
      "/usr/share/fonts/truetype/freefont"
      CACHE PATH "Path for TrueType fonts"
      )
  endif(WIN32_OR_CYGWIN)
  # PLplot internally needs a trailing slash for this path.
  set(PL_FREETYPE_FONT_DIR "${PL_FREETYPE_FONT_PATH}/")

  set(PL_FREETYPE_FONT_LIST
    "PL_FREETYPE_MONO:FreeMono.ttf:cour.ttf"
    "PL_FREETYPE_MONO_BOLD:FreeMonoBold.ttf:courbd.ttf"
    "PL_FREETYPE_MONO_BOLD_ITALIC:FreeMonoBoldOblique.ttf:courbi.ttf"
    "PL_FREETYPE_MONO_BOLD_OBLIQUE:FreeMonoBoldOblique.ttf:courbi.ttf"
    "PL_FREETYPE_MONO_ITALIC:FreeMonoOblique.ttf:couri.ttf"
    "PL_FREETYPE_MONO_OBLIQUE:FreeMonoOblique.ttf:couri.ttf"
    "PL_FREETYPE_SANS:FreeSans.ttf:arial.ttf"
    "PL_FREETYPE_SANS_BOLD:FreeSansBold.ttf:arialbd.ttf"
    "PL_FREETYPE_SANS_BOLD_ITALIC:FreeSansBoldOblique.ttf:arialbi.ttf"
    "PL_FREETYPE_SANS_BOLD_OBLIQUE:FreeSansBoldOblique.ttf:arialbi.ttf"
    "PL_FREETYPE_SANS_ITALIC:FreeSansOblique.ttf:ariali.ttf"
    "PL_FREETYPE_SANS_OBLIQUE:FreeSansOblique.ttf:ariali.ttf"
    "PL_FREETYPE_SCRIPT:FreeSerif.ttf:arial.ttf"
    "PL_FREETYPE_SCRIPT_BOLD:FreeSerifBold.ttf:arialbd.ttf"
    "PL_FREETYPE_SCRIPT_BOLD_ITALIC:FreeSerifBoldItalic.ttf:arialbi.ttf"
    "PL_FREETYPE_SCRIPT_BOLD_OBLIQUE:FreeSerifBoldItalic.ttf:arialbi.ttf"
    "PL_FREETYPE_SCRIPT_ITALIC:FreeSerifItalic.ttf:ariali.ttf"
    "PL_FREETYPE_SCRIPT_OBLIQUE:FreeSerifItalic.ttf:ariali.ttf"
    "PL_FREETYPE_SERIF:FreeSerif.ttf:times.ttf"
    "PL_FREETYPE_SERIF_BOLD:FreeSerifBold.ttf:timesbd.ttf"
    "PL_FREETYPE_SERIF_BOLD_ITALIC:FreeSerifBoldItalic.ttf:timesbi.ttf"
    "PL_FREETYPE_SERIF_BOLD_OBLIQUE:FreeSerifBoldItalic.ttf:timesbi.ttf"
    "PL_FREETYPE_SERIF_ITALIC:FreeSerifItalic.ttf:timesi.ttf"
    "PL_FREETYPE_SERIF_OBLIQUE:FreeSerifItalic.ttf:timesi.ttf"
    "PL_FREETYPE_SYMBOL:FreeSans.ttf:arial.ttf"
    "PL_FREETYPE_SYMBOL_BOLD:FreeSansBold.ttf:arialbd.ttf"
    "PL_FREETYPE_SYMBOL_BOLD_ITALIC:FreeSansBoldOblique.ttf:arialbi.ttf"
    "PL_FREETYPE_SYMBOL_BOLD_OBLIQUE:FreeSansBoldOblique.ttf:arialbi.ttf"
    "PL_FREETYPE_SYMBOL_ITALIC:FreeSansOblique.ttf:ariali.ttf"
    "PL_FREETYPE_SYMBOL_OBLIQUE:FreeSansOblique.ttf:ariali.ttf"
    )

  foreach(FONT_ENTRY ${PL_FREETYPE_FONT_LIST})
    string(REGEX REPLACE "^(.*):.*:.*$" "\\1" NAME ${FONT_ENTRY})
    if (WIN32_OR_CYGWIN)
      string(REGEX REPLACE "^.*:.*:(.*)$" "\\1" FONT ${FONT_ENTRY})
    else (WIN32_OR_CYGWIN)
      string(REGEX REPLACE "^.*:(.*):.*$" "\\1" FONT ${FONT_ENTRY})
    endif (WIN32_OR_CYGWIN)
    set(${NAME} ${FONT}
      CACHE FILEPATH "Font file for ${NAME}"
      )
  endforeach(FONT_ENTRY PL_FREETYPE_FONT_LIST)

  # Check a couple of fonts actually exists
  if (EXISTS ${PL_FREETYPE_FONT_DIR}${PL_FREETYPE_SANS})
    if (EXISTS ${PL_FREETYPE_FONT_DIR}${PL_FREETYPE_SYMBOL})
    else (EXISTS ${PL_FREETYPE_FONT_DIR}${PL_FREETYPE_SYMBOL})
      message("Fonts not found - disabling freetype")
      set(WITH_FREETYPE OFF CACHE BOOL
	"Enable driver options for using freetype library for fonts" FORCE
	)
    endif (EXISTS ${PL_FREETYPE_FONT_DIR}${PL_FREETYPE_SYMBOL})
  else (EXISTS ${PL_FREETYPE_FONT_DIR}${PL_FREETYPE_SANS})
    message("Fonts not found - disabling freetype")
    set(WITH_FREETYPE OFF CACHE BOOL
      "Enable driver options for using freetype library for fonts" FORCE
      )
  endif (EXISTS ${PL_FREETYPE_FONT_DIR}${PL_FREETYPE_SANS})

endif (WITH_FREETYPE)

if (WITH_FREETYPE)
  set(PL_HAVE_FREETYPE ON)
endif (WITH_FREETYPE)
