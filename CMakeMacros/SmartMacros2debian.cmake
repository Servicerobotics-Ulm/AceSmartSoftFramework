
# TODO: The current implementation is DEPRICATED. It is strongly suggested to use CPackDeb instead!!!
# For more information on CPackDeb see: https://cmake.org/cmake/help/v3.3/module/CPackDeb.html

MACRO(SMART_COMPONENT_BUILD_DEBIAN_PACKAGE PROJ_NAME VERSION)
  SMART_COMMOBJECT_BUILD_DEBIAN_PACKAGE(${PACKAGE} ${PROJ_NAME} ${VERSION})
ENDMACRO(SMART_COMPONENT_BUILD_DEBIAN_PACKAGE PROJ_NAME VERSION)


# generate debian package
# TODO: this macro needs to be refined (e.g. using CPackDeb generator)
MACRO(SMART_COMMOBJECT_BUILD_DEBIAN_PACKAGE PROJ_NAME VERSION)
  # this is the regular case for the SmartSoft MDSD toolchain v2
  ADD_CUSTOM_COMMAND(
    OUTPUT ${PROJECT_SOURCE_DIR}/debian/control
    COMMAND ${CMAKE_COMMAND} -P ${PROJECT_SOURCE_DIR}/src-gen/GenerateDebianControl.cmake DEPENDS ${PROJECT_SOURCE_DIR}/debian/control.user ${PROJECT_SOURCE_DIR}/src-gen/GenerateDebianControl.cmake COMMENT "generate control file for ${PROJECT_NAME}"
  )
  
  # if debian package generator script exists. use it to generate debian packages
  FIND_FILE(DEBIAN_GEN_FILE GenerateDebianPackage.cmake.in PATHS $ENV{SMART_ROOT_ACE}/CMakeMacros /opt/smartSoftAce/CMakeMacros)
  IF(DEBIAN_GEN_FILE)
    SET(VERSION ${VERSION} CACHE INTERNAL "Version number")
    CONFIGURE_FILE(${DEBIAN_GEN_FILE} ${PROJECT_BINARY_DIR}/GenerateDebianPackage.cmake @ONLY)
    #CMAKE_TRACE_GENERATED_FILE("${PROJECT_BINARY_DIR}/GenerateDebianPackage.cmake")
    ADD_CUSTOM_TARGET(${PROJECT_NAME}_deb_package 
      COMMAND ${CMAKE_COMMAND} -P ${PROJECT_BINARY_DIR}/GenerateDebianPackage.cmake 
      DEPENDS ${PROJECT_SOURCE_DIR}/debian/control COMMENT "generate package ${PROJ_NAME}")
  ENDIF(DEBIAN_GEN_FILE)
ENDMACRO(SMART_COMMOBJECT_BUILD_DEBIAN_PACKAGE PROJ_NAME VERSION)


# TODO: this macro needs to be refined (e.g. using CPackDeb generator)
MACRO(SMART_UTILITY_BUILD_DEBIAN_PACKAGE PROJ_NAME VERSION)
  # if debian package generator script exists. use it to generate debian packages
  FIND_FILE(DEBIAN_GEN_FILE GenerateDebianPackage.cmake.in PATHS $ENV{SMART_ROOT_ACE}/CMakeMacros /opt/smartSoftAce/CMakeMacros)
  IF(DEBIAN_GEN_FILE)
    SET(VERSION ${VERSION} CACHE INTERNAL "Version number")
    CONFIGURE_FILE(${DEBIAN_GEN_FILE} ${PROJECT_BINARY_DIR}/GenerateDebianPackage.cmake @ONLY)
    #CMAKE_TRACE_GENERATED_FILE("${PROJECT_BINARY_DIR}/GenerateDebianPackage.cmake")
    ADD_CUSTOM_TARGET(${PROJECT_NAME}_deb_package 
      COMMAND ${CMAKE_COMMAND} -P ${PROJECT_BINARY_DIR}/GenerateDebianPackage.cmake 
      COMMENT "generate package ${PROJ_NAME}")
  ENDIF(DEBIAN_GEN_FILE)   
ENDMACRO(SMART_UTILITY_BUILD_DEBIAN_PACKAGE PROJ_NAME VERSION)


