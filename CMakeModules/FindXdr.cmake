#
#  The RealityGrid Steering Library
#
#  Copyright (c) 2002-2010, University of Manchester, United Kingdom.
#  All rights reserved.
#
#  This software is produced by Research Computing Services, University
#  of Manchester as part of the RealityGrid project and associated
#  follow on projects, funded by the EPSRC under grants GR/R67699/01,
#  GR/R67699/02, GR/T27488/01, EP/C536452/1, EP/D500028/1,
#  EP/F00561X/1.
#
#  LICENCE TERMS
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#
#    * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#
#    * Redistributions in binary form must reproduce the above
#      copyright notice, this list of conditions and the following
#      disclaimer in the documentation and/or other materials provided
#      with the distribution.
#
#    * Neither the name of The University of Manchester nor the names
#      of its contributors may be used to endorse or promote products
#      derived from this software without specific prior written
#      permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
#  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
#  COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
#  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
#  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
#  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
#  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#
#  Author: Robert Haines

if(XDR_INCLUDE_DIR)
  # already in cache so be quiet
  set(XDR_FIND_QUIETLY TRUE)
endif(XDR_INCLUDE_DIR)

# if we have rpc.h then we may *need* it for xdr.h
# so don't only look for xdr.h
find_path(RPC_INCLUDE_DIR "rpc/rpc.h")

# might only have xdr.h
if(RPC_INCLUDE_DIR STREQUAL "RPC_INCLUDE_DIR-NOTFOUND")
  find_path(XDR_INCLUDE_DIR "rpc/xdr.h")
  if(NOT XDR_INCLUDE_DIR STREQUAL "XDR_INCLUDE_DIR-NOTFOUND")
    set(REG_HAS_XDR_H 1)
  endif(NOT XDR_INCLUDE_DIR STREQUAL "XDR_INCLUDE_DIR-NOTFOUND")
else(RPC_INCLUDE_DIR STREQUAL "RPC_INCLUDE_DIR-NOTFOUND")
  set(REG_HAS_RPC_H 1)
  set(XDR_INCLUDE_DIR ${RPC_INCLUDE_DIR})
endif(RPC_INCLUDE_DIR STREQUAL "RPC_INCLUDE_DIR-NOTFOUND")

# find the lib and add it if found
find_library(XDR_LIBRARY NAMES rpc xdr_s xdr openxdr)
if(NOT XDR_LIBRARY STREQUAL "XDR_LIBRARY-NOTFOUND")
  set(REG_EXTERNAL_LIBS ${REG_EXTERNAL_LIBS} ${XDR_LIBRARY})
endif(NOT XDR_LIBRARY STREQUAL "XDR_LIBRARY-NOTFOUND")

mark_as_advanced(RPC_INCLUDE_DIR XDR_INCLUDE_DIR XDR_LIBRARY)

# tend not to need a separate lib on Unix systems
# do need one for Windows and Cygwin
include(FindPackageHandleStandardArgs)
if(UNIX)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(XDR DEFAULT_MSG XDR_INCLUDE_DIR)
else(UNIX)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(XDR DEFAULT_MSG XDR_INCLUDE_DIR XDR_LIBRARY)
endif(UNIX)