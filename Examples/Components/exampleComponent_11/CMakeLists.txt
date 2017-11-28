SET(TargetName exampleComponent11)
SET(srcs smartExampleComponent11.cc)

ADD_EXECUTABLE(${TargetName} ${srcs})
ADD_DEPENDENCIES(${TargetName} AceSmartSoftKernel ExampleInterfaceClasses)
TARGET_LINK_LIBRARIES(${TargetName} AceSmartSoftKernel ExampleInterfaceClasses)

INSTALL(TARGETS ${TargetName} DESTINATION bin)
