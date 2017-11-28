CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(ExampleInterfaceClasses)

# find ACE library package
IF(NOT TARGET ACE)
  FIND_PACKAGE(ACE 6.0.2 PATHS $ENV{SMART_ROOT_ACE} /opt/smartSoftAce)
ENDIF(NOT TARGET ACE)

SET(libSrc
  commExampleEvent1.cc
  commExampleEvent2.cc
  commExamplePrint.cc
  commExampleResult.cc
  commExampleTime.cc
  commExampleTypes.cc
  commExampleValues.cc
)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${libSrc})

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ACE)

#INSTALL(TARGETS ${PROJECT_NAME} DESTINATION lib)
