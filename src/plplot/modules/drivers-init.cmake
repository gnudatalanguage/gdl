# cmake/modules/drivers-init.cmake
#
# Start driver initializations
#
# Copyright (C) 2006-2018  Alan W. Irwin
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

# Module to initialize device options.

# Results are contained in the following variables:
# PLD_devicename (ON or OFF): whether each PLplot-related device is enabled
#   or not.  devicename is png, jpeg, etc.
# DRIVERS_DEVICE_LIST: see below for the definition

# We think of a "device" as the output medium.  Could be a machine
# style (Tektronix, X11), or a file format (Postscript).
#
# A "driver" is the PLplot code that produces the output on the
# medium.
#
# Some drivers support multiple devices.  But we we need to make sure
# we compile only the selected drivers.  That is, if a driver supports
# devices not selected, we don't want to compile it at all.  Or if it
# supports multiple selected devices, we only want to compile it
# once.  To support this, we build an association list between devices
# and drivers.  This will be inspected to build up the set of drivers
# to be compiled based on which devices are selected.

# The DRIVERS_DEVICE_LIST defined below is a colon-separated list of
# <device>:<driver>:<enable_default>:<kind>:<familied> items.

# <device> is the name of the device (e.g., epsqt).

# <driver> is the name of the device driver (e.g., qt).

# <enable_default> should be ON or OFF and this will reflect in
# inclusion/exclusion by default (as shown by ccmake).

# <kind> should be one of F (file), I (interactive), M (memory), E
# (external), or N (null, i.e., valid prototype drive that produces
# no results.)

# <familed> should be either ON or OFF depending on whether familying is
# a necessity for this device to generate multipage plots.

# <device>, <driver>, and <enable_default> are fundamental data that
# help to determine the list of enabled drivers, DRIVERS_LIST (used in
# the summary), and list of enabled devices, DEVICES_LIST (used in the
# summary and also when iterating over enabled devices in the build
# system).  <kind> is used (in drivers-finish.cmake) to determine
# which enabled devices are included in the FILE_DEVICES_LIST (which
# also includes <familied> information), and <kind> is used to
# determine (again in drivers-finish.cmake) which enabled devices are
# included in the INTERACTIVE_DEVICES_LIST.  Both FILE_DEVICES_LIST
# and INTERACTIVE_DEVICES_LIST are used in the examples subdirectory
# to set up testing of file devices and interactive devices.

# Ordered alphabetically by second in each pair (the driver) and then
# alphabetically by device (the first in each pair) for human
# consumption, but this is not necessary.

set(DRIVERS_DEVICE_LIST
  "mem:mem:ON:M:OFF"
  "null:null:ON:M:OFF"
  "ps:ps:ON:F:OFF"
  "psc:ps:ON:F:OFF"
  "svg:svg:ON:F:ON"
  "wxwidgets:wxwidgets:ON:I:OFF"
  "xwin:xwin:ON:I:OFF"
  )

  set(DEFAULT ON)
  set(PRESET_DEFAULT OFF)

foreach(DRIVERS_DEVICE ${DRIVERS_DEVICE_LIST})
  string(REGEX REPLACE "^(.*):.*:.*:.*:.*$" "\\1" DEVICE ${DRIVERS_DEVICE})
  string(REGEX REPLACE "^.*:.*:.*:(.*):.*$" "\\1" KIND ${DRIVERS_DEVICE})
  string(REGEX REPLACE "^.*:.*:.*:.*:(.*)$" "\\1" REQUIRE_FAMILYING ${DRIVERS_DEVICE})
  # Check that you don't have the invalid combination of a non-file device
  # that requires familying.
  if(REQUIRE_FAMILYING AND NOT KIND STREQUAL "F")
    message(FATAL_ERROR "Invalid combination of REQUIRE_FAMILYING = ${REQUIRE_FAMILYING} AND KIND = ${KIND} for DEVICE = ${DEVICE}")
  endif(REQUIRE_FAMILYING AND NOT KIND STREQUAL "F")

  # DRIVERS_DEVICE only used in one case below:
  set(USED_DRIVERS_DEVICE OFF)
  if(NOT PRESET_DEFAULT)
    # must set DEFAULT value since it hasn't been preset.
    if(DEFAULT_NO_QT_DEVICES AND DEVICE MATCHES ".*qt.*" AND NOT DEVICE STREQUAL "aqt")
      set(DEFAULT OFF)
    elseif(DEFAULT_NO_CAIRO_DEVICES AND DEVICE MATCHES ".*cairo.*")
      set(DEFAULT OFF)
    else(DEFAULT_NO_QT_DEVICES AND DEVICE MATCHES ".*qt.*" AND NOT DEVICE STREQUAL "aqt")
      string(REGEX REPLACE "^.*:.*:(.*):.*:.*$" "\\1" DEFAULT ${DRIVERS_DEVICE})
      set(USED_DRIVERS_DEVICE ON)
    endif(DEFAULT_NO_QT_DEVICES AND DEVICE MATCHES ".*qt.*" AND NOT DEVICE STREQUAL "aqt")
  endif(NOT PRESET_DEFAULT)
  #message(STATUS "DEBUG: DEVICE = ${DEVICE}")
  #message(STATUS "DEBUG: DEFAULT= ${DEFAULT}")
  option(PLD_${DEVICE} "Enable ${DEVICE} device" ${DEFAULT})
  if(PLD_${DEVICE} AND USED_DRIVERS_DEVICE AND NOT ${DEFAULT})
    message(STATUS
      "WARNING: You have enabled the PLD_${DEVICE} device which is disabled by "
      "default either because it is deprecated or because there are know issues "
      "with it. Please check the documentation / release notes for details.")
  endif(PLD_${DEVICE} AND USED_DRIVERS_DEVICE AND NOT ${DEFAULT})
endforeach(DRIVERS_DEVICE)


