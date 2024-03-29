install( FILES
    ksokoban_classic.svg
    ksokoban_classic.desktop
    #ksokoban_classic_preview.png
    DESTINATION  ${KDE_INSTALL_DATADIR}/ksokoban/themes
)

function(install_classic_sprite_image)
    cmake_parse_arguments(ARGS "" "SOURCE_DIR;NAMESPACE" "IMAGES" ${ARGN})

    foreach(image ${ARGS_IMAGES})
        install(FILES "${ARGS_SOURCE_DIR}/${image}"
            RENAME "${ARGS_NAMESPACE}${image}"
            DESTINATION  ${KDE_INSTALL_DATADIR}/ksokoban/themes
        )
    endforeach()
endfunction()

install_classic_sprite_image(
    NAMESPACE "ksokoban_classic_"
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/images"
    IMAGES
        goal.png
        man.png
        object.png
        saveman.png
        treasure.png
        halfstone_1.png
        halfstone_2.png
        halfstone_3.png
        halfstone_4.png
        stone_1.png
        stone_2.png
        stone_3.png
        stone_4.png
        stone_5.png
        stone_6.png
        starfield.png
)
