CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)

#############################################################
## SMART_VERSION
## This macro simplifies the creation of the version string
#############################################################
MACRO(SMART_VERSION)
  IF(DEFINED VERSION)
    STRING(TOUPPER ${PROJECT_NAME} UPPER_NAME)
    STRING(REPLACE "." ";" VERSION_LIST ${VERSION})
    LIST(LENGTH VERSION_LIST VERSION_LIST_SIZE)
    IF(${VERSION_LIST_SIZE} LESS 3)
      MESSAGE(WARNING "SMART_VERSION: failed to parse the version string ${VERSION}. The Version format should be <MajorNbr>.<MinorNbr>.<PatchNbr>, e.g. 1.0.0")
    ELSE()
      LIST(GET VERSION_LIST 0 ${UPPER_NAME}_VERSION_MAJOR)
      LIST(GET VERSION_LIST 1 ${UPPER_NAME}_VERSION_MINOR)
      LIST(GET VERSION_LIST 2 ${UPPER_NAME}_VERSION_PATCH)
      SET(${UPPER_NAME}_VERSION_STRING ${${UPPER_NAME}_VERSION_MAJOR}.${${UPPER_NAME}_VERSION_MINOR}.${${UPPER_NAME}_VERSION_PATCH})
    
      # the following variable is used to generate the PackageConfigVersion.cmake file
      SET(PACKAGE_VERSION_STRING ${${UPPER_NAME}_VERSION_STRING})
      FIND_FILE(PACKAGE_VERION_FILE PackageConfigVersion.cmake.in PATHS $ENV{SMART_ROOT_ACE}/cmake-templates /opt/smartSoftAce/cmake-templates)
      IF(PACKAGE_VERION_FILE)
        CONFIGURE_FILE(${PACKAGE_VERION_FILE} ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake @ONLY)
		#CMAKE_TRACE_GENERATED_FILE("${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake")
      ENDIF(PACKAGE_VERION_FILE)
    ENDIF()
    UNSET(UPPER_NAME)
    UNSET(VERSION_LIST)
    UNSET(VERSION_LIST_SIZE)
  ENDIF(DEFINED VERSION)
ENDMACRO(SMART_VERSION)

#############################################################
## SMART_CHECK_OUT_OF_SOURCE_BUILD
## This macro checks whether cmake is called from a subdirectory
## and not within the source-tree. Calling cmake within the
## the source tree can cause infinity cylic dependencies
## resulting cmake running in infinite loops.
#############################################################
MACRO(SMART_CHECK_OUT_OF_SOURCE_BUILD)
	IF(DEFINED ENV{PWD})
		IF("$ENV{PWD}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
			MESSAGE(FATAL_ERROR "CMake is called within source tree (not from a subfolder named 'build') => this violates SmartSoft's build rules and will not work in the SmartMDSD Toolchain (please delete all cmake generated files from source tree and call 'cmake ..' from within a subfolder named 'build')")
		ELSE("$ENV{PWD}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
			IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/CMakeCache.txt")
				MESSAGE(FATAL_ERROR "${CMAKE_CURRENT_SOURCE_DIR}/CMakeCache.txt found in the source tree => this can cause infinite cyclic loops and should be avoided (please delete all cmake generated files from source tree and call 'cmake ..' from within a subfolder named 'build')")		
			ENDIF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/CMakeCache.txt")
		ENDIF("$ENV{PWD}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
	ELSE(DEFINED ENV{PWD})
		IF("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
			MESSAGE(FATAL_ERROR "CMake is either called within the source tree or another (old) CMakeCache.txt exists within the source tree => this violates SmartSoft's build rules and will not work in the SmartMDSD Toolchain (please delete all cmake generated files from source tree and call 'cmake ..' from within a subfolder named 'build')")
		ENDIF("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
	ENDIF(DEFINED ENV{PWD})
ENDMACRO(SMART_CHECK_OUT_OF_SOURCE_BUILD)

#############################################################
## SMART_SETUP_INSTALL_PREFIX
## This macro sets-up the default CMAKE_INSTALL_PREFIX
## according to the SmartSoft installtion guidelines.
#############################################################
MACRO(SMART_SETUP_INSTALL_PREFIX)
  # default installation prefix is set to the path given in the SMART_ROOT_ACE environment variable
  IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    IF(DEFINED ENV{SMART_ROOT_ACE})
      # in case the environment variable SMART_ROOT_ACE is defined, its content is used as the default install prefix
      SET(CMAKE_INSTALL_PREFIX $ENV{SMART_ROOT_ACE} CACHE PATH "Set default SmartSoft install prefix" FORCE)
    ELSE()
      MESSAGE(WARNING "CMAKE_INSTALL_PREFIX not defined, please either define the environment variable SMART_ROOT_ACE or directly set CMAKE_INSTALL_PREFIX")
    ENDIF()
  ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
ENDMACRO(SMART_SETUP_INSTALL_PREFIX)


#############################################################
## SMART_INIT_DISTCLEAN_ALL
## This macro is used to create a custom target for cleaning
## up all cmake generated files (should be called at the beginning 
## directly after initializing a CMake PROJECT.
#############################################################
MACRO(SMART_INIT_DISTCLEAN_ALL)
	IF(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/cmake_generated_files.txt")
		FILE(REMOVE "${CMAKE_CURRENT_BINARY_DIR}/cmake_generated_files.txt")
	ENDIF(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/cmake_generated_files.txt")

	FIND_FILE(CMAKE_DISTCLEAN_ALL_TEMPLATE cmake_distclean_all.cmake.in PATHS $ENV{SMART_ROOT_ACE}/cmake-templates /opt/smartSoftAce/cmake-templates)
	IF(CMAKE_DISTCLEAN_ALL_TEMPLATE)
		CONFIGURE_FILE("${CMAKE_DISTCLEAN_ALL_TEMPLATE}" "${CMAKE_CURRENT_BINARY_DIR}/cmake_distclean_all.cmake" @ONLY)

		ADD_CUSTOM_TARGET(${PROJECT_NAME}_distclean
			COMMAND ${CMAKE_BUILD_TOOL} clean
			COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_distclean_all.cmake"
			COMMAND ${CMAKE_COMMAND} -E remove "${CMAKE_CURRENT_BINARY_DIR}/cmake_distclean_all.cmake"
			DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/cmake_distclean_all.cmake"
			COMMENT "Remove ALL generated files from make AND CMake"
		)
		IF(NOT TARGET distclean-all)
			ADD_CUSTOM_TARGET(distclean-all)
		ENDIF(NOT TARGET distclean-all)
		ADD_DEPENDENCIES(distclean-all ${PROJECT_NAME}_distclean)
	ENDIF(CMAKE_DISTCLEAN_ALL_TEMPLATE)
ENDMACRO(SMART_INIT_DISTCLEAN_ALL)

#############################################################
# SMART_TRACE_GENERATED_FILE
# this macro should be called for each custom generated file
# (e.g. for files generated by the CONFIGURE_FILE or the 
#  CUSTOM_COMMAND macros)
#############################################################
MACRO(SMART_TRACE_GENERATED_FILE FILE)
	IF(EXISTS "${FILE}")
		FILE(APPEND "${CMAKE_CURRENT_BINARY_DIR}/cmake_generated_files.txt" "${FILE}\n")
	ENDIF(EXISTS "${FILE}")
ENDMACRO(SMART_TRACE_GENERATED_FILE FILE)
