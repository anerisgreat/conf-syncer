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

macro(add_managed_conf FNAME DSTDIR)
    #Just for local use
    get_filename_component(CONF_FNAME ${FNAME} NAME)
    set(CONF_SRC "${CMAKE_CURRENT_LIST_DIR}/${FNAME}")
    set(CONF_DST "${DSTDIR}/${CONF_FNAME}")

    #list(APPEND ${CONF_LISTNAME} "${CONF_SRC}")
    set(CONF_GEN_LIST "${CONF_GEN_LIST}\"${CONF_SRC}\":\"${CONF_DST}\";")
    message("${CONF_GEN_LIST}")
    #set(${CONF_LISTNAME} "TITS")

    #message("${${CONF_DST}_MANAGED}")
    #if(NOT ${DIST_DIR}_INIT)
    #    #File still unmanaged
    #    SET(${DIST_DIR}_INIT TRUE)
    #    add_custom_target( FinalMessage ALL
    #        ${CMAKE_COMMAND} -E cmake_echo_color --cyan "${${CONF_LISTNAME}} !"
    #        COMMENT "Final Message" )
    #endif()
endmacro()
