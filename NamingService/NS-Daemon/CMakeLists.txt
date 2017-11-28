

ADD_EXECUTABLE(${PROJECT_NAME} NameHandler.h NameHandler.cpp NameAcceptor.h NameAcceptor.cpp RemoteNSTask.cpp RemoteNSTask.h main.cpp)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} AceSmartSoftKernel)

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin/NamingService)
