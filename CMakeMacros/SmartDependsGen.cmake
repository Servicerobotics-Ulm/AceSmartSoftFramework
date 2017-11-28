# --------------------------------------------------------------------------
#
#  Copyright (C) 2013 Alex Lotz
#
#        lotz@fh-ulm.de
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

#SET(SMART_MODULE_PATH "$ENV{SMART_ROOT_ACE}/ModuleDepends" CACHE PATH "Path to export Find<project-name>.cmake modules")
SET(SMART_MODULE_PATH "${CMAKE_BINARY_DIR}/ModuleDepends" CACHE PATH "Path to export Find<project-name>.cmake modules")

# find all Find<project-name>.cmake.in files recursivelly from top level dir
FILE(GLOB_RECURSE DEP_FILES ${CMAKE_SOURCE_DIR}/Find*.cmake.in)

# configure the file for each dependency
FOREACH(DEP_FILE IN LISTS DEP_FILES)

# get the path (without the file-name)
GET_FILENAME_COMPONENT(CURR_SOURCE_DIR ${DEP_FILE} PATH)
# get the file name without the .cmake.in ending
GET_FILENAME_COMPONENT(CURR_FILE_NAME ${DEP_FILE} NAME_WE)

# configure file and replace the CURR_DIR with the current source-directory
CONFIGURE_FILE(${DEP_FILE} "${SMART_MODULE_PATH}/${CURR_FILE_NAME}.cmake" @ONLY)

ENDFOREACH(DEP_FILE)


# find all Find<project-name>.cmake module-files recursivelly from top level dir
FILE(GLOB_RECURSE MOD_FILES ${CMAKE_SOURCE_DIR}/Find*.cmake)

# configure the file for each dependency
FOREACH(MOD_FILE IN LISTS MOD_FILES)

# get the file name without the .cmake.in ending
GET_FILENAME_COMPONENT(CURR_FILE_NAME ${MOD_FILE} NAME)

# configure file and replace the CURR_DIR with the current source-directory
CONFIGURE_FILE(${MOD_FILE} "${SMART_MODULE_PATH}/${CURR_FILE_NAME}" COPYONLY)

ENDFOREACH(MOD_FILE)

# set-up the module path to find the corresponding modules
SET(CMAKE_MODULE_PATH ${SMART_MODULE_PATH})

