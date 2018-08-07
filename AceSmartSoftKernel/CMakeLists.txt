#--------------------------------------------------------------------------
#
# Copyright (C) 2012 Alex Lotz
#
#       lotz@hs-ulm.de
#       schlegel@hs-ulm.de
#
#       Prof. Dr. Christian Schlegel
#       University of Applied Sciences
#       Prittwitzstr. 10
#       D-89075 Ulm
#       Germany
#
#
# This file is part of ACE/SmartSoft.
#
# ACE/SmartSoft is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# ACE/SmartSoft is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with ACE/SmartSoft.  If not, see <http://www.gnu.org/licenses/>.
#
#--------------------------------------------------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(${PARENT_NAME}Kernel)

# find ACE library package
IF(NOT TARGET ACE)
  FIND_PACKAGE(ACE 6.0.2 PATHS ${CMAKE_CURRENT_SOURCE_DIR}/CMakeMacros)
ENDIF(NOT TARGET ACE)

# find the SmartSoft Component-Developer API package
FIND_PACKAGE(SmartSoft_CD_API 1.0 PATHS $ENV{SMART_ROOT_ACE}/modules)

# create a ConfigFile including the current SmartSoft version
CONFIGURE_FILE(
  "${PROJECT_SOURCE_DIR}/middlewareMapping/smartConfig.hh.in"
  "${PROJECT_BINARY_DIR}/middlewareMapping/smartConfig.hh" @ONLY
)

# OS mapping sources
FILE(GLOB OSMappingSrc middlewareMapping/*.cc)

# Kernel implementation sources
FILE(GLOB kernelSrc *.cc)

# all header files in this folder and the middlewareMapping subfolder
FILE(GLOB headerFiles *.hh)
FILE(GLOB osMappingHeaderFiles middlewareMapping/*.hh)
FILE(GLOB templateHeaders *.th)
SOURCE_GROUP(TemplateHeaders FILES ${templateHeaders})


# ================================================================================
# create the SmartSoftKernel library

IF(WIN32)
ADD_LIBRARY(${PROJECT_NAME} STATIC ${OSMappingSrc} ${osMappingHeaderFiles} ${kernelSrc} ${headerFiles} ${templateHeaders})
ELSE()
ADD_LIBRARY(${PROJECT_NAME} SHARED ${OSMappingSrc} ${osMappingHeaderFiles} ${kernelSrc} ${headerFiles} ${templateHeaders})
ENDIF()

# use the auto-type compile feature (available since C++11)
TARGET_COMPILE_FEATURES(${PROJECT_NAME} PUBLIC cxx_auto_type)

# link SmartSoftKernel with the ACE library
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ACE)

# link SmartSoftKernel with the SmartSoft_CD_API library
TARGET_LINK_LIBRARIES(${PROJECT_NAME} SmartSoft_CD_API)

# define include paths for the SmartSoftKernel target
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/middlewareMapping>
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/middlewareMapping>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>  # <prefix>/include/SmartSoftKernel
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}/middlewareMapping>  # <prefix>/include/SmartSoftKernel
)


IF(DEFINED SmartSoft_VERSION)
# setup the version to be generated into the library name
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES VERSION ${SmartSoft_VERSION}
                                          SOVERSION ${SmartSoft_VERSION_MAJOR})
ENDIF(DEFINED SmartSoft_VERSION)
# ================================================================================

# ================================================================================
# Here are the install and export definitions to use the SmartSoft kernel in 
# external projects (either using the build tree directly or the installation tree)

# Add all targets to the build-tree export set
IF(DEFINED PARENT_NAME)
  EXPORT(TARGETS ${PROJECT_NAME} FILE "${${PARENT_NAME}_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")
ELSE()
  EXPORT(TARGETS ${PROJECT_NAME} FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")
ENDIF()

#install library into destination and export as dependency
INSTALL(TARGETS ${PROJECT_NAME}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib # installation directoriy is <install-prefix>/lib
  # add this project to the dependencies
  EXPORT ${PROJECT_NAME}Targets
)

# install the pseudo target export (creates SmartSoftKernelTargets.cmake)
INSTALL(EXPORT ${PROJECT_NAME}Targets DESTINATION modules)

# install header-files as part of the SmartSoft Kernel library into the target/include location
INSTALL(FILES 
  ${headerFiles}
  ${templateHeaders}  
  DESTINATION include/${PROJECT_NAME})

INSTALL(FILES 
  ${PROJECT_BINARY_DIR}/middlewareMapping/smartConfig.hh 
  ${osMappingHeaderFiles}
  DESTINATION include/${PROJECT_NAME}/middlewareMapping)

