CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

INCLUDE(ExternalProject)

############################################################
## internal API (do not use in SmartSoft projects directly)
############################################################
FUNCTION(INTERNAL_FIND_PACKAGE PACKAGE_NAME)
  # check whether the environment variable SMART_PACKAGE_PATH is defined
  # - if so, the paths defined within this variable are used to search for SmartSoft packages
  # - it is expected that the packages found through this variable contain complete source trees
  # - all packages found using this environment variable are used to include and link this package 
  # --> AND in particular to automatically compile the reference in case it is outdated
  IF(DEFINED ENV{SMART_PACKAGE_PATH})
    # replace the ":" path separator by ";" which is a standard separator for list elements in cmake
    STRING(REPLACE ":" ";" PATH_LIST $ENV{SMART_PACKAGE_PATH})

    # iterate all package paths and for each root folder do:
    # - find all <package-name>Config.cmake.in files rekursivelly
    # - add these files to the SMART_CONFIG_FILES list
    FOREACH(DIR IN LISTS PATH_LIST)
      # MESSAGE("PACKAGE_PATH: ${DIR}")
      FILE(GLOB_RECURSE CONFIG_FILES ${DIR}/${PACKAGE_NAME}Config.cmake.in)
      LIST(LENGTH CONFIG_FILES LENGTH)
      IF(${LENGTH} GREATER 0)
        # use the first element that has been found (ignore potential multiple matches)
        LIST(GET CONFIG_FILES 0 ${PACKAGE_NAME}ConfigFile)
      ENDIF(${LENGTH} GREATER 0)
    ENDFOREACH(DIR)
  ENDIF(DEFINED ENV{SMART_PACKAGE_PATH})

  IF(${PACKAGE_NAME}ConfigFile)
    # get the root directory of the comm-object
    GET_FILENAME_COMPONENT(PACKAGE_ROOT ${${PACKAGE_NAME}ConfigFile} DIRECTORY)
    IF(EXISTS ${PACKAGE_ROOT}/CMakeLists.txt)
      # this is the return value of this function
      SET(${PACKAGE_NAME}_SOURCE_DIR ${PACKAGE_ROOT} PARENT_SCOPE)
      SET(${PACKAGE_NAME}_FOUND TRUE PARENT_SCOPE)
    ELSE(EXISTS ${PACKAGE_ROOT}/CMakeLists.txt)
      # get the parent folder (assuming that this is the atual root folder)
      GET_FILENAME_COMPONENT(PACKAGE_ROOT ${PACKAGE_ROOT} DIRECTORY)
      IF(EXISTS ${PACKAGE_ROOT}/CMakeLists.txt)
        # this is the return value of this function
        SET(${PACKAGE_NAME}_SOURCE_DIR ${PACKAGE_ROOT} PARENT_SCOPE)
        SET(${PACKAGE_NAME}_FOUND TRUE PARENT_SCOPE)
      ENDIF(EXISTS ${PACKAGE_ROOT}/CMakeLists.txt) 
    ENDIF(EXISTS ${PACKAGE_ROOT}/CMakeLists.txt)
  ENDIF(${PACKAGE_NAME}ConfigFile)

  IF(NOT ${PACKAGE_NAME}_SOURCE_DIR)
    # if no source tree is found, we try to find the installed version as a second search strategy
    FIND_PACKAGE(${PACKAGE_NAME} PATHS $ENV{SMART_ROOT_ACE}/modules /opt/smartSoftAce/modules)
  ENDIF(NOT ${PACKAGE_NAME}_SOURCE_DIR)

  IF(NOT ${PACKAGE_NAME}_FOUND)
    MESSAGE(FATAL_ERROR "Package ${PACKAGE_NAME} NOT FOUND! Make sure it is either checked out under SMART_PACKAGE_PATH with sources, or it is installed in SMART_ROOT_ACE/modules or /opt/smartSoftAce/modules")
  ENDIF(NOT ${PACKAGE_NAME}_FOUND)
ENDFUNCTION(INTERNAL_FIND_PACKAGE PACKAGE_NAME)

############################################################
## internal API (do not use in SmartSoft projects directly)
############################################################
MACRO(INTERNAL_CMAKE_CONFIGURE PACKAGE_ROOT)
  # create the build folder (if not existing)
  IF(NOT EXISTS ${PACKAGE_ROOT}/build)
    MESSAGE("-- Create folder ${PACKAGE_ROOT}/build")
    EXECUTE_PROCESS(
      COMMAND ${CMAKE_COMMAND} -E make_directory ${PACKAGE_ROOT}/build
    )
  ENDIF(NOT EXISTS ${PACKAGE_ROOT}/build)

  # execute CMake configure command for the external project
  # this will generate all required configuration files
  IF(NOT EXISTS ${PACKAGE_ROOT}/build/CMakeCache.txt)
    MESSAGE("-- Execute CMake in ${PACKAGE_ROOT}/build")
    EXECUTE_PROCESS(
      COMMAND ${CMAKE_COMMAND} ..
      WORKING_DIRECTORY ${PACKAGE_ROOT}/build
    )
  ENDIF(NOT EXISTS ${PACKAGE_ROOT}/build/CMakeCache.txt)
ENDMACRO(INTERNAL_CMAKE_CONFIGURE PACKAGE_ROOT)

############################################################
## internal API (do not use in SmartSoft projects directly)
############################################################
MACRO(INTERNAL_ADD_ALL_DEPENDENCIES PACKAGE_NAME PACKAGE_ROOT)
  # configure and (optionally) include the dependencies file
  INTERNAL_CONFIGURE_DEPENDENCIES_FILE(${PACKAGE_NAME} ${PACKAGE_ROOT})

  # if loaded, then the ${PACKAGE_NAME}_DEPENDENCIES variable should become available
  IF(DEFINED ${PACKAGE_NAME}_DEPENDENCIES)
    # for-each external package-dependency, call INTERNAL_ADD_PACKAGE(...)
    FOREACH(DEPENDENCY ${${PACKAGE_NAME}_DEPENDENCIES})
      # add package dependencies recursivelly (to add the dependencies of the dependencies, etc.)
      INTERNAL_ADD_PACKAGE(${DEPENDENCY})
      # collect the direct dependencies in an extra list (will be used to create the current package)
      STRING(CONCAT EXTERNAL_DEP ${DEPENDENCY} "External")
      LIST(APPEND ${PACKAGE_NAME}_EXTERNAL_DEPENDENCIES ${EXTERNAL_DEP})
    ENDFOREACH()
    # remove potential duplicates
    LIST(REMOVE_DUPLICATES ${PACKAGE_NAME}_EXTERNAL_DEPENDENCIES)
  ENDIF(DEFINED ${PACKAGE_NAME}_DEPENDENCIES)
ENDMACRO(INTERNAL_ADD_ALL_DEPENDENCIES PACKAGE_NAME PACKAGE_ROOT)


############################################################
## internal API (do not use in SmartSoft projects directly)
############################################################
MACRO(INTERNAL_ADD_PACKAGE PACKAGE_NAME)
  # only add an ExternalProject if it has not yet been defined
  IF(NOT TARGET ${PACKAGE_NAME}External)
    # An optional second argument can be used to provide an alternative root folder 
    # (which should be an absolute path to a folder that contains the main 
    #  CMakeLists.txt file of the external package)
    IF(${ARGC} GREATER 1)
      # local root folder given using the optional argument
      SET(${PACKAGE_NAME}_SOURCE_DIR ${ARGV1})
    ELSEIF(EXISTS ${CMAKE_CURRENT_LIST_DIR}/${PACKAGE_NAME}/CMakeLists.txt)
      # the default behavior is to assume that the local root folder 
      # is named the same as the PACKAGE_NAME
      SET(${PACKAGE_NAME}_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/${PACKAGE_NAME})
    ELSEIF(EXISTS ${CMAKE_CURRENT_LIST_DIR}/${PACKAGE_NAME}/smartsoft/CMakeLists.txt)
      # some package use the smartsoft subfolder as the main CMake root folder
      SET(${PACKAGE_NAME}_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/${PACKAGE_NAME}/smartsoft)
    ELSE()
      # search for the external definition
      INTERNAL_FIND_PACKAGE(${PACKAGE_NAME})
    ENDIF()

    # check if the given root folder contains the file CMakeLists.txt
    IF(NOT EXISTS ${${PACKAGE_NAME}_SOURCE_DIR}/CMakeLists.txt)
      MESSAGE(FATAL_ERROR "Build tree for package ${PACKAGE_NAME} NOT FOUND!\nHint: use the cmake flag -DBUILD_DEPENDENCIES=OFF if you don't want to autobuild external dependencies.")
    ENDIF(NOT EXISTS ${${PACKAGE_NAME}_SOURCE_DIR}/CMakeLists.txt)

    # run CMake configure command if needed
    INTERNAL_CMAKE_CONFIGURE(${${PACKAGE_NAME}_SOURCE_DIR})

    # add all package dependencies
    # this macro will call INTERNAL_ADD_PACKAGE recursivelly
    # to collect all dependencies and the dependencies of the
    # dependencies, etc.
    INTERNAL_ADD_ALL_DEPENDENCIES(${PACKAGE_NAME} ${${PACKAGE_NAME}_SOURCE_DIR})

    #MESSAGE("${PACKAGE_NAME}_EXTERNAL_DEPENDENCIES: ${${PACKAGE_NAME}_EXTERNAL_DEPENDENCIES}")

    # add package as an ExternalProject
    ExternalProject_Add(${PACKAGE_NAME}External
      DEPENDS ${${PACKAGE_NAME}_EXTERNAL_DEPENDENCIES}
      PREFIX ${PACKAGE_NAME}
      SOURCE_DIR ${${PACKAGE_NAME}_SOURCE_DIR}
      BINARY_DIR ${${PACKAGE_NAME}_SOURCE_DIR}/build
      CMAKE_ARGS -DBUILD_DEPENDENCIES=OFF
      INSTALL_COMMAND ""
    )

    # reset the DBUILD_DEPENDENCIES back to ON (so the local build considers the dependencies again)
    ExternalProject_Add_Step(
      ${PACKAGE_NAME}External
      ${PACKAGE_NAME}Step1
      DEPENDEES install
      WORKING_DIRECTORY ${${PACKAGE_NAME}_SOURCE_DIR}/build
      COMMAND ${CMAKE_COMMAND} -DBUILD_DEPENDENCIES=ON .
      COMMENT "Reset BUILD_DEPENDENCIES of '${PACKAGE_NAME}External'"
    )

    # get STAMP_DIR folder of the created ExternalProject
    ExternalProject_Get_Property(${PACKAGE_NAME}External STAMP_DIR)
    # touch file ${PACKAGE_NAME}External-update in STAMP_DIR in order to trigger updating the external project on next build
    ExternalProject_Add_Step(
      ${PACKAGE_NAME}External
      ${PACKAGE_NAME}Step2
      DEPENDEES ${PACKAGE_NAME}Step1
      WORKING_DIRECTORY ${STAMP_DIR}
      COMMAND ${CMAKE_COMMAND} -E touch ${PACKAGE_NAME}External-update
      COMMENT "Touch file '${PACKAGE_NAME}External-update' to trigger configure on next build"
    )
  ENDIF(NOT TARGET ${PACKAGE_NAME}External)
ENDMACRO(INTERNAL_ADD_PACKAGE PACKAGE_NAME)

############################################################
## internal API (do not use in SmartSoft projects directly)
############################################################
MACRO(INTERNAL_CONFIGURE_DEPENDENCIES_FILE PACKAGE_NAME PACKAGE_ROOT)
  IF(EXISTS ${PACKAGE_ROOT}/src-gen/${PACKAGE_NAME}Dependencies.cmake.in)
    # configure depndencies file
    CONFIGURE_FILE(
      ${PACKAGE_ROOT}/src-gen/${PACKAGE_NAME}Dependencies.cmake.in
      ${PACKAGE_ROOT}/build/${PACKAGE_NAME}Dependencies.cmake
      @ONLY
    )
  ENDIF(EXISTS ${PACKAGE_ROOT}/src-gen/${PACKAGE_NAME}Dependencies.cmake.in)

  #try to include the dependencies file (if one is available)
  INCLUDE(${PACKAGE_ROOT}/build/${PACKAGE_NAME}Dependencies.cmake OPTIONAL)
ENDMACRO(INTERNAL_CONFIGURE_DEPENDENCIES_FILE PACKAGE_NAME PACKAGE_ROOT)

############################################################
## internal API (do not use in SmartSoft projects directly)
############################################################
MACRO(INTERNAL_IMPORT_ALL_DEPENDENCIES PACKAGE_NAME PACKAGE_ROOT)
  # configure and (optionally) include the dependencies file
  INTERNAL_CONFIGURE_DEPENDENCIES_FILE(${PACKAGE_NAME} ${PACKAGE_ROOT})

  # if loaded, then the ${PACKAGE_NAME}_DEPENDENCIES variable should become available
  IF(DEFINED ${PACKAGE_NAME}_DEPENDENCIES)
    # for-each external package-dependency, call INTERNAL_IMPORT_PACKAGE(...)
    FOREACH(DEPENDENCY ${${PACKAGE_NAME}_DEPENDENCIES})
      # add package dependencies recursivelly (to add the dependencies of the dependencies, etc.)
      INTERNAL_IMPORT_PACKAGE(${DEPENDENCY})
    ENDFOREACH()
  ENDIF(DEFINED ${PACKAGE_NAME}_DEPENDENCIES)
ENDMACRO(INTERNAL_IMPORT_ALL_DEPENDENCIES PACKAGE_NAME PACKAGE_ROOT)

############################################################
## internal API (do not use in SmartSoft projects directly)
############################################################
MACRO(INTERNAL_IMPORT_PACKAGE PACKAGE_NAME)
  IF(NOT TARGET ${PACKAGE_NAME})
    # try finding the package
    INTERNAL_FIND_PACKAGE(${PACKAGE_NAME})

    IF(${PACKAGE_NAME}_SOURCE_DIR)
      #package is found with sources, so the build tree version is used

      # run CMake configure command if needed
      INTERNAL_CMAKE_CONFIGURE(${${PACKAGE_NAME}_SOURCE_DIR})

      # include ${PACKAGE_NAME}Config.cmake file from the build tree of the external project
      MESSAGE("-- Include ${${PACKAGE_NAME}_SOURCE_DIR}/build/${PACKAGE_NAME}Config.cmake")
      INCLUDE(${${PACKAGE_NAME}_SOURCE_DIR}/build/${PACKAGE_NAME}Config.cmake)

      # make sure that the external project is automatically rebuilt for the imported target
      IF(TARGET ${PACKAGE_NAME}External)
        ADD_DEPENDENCIES(${PACKAGE_NAME} ${PACKAGE_NAME}External)
      ENDIF(TARGET ${PACKAGE_NAME}External)
    ELSEIF(${PACKAGE_NAME}_DIR)
      # package is found as installed version, so we use then installation tree
      # we don't need to actually include anything as this is already done by the find_package command implicitly
      MESSAGE("-- Found installed package ${PACKAGE_NAME} in ${${PACKAGE_NAME}_DIR}")
    ELSE()
      MESSAGE(FATAL_ERROR "Package {${PACKAGE_NAME} NOT FOUND!")
    ENDIF()
  ENDIF(NOT TARGET ${PACKAGE_NAME})
ENDMACRO(INTERNAL_IMPORT_PACKAGE PACKAGE_NAME)

