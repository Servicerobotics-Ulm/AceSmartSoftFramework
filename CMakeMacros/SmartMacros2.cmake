CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)

# include the private macros (not meant to be used directly)
INCLUDE(${CMAKE_CURRENT_LIST_DIR}/SmartMacros2internal.cmake)

# include all the general helper macros
INCLUDE(${CMAKE_CURRENT_LIST_DIR}/SmartMacros2helpers.cmake)


############################################################
## This macro can be used to add a SmartSoft package into
## a global build tree. This macro is comparable to the 
## CMake macro ADD_SUBDIRECTORY, except that the added
## package is not included into the global build tree,
## but is treated as an external package with its own
## build tree. In addition, all the dependnecies
## of the package are automatically included.
## A SmartSoft package is a SmartSoft project, e.g.
## Utility, CommunicationObjects, Component, etc.
## Usage: SMART_ADD_PACKAGE(MyPackageName)
## Call this macro repetitive to build multiple packages.
## An optional second parameter can be used to provide
## an alternative path that deviates from the PACKAGE_NAME
## (this is reasonable for e.g. Utilities).
############################################################
MACRO(SMART_ADD_PACKAGE PACKAGE_NAME)
  # An optional second argument can be used to provide an alternative root folder 
  # (which should be an absolute path to a folder that contains the main 
  #  CMakeLists.txt file of the external package)
  IF(${ARGC} GREATER 1)
    # local root folder given using the optional argument
    INTERNAL_ADD_PACKAGE(${PACKAGE_NAME} ${ARGV1})
  ELSE()
    INTERNAL_ADD_PACKAGE(${PACKAGE_NAME})
  ENDIF()
ENDMACRO(SMART_ADD_PACKAGE PACKAGE_NAME)


#############################################################
## SMART_COMPONENT_PROJECT()
## This macro encapsulates the project creation procedure
## for SmartSoft-Components
#############################################################
MACRO(SMART_COMPONENT_PROJECT)
  INCLUDE(${CMAKE_CURRENT_LIST_DIR}/src-gen/ComponentProject.cmake)

  MESSAGE("-- COMPONENT_PROJECT(${PROJECT_NAME})")

  IF(SMARTCHECKOUTOFSOURCEBULID)
    SMART_CHECK_OUT_OF_SOURCE_BUILD()
  ENDIF(SMARTCHECKOUTOFSOURCEBULID)

  # setup custom distclean-all target that also cleans up the CMake generated files
  SMART_INIT_DISTCLEAN_ALL()

  # default installation prefix is set to the path given in the SMART_ROOT_ACE environment variable
  SMART_SETUP_INSTALL_PREFIX()

  # define a custom target that allows to build components without checking their dependencies (usefull for fast build)
  ADD_CUSTOM_TARGET(nodep COMMAND ${CMAKE_BUILD_TOOL} ${PROJECT_NAME}/fast WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Build ${PROJECT_NAME} without dependency checking")

  # define uninstall and disstclean targets
  FIND_FILE(UNINSTALL_FILE cmake_uninstall.cmake.in PATHS $ENV{SMART_ROOT_ACE}/CMakeMacros /opt/smartSoftAce/CMakeMacros)
  IF(UNINSTALL_FILE)
    CONFIGURE_FILE(${UNINSTALL_FILE} ${PROJECT_BINARY_DIR}/cmake_uninstall.cmake @ONLY)
    ADD_CUSTOM_TARGET(uninstall ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake COMMENT "uninstall ${PROJECT_NAME}")
  ELSE(UNINSTALL_FILE)
    MESSAGE(WARNING "Could NOT find cmake_uninstall.cmake.in file -> ignoring")
  ENDIF(UNINSTALL_FILE)
ENDMACRO(SMART_COMPONENT_PROJECT)


############################################################
## This macro can be used in Component projects to
## automatically BUILD external Communication Object
## dependencies.
## Usage: SMART_COMPONENT_AUTOBUILD_DEPENDENCIES()
## This macro needs to be called after SMART_COMPONENT_PROJECT(...)
############################################################
MACRO(SMART_COMPONENT_AUTOBUILD_DEPENDENCIES)
  # this option is used to activate/deactivate automatic dependency building
  OPTION(BUILD_DEPENDENCIES "Activates dependency building" ON)

  # include and build dependencies
  IF(${BUILD_DEPENDENCIES})
    INTERNAL_ADD_ALL_DEPENDENCIES(${PROJECT_NAME} ${PROJECT_SOURCE_DIR})
  ENDIF(${BUILD_DEPENDENCIES})
ENDMACRO(SMART_COMPONENT_AUTOBUILD_DEPENDENCIES)

############################################################
## This macro can be used in Communication Object projects
## to automatically LINK to external Communication Object
## dependencies.
## Usage: SMART_COMMOBJECT_AUTOLINK_DEPENDENCIES()
## This macro needs to be called after ADD_EXECUTABLE(...)
############################################################
MACRO(SMART_COMPONENT_AUTOLINK_DEPENDENCIES)
  # automatically import all external dependencies
  INTERNAL_IMPORT_ALL_DEPENDENCIES(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
  # if loaded, then the ${COMM_OBJECT_NAME}_DEPENDENCIES variable should be set
  IF(DEFINED ${PROJECT_NAME}_DEPENDENCIES)
    # link all external dependencies to the current comm-object
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${${PROJECT_NAME}_DEPENDENCIES})
  ENDIF(DEFINED ${PROJECT_NAME}_DEPENDENCIES)
ENDMACRO(SMART_COMPONENT_AUTOLINK_DEPENDENCIES)


############################################################
## This macro can be used in Component projects
## to generate the Component Doxygen documentation.
## Usage: SMART_COMPONENT_GENERATE_DOC()
## This macro needs to be called after ADD_EXECUTABLE(...)
############################################################
MACRO(SMART_COMPONENT_GENERATE_DOC)
  FIND_PACKAGE(Doxygen)
  IF (DOXYGEN_FOUND)
    IF( (EXISTS "$ENV{SMART_ROOT_ACE}/doc/DoxyfileComponentStandalone") AND
	(EXISTS "${PROJECT_SOURCE_DIR}/src-gen/mainpage.dox") )
    SET(DOXYGEN_INPUT "$ENV{SMART_ROOT_ACE}/doc/DoxyfileComponentStandalone")  
    SET(DOXYGEN_OUTPUT ${PROJECT_SOURCE_DIR}/doc-gen)

    FILE(GLOB DOXYGEN_FILES ${PROJECT_SOURCE_DIR}/src-gen/*.dox)

    ADD_CUSTOM_COMMAND(
      OUTPUT ${DOXYGEN_OUTPUT}
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
      DEPENDS ${DOXYGEN_FILES}
    )

    ADD_CUSTOM_TARGET(${PROJECT_NAME}_doc ALL DEPENDS ${DOXYGEN_OUTPUT})

    SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${PROJECT_SOURCE_DIR}/doc-gen)
    ENDIF()
  ENDIF (DOXYGEN_FOUND)
ENDMACRO(SMART_COMPONENT_GENERATE_DOC)


############################################################
## This macro can be used in Component projects
## to setup a default installation configuration.
## Usage: SMART_COMPONENT_AUTOINSTALL()
## This macro should come last in the CMakeLists.txt file.
############################################################
MACRO(SMART_COMPONENT_AUTOINSTALL)
  # automatically run "make install" after regular "make" for the communication object
  ADD_CUSTOM_TARGET(autoinstall ALL COMMAND ${CMAKE_COMMAND} ARGS -P cmake_install.cmake WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Install ${PROJECT_NAME}")
  ADD_DEPENDENCIES(autoinstall ${PROJECT_NAME})

  # install component executable target
  INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin)
ENDMACRO(SMART_COMPONENT_AUTOINSTALL)


############################################################
## SMART_ADD_UTILITY(MyUtility)
## This macro allows to (manually) add Utility dependencies
## within CommunicationObject projects.
############################################################
MACRO(SMART_ADD_UTILITY UTILITY_NAME)
  IF(${BUILD_DEPENDENCIES})
    INTERNAL_ADD_PACKAGE(${UTILITY_NAME})
  ENDIF(${BUILD_DEPENDENCIES})
  INTERNAL_IMPORT_PACKAGE(${UTILITY_NAME})
  LIST(APPEND ${PROJECT_NAME}_LOCAL_DEPENDENCIES ${UTILITY_NAME})
ENDMACRO(SMART_ADD_UTILITY UTILITY_NAME)

#############################################################
## SMART_COMMOBJECT_PROJECT
## This macro encapsulates the project creation procedure
## for SmartSoft-CommunicationObjects
#############################################################
MACRO(SMART_COMMOBJECT_PROJECT)
  INCLUDE(${CMAKE_CURRENT_LIST_DIR}/src-gen/CommObjectProject.cmake)

  MESSAGE("-- COMMOBJECT_PROJECT(${PROJECT_NAME})")

  IF(DEFINED ${PROJECT_NAME}_VERSION)
      FIND_FILE(PACKAGE_VERION_FILE PackageConfigVersion.cmake.in PATHS $ENV{SMART_ROOT_ACE}/CMakeMacros /opt/smartSoftAce/CMakeMacros)
      IF(PACKAGE_VERION_FILE)
        CONFIGURE_FILE(${PACKAGE_VERION_FILE} ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake @ONLY)
        SMART_TRACE_GENERATED_FILE("${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake")
      ENDIF(PACKAGE_VERION_FILE)
  ENDIF(DEFINED ${PROJECT_NAME}_VERSION)

  OPTION(CHECK_OUT_OF_SOURCE_BULID "This option defines whether the check of out of source builds is enabled" ON)

  # in-source builds are discouraged
  IF(CHECK_OUT_OF_SOURCE_BULID)
    SMART_CHECK_OUT_OF_SOURCE_BUILD()
  ENDIF(CHECK_OUT_OF_SOURCE_BULID)

  # create distclean-all target that also removes the CMake-generated files
  SMART_INIT_DISTCLEAN_ALL()

  # each comm-obj depend on the ACE middleware library
  FIND_PACKAGE(ACE 6.0.2 PATHS $ENV{SMART_ROOT_ACE}/CMakeMacros /opt/smartSoftAce/CMakeMacros)

  # default installation prefix is set to the path given in the SMART_ROOT_ACE environment variable
  SMART_SETUP_INSTALL_PREFIX()

  FIND_FILE(UNINSTALL_FILE cmake_uninstall.cmake.in PATHS $ENV{SMART_ROOT_ACE}/CMakeMacros /opt/smartSoftAce/CMakeMacros)
  IF(UNINSTALL_FILE)
    CONFIGURE_FILE(${UNINSTALL_FILE} ${PROJECT_BINARY_DIR}/cmake_uninstall.cmake @ONLY)
    ADD_CUSTOM_TARGET(uninstall ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake COMMENT "uninstall ${PROJECT_NAME}")
  ELSE(UNINSTALL_FILE)
    MESSAGE(WARNING "Could NOT find cmake_uninstall.cmake.in file -> ignoring")
  ENDIF(UNINSTALL_FILE)
ENDMACRO(SMART_COMMOBJECT_PROJECT)


############################################################
## This macro can be used in Communication Object projects
## to automatically BUILD external Communication Object
## dependencies.
## Usage: SMART_COMMOBJECT_AUTOBUILD_DEPENDENCIES()
## This macro needs to be called after SMART_COMMOBJECT_PROJECT(...)
############################################################
MACRO(SMART_COMMOBJECT_AUTOBUILD_DEPENDENCIES)
  # this option is used to activate/deactivate automatic dependency building
  OPTION(BUILD_DEPENDENCIES "Activates dependency building" ON)

  # include and build dependencies
  IF(${BUILD_DEPENDENCIES})
    INTERNAL_ADD_ALL_DEPENDENCIES(${PROJECT_NAME} ${PROJECT_SOURCE_DIR})
  ENDIF(${BUILD_DEPENDENCIES})

  # generate hash header file (using the toolchain-generated CMake script)
  FIND_FILE(GENERATE_HASH_SCRIPT GenerateHashHeader.cmake.in PATHS $ENV{SMART_ROOT_ACE}/CMakeMacros /opt/smartSoftAce/CMakeMacros)
  IF(GENERATE_HASH_SCRIPT)
    CONFIGURE_FILE(${GENERATE_HASH_SCRIPT} ${CMAKE_CURRENT_BINARY_DIR}/GenerateHashHeader.cmake @ONLY)
    ADD_CUSTOM_TARGET(${PROJECT_NAME}Hash 
      COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/GenerateHashHeader.cmake
      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/GenerateHashHeader.cmake
      BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/hash.hh ${CMAKE_CURRENT_BINARY_DIR}/HashValues.txt
      COMMENT "generate hash.hh for ${PROJECT_NAME}"
    )
  ENDIF(GENERATE_HASH_SCRIPT)

  # define a custom target that allows to build components without checking their dependencies (usefull for fast build)
  IF(NOT TARGET nodep)
    ADD_CUSTOM_TARGET(nodep COMMAND ${CMAKE_BUILD_TOOL} ${PROJECT_NAME}/fast WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Build ${PROJECT_NAME} without dependency checking")
    IF(TARGET ${PROJECT_NAME}Hash)
      ADD_DEPENDENCIES(nodep ${PROJECT_NAME}Hash)
    ENDIF(TARGET ${PROJECT_NAME}Hash)
  ENDIF(NOT TARGET nodep)

ENDMACRO(SMART_COMMOBJECT_AUTOBUILD_DEPENDENCIES)


############################################################
## This macro can be used in Communication Object projects
## to automatically LINK to external Communication Object
## dependencies.
## Usage: SMART_COMMOBJECT_AUTOLINK_DEPENDENCIES()
## This macro needs to be called after ADD_LIBRARY(...)
############################################################
MACRO(SMART_COMMOBJECT_AUTOLINK_DEPENDENCIES)
  # automatically import all specified dependencies
  INTERNAL_IMPORT_ALL_DEPENDENCIES(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})

  # if loaded, then the ${COMM_OBJECT_NAME}_DEPENDENCIES variable should be set
  IF(DEFINED ${PROJECT_NAME}_DEPENDENCIES)
    # link all external dependencies to the current comm-object
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${${PROJECT_NAME}_DEPENDENCIES})
  ENDIF(DEFINED ${PROJECT_NAME}_DEPENDENCIES)

  IF(TARGET ${PROJECT_NAME}Hash)
    ADD_DEPENDENCIES(${PROJECT_NAME} ${PROJECT_NAME}Hash)
  ENDIF(TARGET ${PROJECT_NAME}Hash)

  # ACE is the default (main) dependency
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ACE)

  # setup default include directories for this target
  TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src-gen>
    $<INSTALL_INTERFACE:include>  # <prefix>/include
  )
ENDMACRO(SMART_COMMOBJECT_AUTOLINK_DEPENDENCIES)



############################################################
## This macro can be used in Communication Object projects
## to setup a default installation configuration.
## Usage: SMART_COMMOBJECT_AUTOINSTALL()
## This macro should come last in the CMakeLists.txt file.
############################################################
MACRO(SMART_COMMOBJECT_AUTOINSTALL)
  # automatically run "make install" after regular "make" for the communication object
  ADD_CUSTOM_TARGET(autoinstall ALL COMMAND ${CMAKE_COMMAND} ARGS -P cmake_install.cmake WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Install ${PROJECT_NAME}")
  ADD_DEPENDENCIES(autoinstall ${PROJECT_NAME})

  IF(DEFINED ${PROJECT_NAME}_VERSION)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES VERSION ${${PROJECT_NAME}_VERSION} SOVERSION ${${PROJECT_NAME}_VERSION_MAJOR})
    IF(EXISTS ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake)
      INSTALL(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake DESTINATION modules)
      SMART_TRACE_GENERATED_FILE(${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake)
    ENDIF(EXISTS ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake)
  ENDIF(DEFINED ${PROJECT_NAME}_VERSION)

  # generate config file (can be used by FIND_PROJECT)
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/src-gen/${PROJECT_NAME}Config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake @ONLY)
  SMART_TRACE_GENERATED_FILE(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake)
  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake DESTINATION modules)
  # install library and target-exports
  INSTALL(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets DESTINATION lib)
  EXPORT(EXPORT ${PROJECT_NAME}Targets)
  INSTALL(EXPORT ${PROJECT_NAME}Targets DESTINATION modules)
  SMART_TRACE_GENERATED_FILE(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake)

  # install header files from 'src' and 'src-gen' folders
  FILE(GLOB SRC_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}/*.hh)
  FILE(GLOB SRC_GEN_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/src-gen/${PROJECT_NAME}/*.hh)
  INSTALL(FILES ${SRC_HDRS} ${SRC_GEN_HDRS} DESTINATION include/${PROJECT_NAME})
  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/hash.hh DESTINATION include/${PROJECT_NAME})
ENDMACRO(SMART_COMMOBJECT_AUTOINSTALL)


###################################################################
## SMART_UTILITY_PROJECT(<ProjectName> <Version>)
## This macro encapsulates the project creation procedure
## for SmartSoft-Utility
###################################################################
MACRO(SMART_UTILITY_PROJECT PROJ_NAME VERSION)
  PROJECT(${PROJ_NAME} VERSION ${VERSION})

  MESSAGE("-- UTILITY_PROJECT(${PROJECT_NAME})")

  IF(DEFINED ${PROJECT_NAME}_VERSION)
      FIND_FILE(PACKAGE_VERION_FILE PackageConfigVersion.cmake.in PATHS $ENV{SMART_ROOT_ACE}/CMakeMacros /opt/smartSoftAce/CMakeMacros)
      IF(PACKAGE_VERION_FILE)
        CONFIGURE_FILE(${PACKAGE_VERION_FILE} ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake @ONLY)
        SMART_TRACE_GENERATED_FILE("${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake")
      ENDIF(PACKAGE_VERION_FILE)
  ENDIF(DEFINED ${PROJECT_NAME}_VERSION)

  # this option is used to activate/deactivate automatic dependency building
  OPTION(BUILD_DEPENDENCIES "Activates dependency building" ON)

  # default installation prefix is set to the path given in the SMART_ROOT_ACE environment variable
  SMART_SETUP_INSTALL_PREFIX()

  FIND_FILE(UNINSTALL_FILE cmake_uninstall.cmake.in PATHS $ENV{SMART_ROOT_ACE}/CMakeMacros /opt/smartSoftAce/CMakeMacros)
  IF(UNINSTALL_FILE)
    CONFIGURE_FILE(${UNINSTALL_FILE} ${PROJECT_BINARY_DIR}/cmake_uninstall.cmake @ONLY)
    ADD_CUSTOM_TARGET(uninstall ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake COMMENT "uninstall ${PROJECT_NAME}")
  ELSE(UNINSTALL_FILE)
    MESSAGE(WARNING "Could NOT find cmake_uninstall.cmake.in file -> ignoring")
  ENDIF(UNINSTALL_FILE)

  # define a custom target that allows to build components without checking their dependencies (usefull for fast build)
  IF(NOT TARGET nodep)
    ADD_CUSTOM_TARGET(nodep COMMAND ${CMAKE_BUILD_TOOL} ${PROJECT_NAME}/fast WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Build ${PROJECT_NAME} without dependency checking")
  ENDIF(NOT TARGET nodep)
ENDMACRO(SMART_UTILITY_PROJECT PROJ_NAME VERSION)


MACRO(SMART_UTILITY_INSTALL TARGET_NAME)
  # automatically run "make install" after regular "make" for the communication object
  ADD_CUSTOM_TARGET(autoinstall ALL COMMAND ${CMAKE_COMMAND} ARGS -P cmake_install.cmake WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Install ${TARGET_NAME}")
  ADD_DEPENDENCIES(autoinstall ${TARGET_NAME})

  IF(DEFINED ${TARGET_NAME}_VERSION)
    GET_TARGET_PROPERTY(TARGET_TYPE ${TARGET_NAME} TYPE)
    IF("${TARGET_TYPE}" STREQUAL "SHARED_LIBRARY")
      SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES VERSION ${${TARGET_NAME}_VERSION} SOVERSION ${${TARGET_NAME}_VERSION_MAJOR})
    ENDIF("${TARGET_TYPE}" STREQUAL "SHARED_LIBRARY")
    IF(EXISTS ${PROJECT_BINARY_DIR}/${TARGET_NAME}ConfigVersion.cmake)
      INSTALL(FILES ${PROJECT_BINARY_DIR}/${TARGET_NAME}ConfigVersion.cmake DESTINATION modules)
    ENDIF(EXISTS ${PROJECT_BINARY_DIR}/${TARGET_NAME}ConfigVersion.cmake)
  ENDIF(DEFINED ${TARGET_NAME}_VERSION)

  IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_NAME}Config.cmake.in)
    # generate config file (can be used by FIND_PROJECT)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_NAME}Config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}Config.cmake @ONLY)
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}Config.cmake DESTINATION modules)
  ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_NAME}Config.cmake.in)

  # install library and target-exports
  INSTALL(TARGETS ${TARGET_NAME} EXPORT ${TARGET_NAME}Targets DESTINATION lib)
  EXPORT(EXPORT ${TARGET_NAME}Targets)
  INSTALL(EXPORT ${TARGET_NAME}Targets DESTINATION modules)
ENDMACRO(SMART_UTILITY_INSTALL TARGET_NAME)





