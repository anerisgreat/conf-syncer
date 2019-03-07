option(CONF_OVERRIDE "Override configuration defaults")

function(add_unmanaged_conf FNAME DSTDIR DEFAULT_OVERRIDE)
    set(CONF_SRC "${CMAKE_CURRENT_LIST_DIR}/${FNAME}")
    set(CONF_DST "${DSTDIR}/${FNAME}")

    set(TO_INSTALL_${FNAME} ON)

    if(EXISTS "${CONF_DST}")
        if(NOT DEFAULT_OVERRIDE)
            if(NOT CONF_OVERRIDE)
                set(TO_INSTALL OFF)
            endif()
        endif()
    endif()

    if(TO_INSTALL)
        install(FILES "${CONF_SRC}" DESTINATION "${DSTDIR}")
        message("Will install configuration ${CONF_SRC} to ${CONF_DST}")
    else()
        message("Skipping installation of configuration ${CONF_SRC}")
    endif()
endfunction()

#____________________________________________________________________________
message("${CONFS_FILE}")

macro(add_managed_conf FNAME DSTDIR)
    #Just for local use
    get_filename_component(CONF_FNAME ${FNAME} NAME)
    set(CONF_SRC "${CMAKE_CURRENT_LIST_DIR}/${FNAME}")
    set(CONF_DST "${DSTDIR}/${CONF_FNAME}")

    set(CONF_LST "${CONF_LST}${CONF_SRC}:${CONF_DST}:${PROJECT_NAME}\n")
    file(WRITE "${CMAKE_BINARY_DIR}/conf-syncer-config.txt" "${CONF_LST}")
    if(NOT ${CONF_DST}_INSTALLED)
        install(FILES ${CMAKE_BINARY_DIR}/${CONF_FNAME} DESTINATION ${DSTDIR})
    endif()
    SET(${CONF_DST}_INSTALLED ON)
endmacro()

set(CONF_PROJECT ${CMAKE_PROJECT_NAME}-conf)

project(${CONF_PROJECT}_SETUP)
add_executable(ConfSyncerGen lib/gen-conf-main.cxx)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/conf.h ${CMAKE_CURRENT_BINARY_DIR}/conf.c
    COMMAND ConfSyncerGen ${CMAKE_CURRENT_BINARY_DIR}/conf.h {CMAKE_CURRENT_BINARY_DIR}/conf.c "${CMAKE_BINARY_DIR}/conf-syncer-config.txt"
    DEPENDS ConfSyncerGen)

project(${CONF_PROJECT} C CXX)

set(${PROJECT_NAME}_INCLUDE_DIR
    "${CMAKE_CURRENT_BINARY_DIR}" CACHE TYPE STRING)

add_library(${PROJECT_NAME}
    ${CMAKE_CURRENT_BINARY_DIR}/conf.h
    ${CMAKE_CURRENT_BINARY_DIR}/conf.c
    )

target_include_directories(${PROJECT_NAME} 
    PUBLIC 
    ${${PROJECT_NAME}_INCLUDE_DIR}
    PRIVATE
    "${CMAKE_SOURCE_DIR}")
