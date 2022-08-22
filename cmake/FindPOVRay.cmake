# SPDX-FileCopyrightText: 2022 Friedrich W. H. Kossebau <kossebau@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

#[=======================================================================[.rst:
FindPOVRay
----------

Try to find POVRay.

If the POVRay executable is not in your PATH, you can provide
an alternative name or full path location with the ``POVRay_EXECUTABLE``
variable.

This will define the following variables:

``POVRay_FOUND``
    TRUE if POVRay is available

``POVRay_EXECUTABLE``
    Path to POVRay executable

If ``POVRay_FOUND`` is TRUE, it will also define the following imported
target:

``POVRay::POVRay``
    Path to POVRay executable
#]=======================================================================]

find_program(POVRay_EXECUTABLE NAMES povray)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(POVRay
    FOUND_VAR
        POVRay_FOUND
    REQUIRED_VARS
        POVRay_EXECUTABLE
)
mark_as_advanced(POVRay_EXECUTABLE)

if(NOT TARGET POVRay::POVRay AND POVRay_FOUND)
    add_executable(POVRay::POVRay IMPORTED)
    set_target_properties(POVRay::POVRay PROPERTIES
        IMPORTED_LOCATION "${POVRay_EXECUTABLE}"
    )
endif()

set_package_properties(POVRay PROPERTIES
    URL "https://www.povray.org/"
    DESCRIPTION "Tool for creating three-dimensional graphics"
)
