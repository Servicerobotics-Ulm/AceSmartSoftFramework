OPTION(WITH_SYSTEM_D "use systemd to signal fully initialized namingservice" OFF)
IF(WITH_SYSTEM_D)
    INCLUDE(FindPkgConfig)
    pkg_check_modules(SYSTEMD "libsystemd" REQUIRED)
    ADD_DEFINITIONS(-DWITH_SYSTEM_D)
ENDIF(WITH_SYSTEM_D)

ADD_EXECUTABLE(${PROJECT_NAME} NameHandler.h NameHandler.cpp NameAcceptor.h NameAcceptor.cpp RemoteNSTask.cpp RemoteNSTask.h main.cpp)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} AceSmartSoftKernel ${SYSTEMD_LIBRARIES})

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin/NamingService)
