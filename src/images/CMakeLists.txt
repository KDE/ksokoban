# SPDX-FileCopyrightText: 2022 Friedrich W. H. Kossebau <kossebau@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

find_package(POVRay)
set_package_properties(POVRay PROPERTIES
    TYPE OPTIONAL
    PURPOSE "For rendering the games PNG images"
)

if(NOT POVRay_FOUND)
    return()
endif()


function(add_rendered_png output)
    cmake_parse_arguments(ARGS "" "SOURCE;WORKING_DIRECTORY;RESOLUTION" "DEPENDS" ${ARGN})

    if (NOT ARGS_SOURCE)
        message(FATAL_ERROR "SOURCE argument missing")
    endif()

    if (NOT ARGS_RESOLUTION)
        set(resolution "+W192" "+H192")
    elseif (ARGS_RESOLUTION STREQUAL "STONE")
        set(resolution "+W192" "+H96")
    elseif (ARGS_RESOLUTION STREQUAL "HALFSTONE")
        set(resolution "+W96" "+H96")
    else()
        message(FATAL_ERROR "Unknown RESOLUTION value: ${ARGS_RESOLUTION}")
    endif()

    # no antialias
    #set(antialias)
    # normal antialias
    #set(antialias "Antialias=on")
    # slow antialias
    set(antialias "Antialias=on" "Antialias_Threshold=0.0" "Antialias_Depth=9")
    # used version assumed by the creation date of the files
    set(versionhint "+MV3.0")

    set(_workdir_arg)
    if(ARGS_WORKING_DIRECTORY)
        set(_workdir_arg "WORKING_DIRECTORY" ${ARGS_WORKING_DIRECTORY})
    endif()

    if(NOT IS_ABSOLUTE output)
        string(PREPEND output "${CMAKE_CURRENT_SOURCE_DIR}/")
    endif()

    set(source ${ARGS_SOURCE})
    if(NOT IS_ABSOLUTE source)
        string(PREPEND source "${CMAKE_CURRENT_SOURCE_DIR}/")
    endif()

    get_filename_component(include ${source} DIRECTORY)

    set(depends)
    foreach(d ${ARGS_DEPENDS})
        if(NOT IS_ABSOLUTE d)
            string(PREPEND d "${CMAKE_CURRENT_SOURCE_DIR}/")
        endif()
        list(APPEND depends ${d})
    endforeach()

    # redirect all logging to file (openSUSE POV-Ray 3.7.0.10.unofficial fails to disable console though)
    get_filename_component(logbasename ${output} NAME)
    set(logredirect "-GA${logbasename}.log")

    add_custom_command(OUTPUT ${output}
        COMMENT "Generating ${output}"
        ${_workdir_arg}
        COMMAND POVRay::POVRay ${antialias} ${versionhint} ${resolution} ${logredirect} +I${source} +L${include} +O${output}
        VERBATIM
        MAIN_DEPENDENCY ${sources}
        DEPENDS ${depends}
    )
endfunction()

add_rendered_png(treasure.png SOURCE treasure.pov DEPENDS goal.pov floor_common.inc)
add_rendered_png(object.png SOURCE object.pov DEPENDS floor_common.inc)
add_rendered_png(goal.png SOURCE goal.pov DEPENDS floor_common.inc)

add_rendered_png(man.png SOURCE man.pov DEPENDS man_common.inc floor_common.inc)
add_rendered_png(saveman.png SOURCE saveman.pov DEPENDS man_common.inc goal.pov floor_common.inc)

add_rendered_png(halfstone_1.png SOURCE halfstone_1.pov DEPENDS stone_common.inc RESOLUTION HALFSTONE)
add_rendered_png(halfstone_2.png SOURCE halfstone_2.pov DEPENDS stone_common.inc RESOLUTION HALFSTONE)
add_rendered_png(halfstone_3.png SOURCE halfstone_3.pov DEPENDS stone_common.inc RESOLUTION HALFSTONE)
add_rendered_png(halfstone_4.png SOURCE halfstone_4.pov DEPENDS stone_common.inc RESOLUTION HALFSTONE)

add_rendered_png(stone_1.png SOURCE stone_1.pov DEPENDS stone_common.inc RESOLUTION STONE)
add_rendered_png(stone_2.png SOURCE stone_2.pov DEPENDS stone_common.inc RESOLUTION STONE)
add_rendered_png(stone_3.png SOURCE stone_3.pov DEPENDS stone_common.inc RESOLUTION STONE)
add_rendered_png(stone_4.png SOURCE stone_4.pov DEPENDS stone_common.inc RESOLUTION STONE)
add_rendered_png(stone_5.png SOURCE stone_5.pov DEPENDS stone_common.inc RESOLUTION STONE)
add_rendered_png(stone_6.png SOURCE stone_6.pov DEPENDS stone_common.inc RESOLUTION STONE)
