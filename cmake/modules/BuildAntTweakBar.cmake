if(WIN32)
    add_library(anttweakbar SHARED IMPORTED)

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ANTTWEAKBAR_LIB_PATH ${PROJECT_SOURCE_DIR}/ext/AntTweakBar/lib/AntTweakBar64.dll)

        set_target_properties(anttweakbar PROPERTIES
                IMPORTED_LOCATION ${ANTTWEAKBAR_LIB_PATH}
                IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/ext/AntTweakBar/lib/AntTweakBar64.lib
                INTERFACE_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/ext/AntTweakBar/include)

    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(ANTTWEAKBAR_LIB_PATH ${PROJECT_SOURCE_DIR}/ext/AntTweakBar/lib/AntTweakBar.dll)

        set_target_properties(anttweakbar PROPERTIES
                IMPORTED_LOCATION ${ANTTWEAKBAR_LIB_PATH}
                IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/ext/AntTweakBar/lib/AntTweakBar.lib
                INTERFACE_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/ext/AntTweakBar/include)
    endif()

    file(COPY ${ANTTWEAKBAR_LIB_PATH} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
else()
    include(ExternalProject)
    include(ProcessorCount)

    processorcount(CPU_COUNT)
    if(CPU_COUNT EQUAL 0)
        set(CPU_COUNT 1)
    endif()

    set(ANTTWEAKBAR_LIB_PATH ${PROJECT_SOURCE_DIR}/ext/AntTweakBar/lib/${CMAKE_SHARED_LIBRARY_PREFIX}AntTweakBar${CMAKE_SHARED_LIBRARY_SUFFIX})
    set(MAKEJOBS ${CPU_COUNT})
    find_program(MAKE NAMES gmake nmake make)
    set(EXEC ${MAKE} -j${MAKEJOBS})

    ExternalProject_Add(anttweakbar-external
            SOURCE_DIR ${PROJECT_SOURCE_DIR}/ext/AntTweakBar/src
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ${EXEC} -C <SOURCE_DIR>
            INSTALL_COMMAND cp ${ANTTWEAKBAR_LIB_PATH} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            LOG_BUILD 1
            )

    ExternalProject_Add_Step(anttweakbar-external strip-binary
            DEPENDEES build
            COMMAND strip -s ${ANTTWEAKBAR_LIB_PATH}
            )

    add_library(anttweakbar SHARED IMPORTED)
    set_target_properties(anttweakbar PROPERTIES
            IMPORTED_LOCATION ${ANTTWEAKBAR_LIB_PATH}
            INTERFACE_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/ext/AntTweakBar/include)
    add_dependencies(${PROJECT_NAME} anttweakbar-external)
endif()