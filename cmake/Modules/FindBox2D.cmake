# - Try to find Box2D
# Once done this will define
#  BOX2D_FOUND - System has Box2D
#  BOX2D_INCLUDE_DIRS - The Box2D include directories
#  BOX2D_LIBRARIES - The libraries needed to use Box2D
#  BOX2D_DEFINITIONS - Compiler switches required for using Box2D

find_package(PkgConfig)
pkg_check_modules(PC_BOX2D QUIET Box2D)
set(BOX2D_INCLUDE_DIRS ${PC_BOX2D_INCLUDE_DIRS})
set(BOX2D_DEFINITIONS ${PC_BOX2D_CFLAGS_OTHER})

find_library(BOX2D_LIBRARY NAMES Box2D
             HINTS ${PC_BOX2D_LIBDIR} ${PC_BOX2D_LIBRARY_DIRS})
set(BOX2D_LIBRARIES ${BOX2D_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Box2D DEFAULT_MSG BOX2D_LIBRARY BOX2D_INCLUDE_DIRS)

mark_as_advanced(BOX2D_INCLUDE_DIRS BOX2D_LIBRARY)
