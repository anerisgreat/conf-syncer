set(CONF_SYNCER_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE TYPE STRING)

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

    #Irrelevant as this is evaluated at CMAKE and not on install
    if(TO_INSTALL)
        install(FILES "${CONF_SRC}" DESTINATION "${DSTDIR}")
        message("Will install configuration ${CONF_SRC} to ${CONF_DST}")
    else()
        message("Skipping installation of configuration ${CONF_SRC}")
    endif()
endfunction()

#____________________________________________________________________________

macro(add_managed_conf FNAME DSTDIR CONFPROJ)
    #Just for local use
    get_filename_component(CONF_FNAME ${FNAME} NAME)
    set(CONF_SRC "${CMAKE_CURRENT_LIST_DIR}/${FNAME}")
    set(CONF_DST "${DSTDIR}/${CONF_FNAME}")

    #REMOVE LATER
    project(${CONFPROJ}-${CONF_SRC})
    set(CONF_LST "${CONF_LST}${CONF_SRC}:${CONF_DST}:${PROJECT_NAME}\n")
    set(${CONFPROJ}_TXT_FILE "${CMAKE_BINARY_DIR}/${CONFPROJ}-config.txt")
    file(WRITE ${${CONFPROJ}_TXT_FILE} "${CONF_LST}")
    if(NOT ${CONF_DST}_INSTALLED)
        install(FILES ${CMAKE_BINARY_DIR}/${CONF_FNAME} DESTINATION ${DSTDIR})
    endif()
    SET(${CONF_DST}_INSTALLED ON)


    if(NOT ${CONFPROJ})
        project(${CONFPROJ}_SETUP)
        set(${CONFPROJ}_CONF_GEN ${CONFPROJ}-conf-gen CACHE TYPE STRING)
        set(${CONFPROJ}_CONF_SRC_FNAME
            ${CMAKE_CURRENT_BINARY_DIR}/${CONFPROJ})
        add_executable(${${CONFPROJ}_CONF_GEN} ${CONF_SYNCER_DIR}/lib/gen-conf-main.cxx)
        add_custom_command(OUTPUT 
                ${${CONFPROJ}_CONF_SRC_FNAME}.h
                ${${CONFPROJ}_CONF_SRC_FNAME}.c
                COMMAND ${${CONFPROJ}_CONF_GEN} ${${CONFPROJ}_CONF_SRC_FNAME}.h ${${CONFPROJ}_CONF_SRC_FNAME}.c ${${CONFPROJ}_TXT_FILE}
            DEPENDS ${${CONFPROJ}_CONF_GEN})

        project(${CONFPROJ} C CXX)

        set(${PROJECT_NAME}_INCLUDE_DIR
            "${CMAKE_CURRENT_BINARY_DIR}" CACHE TYPE STRING)

        add_library(${PROJECT_NAME} STATIC
                ${${CONFPROJ}_CONF_SRC_FNAME}.h
                ${${CONFPROJ}_CONF_SRC_FNAME}.c)

        target_include_directories(${PROJECT_NAME} 
            PUBLIC 
            ${${PROJECT_NAME}_INCLUDE_DIR}
            PRIVATE
            "${CMAKE_SOURCE_DIR}")
        SET(${CONFPROJ} ON)
    endif()
endmacro()

