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

CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(AceSmartSoft VERSION 3.0.0)

SET(PARENT_NAME ${PROJECT_NAME})

# this option is used to activate/deactivate automatic dependency building
OPTION(BUILD_DEPENDENCIES "Activates dependency building" ON)

MESSAGE("-- ${PROJECT_NAME} Version ${PROJECT_VERSION}")

STRING(TIMESTAMP SMARTSOFT_RELEASE_DATE UTC)

# create the VERSION file (which uses the defines above)
CONFIGURE_FILE(
  "${PROJECT_SOURCE_DIR}/CMakeMacros/VERSION.in"
  "${PROJECT_BINARY_DIR}/VERSION" @ONLY
)

IF(EXISTS $ENV{SMART_ROOT_ACE}/CMakeMacros/SmartOSConfig.cmake)
  INCLUDE("$ENV{SMART_ROOT_ACE}/CMakeMacros/SmartOSConfig.cmake")
ENDIF(EXISTS $ENV{SMART_ROOT_ACE}/CMakeMacros/SmartOSConfig.cmake)

# default installation prefix is set to the path given in the SMART_ROOT_ACE environment variable
IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  # SET(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR} CACHE PATH "Set default SmartSoft install prefix" FORCE)
  SET(CMAKE_INSTALL_PREFIX $ENV{SMART_ROOT_ACE} CACHE PATH "Set default SmartSoft install prefix" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

FIND_PACKAGE(ACE 6.0.2 PATHS $ENV{SMART_ROOT_ACE}/CMakeMacros /opt/smartSoftAce/CMakeMacros)
# include directories are depricated since CMake v3
#INCLUDE_DIRECTORIES(${ACE_INCLUDE_DIRS})

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# create uninstall configuration file
CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/CMakeMacros/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

# create uninstall target
ADD_CUSTOM_TARGET(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

IF(UNIX)
# TODO: is there a more portable way of doing this?
ADD_CUSTOM_TARGET(dpkg
    COMMAND /usr/bin/dpkg-buildpackage -us -uc -tc WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} COMMENT "generate package of smartsoftKernel")
ENDIF(UNIX)

##################################################################
# trigger the build process of the project(s) in the subdirectory
ADD_SUBDIRECTORY(AceSmartSoftKernel)
ADD_SUBDIRECTORY(NamingService)
ADD_SUBDIRECTORY(Examples)
##################################################################

# ===============================================================
# The following code exports the SmartSoft configuration
# in order to be later used (be found) in external projects.
# For example in order to build a component, add the 
# following lines into the components CMakeLists.txt:
#
# PROJECT(<component-name>)
#
# FIND_PACKAGE(SmartSoft <version> PATHS $ENV{SMART_ROOT_ACE})
#
# ADD_EXECUTABLE(<component-name> <component-sources>)
# TARGET_LINK_LIBRARIES(<component-name> SmartSoftKernel)
# ===============================================================

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
EXPORT(PACKAGE ${PROJECT_NAME})

# copy the SmartSoft configuration file into build-tree
CONFIGURE_FILE(
  "${PROJECT_SOURCE_DIR}/CMakeMacros/${PROJECT_NAME}Config.cmake.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake" @ONLY)

# version file is used in external projects to automatically check compatibility
CONFIGURE_FILE(
  "${PROJECT_SOURCE_DIR}/CMakeMacros/${PROJECT_NAME}ConfigVersion.cmake.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake" @ONLY)

# copy the VERSION and Config files into installation root folder
INSTALL(FILES "${PROJECT_BINARY_DIR}/VERSION" DESTINATION ${CMAKE_INSTALL_PREFIX})
INSTALL(FILES
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION modules)

#ADD_CUSTOM_TARGET(AutoInstall${PROJECT_NAME} ALL COMMAND ${CMAKE_COMMAND} ARGS -P cmake_install.cmake WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Install ${PROJECT_NAME}")
#ADD_DEPENDENCIES(AutoInstall${PROJECT_NAME} ${PROJECT_NAME}Kernel ExampleInterfaceClasses ${PROJECT_NAME}NamingService ${PROJECT_NAME}NamingServiceList ${PROJECT_NAME}NamingServiceClientTest CommDiagnose ${PROJECT_NAME}MonitoringSlave ${PROJECT_NAME}MonitoringMaster ${ALL_EXAMPLE_COMPONENTS})

