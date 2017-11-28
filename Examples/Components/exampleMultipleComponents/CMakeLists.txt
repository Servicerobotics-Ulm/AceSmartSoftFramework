SET(TargetName exampleMultipleComponents)
SET(srcs 
  ClientComponent.cpp
  ClientTask.cpp
  ServerComponent.cpp
  ServerTask.cpp
  ShutdownManager.cpp
  multi.cpp
)

FILE(GLOB headerFiles *.h)

ADD_EXECUTABLE(${TargetName} ${srcs} ${headerFiles})
ADD_DEPENDENCIES(${TargetName} AceSmartSoftKernel ExampleInterfaceClasses)
TARGET_LINK_LIBRARIES(${TargetName} AceSmartSoftKernel ExampleInterfaceClasses)

INSTALL(TARGETS ${TargetName} DESTINATION bin)
