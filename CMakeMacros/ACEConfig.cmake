# --------------------------------------------------------------------------
#
#  Copyright (C) 2014 Alex Lotz
#
#        lotz@hs-ulm.de
#
#        Alex Lotz
#        University of Applied Sciences
#        Prittwitzstr. 10
#        D-89075 Ulm
#        Germany
#
#  This file is part of the "SmartSoft Communication Library".
#  It provides standardized patterns for communication between
#  different components.
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# --------------------------------------------------------------------------

IF(NOT TARGET ACE)
  # set default value, which is reset below if ACE is found
  SET(ACE_FOUND FALSE)
  # ACE was not yet found within this scope
  IF(UNIX)
    # search include folder
    IF(EXISTS "$ENV{ACE_ROOT}/ace/ACE.h")
      SET(ACE_INCLUDE_DIRS $ENV{ACE_ROOT})
    ELSEIF(EXISTS "/usr/include/ace/ACE.h")
      SET(ACE_INCLUDE_DIRS "/usr/include")
    ENDIF()
    # search the actual library file
    FIND_LIBRARY(ACE_LIB NAMES ACE PATHS $ENV{ACE_ROOT}/lib DOC "The Adaptive Communication Environment (ACE) library")
    IF(ACE_LIB)
      #SET(ACE_LIBRARIES ACE pthread)
      # set compiler flags
      #SET(CMAKE_CXX_FLAGS "-fPIC -D__ACE_INLINE__ -DACE_HAS_VALGRIND -O0 -g3")

      #GET_FILENAME_COMPONENT(ACE_LIB_PATH ${ACE_LIB} DIRECTORY)
      #LINK_DIRECTORIES(${ACE_LIB_PATH})

      # create ACE target as a SHARED IMPORTED LIBRARY
      ADD_LIBRARY(ACE SHARED IMPORTED)
      # setup ACE target configuration
      SET_TARGET_PROPERTIES(ACE PROPERTIES
        INTERFACE_COMPILE_DEFINITIONS "__ACE_INLINE__;ACE_HAS_VALGRIND"
        INTERFACE_INCLUDE_DIRECTORIES ${ACE_INCLUDE_DIRS}
        INTERFACE_LINK_LIBRARIES pthread
        IMPORTED_LOCATION ${ACE_LIB}
      )

      # these variables are considered depricated since CMake v3 and should not be used anymore
      UNSET(ACE_INCLUDE_DIRS)
      #UNSET(ACE_LIBRARIES)

      SET(ACE_FOUND TRUE)
    ENDIF(ACE_LIB)
  ELSEIF(WIN32)
    # search include folder
    IF(EXISTS "$ENV{ACE_ROOT}/ace/ACE.h")
      SET(ACE_INCLUDE_DIRS $ENV{ACE_ROOT})
    ENDIF(EXISTS "$ENV{ACE_ROOT}/ace/ACE.h")
    # search library
    FIND_LIBRARY(ACE_LIBRARIES NAMES ACEd PATHS $ENV{ACE_ROOT}/lib)
    #SET(ACE_LIB_NAME DEBUG ACEd OPTIMIZED ACE)
    IF(ACE_LIBRARIES)
      SET(ACE_FOUND TRUE)
    ENDIF(ACE_LIBRARIES)
     # set compiler flags
    SET(CMAKE_CXX_FLAGS "/D_WINDOWS /DWIN32 /D_CONSOLE /D_CRT_NONSTDC_NO_WARNINGS /D_CRT_SECURE_NO_WARNINGS /D_CRT_SECURE_NO_DEPRECATE /D_CRT_NONSTDC_NO_DEPRECATE /FD /EHsc /W3 /nologo /TP /wd4355 /wd4267 /errorReport:prompt")
  ENDIF()
  # TODO: add individual ACE search strategies for Apple operating systems and Android (if needed)
ENDIF(NOT TARGET ACE)

