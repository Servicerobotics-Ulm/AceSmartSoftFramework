MACRO(LoadSmartSoftAPI SmartSoftApiDIR)
    #mktemp dir in build tree
    FILE(MAKE_DIRECTORY "${SmartSoftApiDIR}" "${SmartSoftApiDIR}download" "${SmartSoftApiDIR}download/build")

    #generate false dependency project
    SET(CMAKE_LIST_CONTENT "
      CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

      INCLUDE(ExternalProject)
      # checkout and build the SmartSoftAPI
      ExternalProject_Add(SmartSoftAPI
        PREFIX SmartSoftAPI
        GIT_REPOSITORY https://github.com/Servicerobotics-Ulm/SmartSoftComponentDeveloperAPIcpp.git
        GIT_TAG develop
        BINARY_DIR ${SmartSoftApiDIR}
        INSTALL_COMMAND \"\" # don't autoinstall this (leave the sources in build-tree)
      )"
    )

    FILE(WRITE ${SmartSoftApiDIR}download/CMakeLists.txt "${CMAKE_LIST_CONTENT}")

    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" ..
        WORKING_DIRECTORY ${SmartSoftApiDIR}download/build
        )
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${SmartSoftApiDIR}download/build
        )

    INCLUDE(${SmartSoftApiDIR}/SmartSoft_CD_API/SmartSoft_CD_APITarget.cmake)
ENDMACRO(LoadSmartSoftAPI SmartSoftApiDIR)
