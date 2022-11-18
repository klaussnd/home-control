# Find the gpiod library

if (LIBGPIOD_FOUND)
    return()
endif()

find_path(LIBGPIOD_INCLUDE_DIR gpiod.hpp PATH_SUFFIXES include)
find_library(LIBGPIODCXX_LIB NAMES gpiodcxx)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libgpiod
    DEFAULT_MSG
    LIBGPIODCXX_LIB LIBGPIOD_INCLUDE_DIR)

if(LIBGPIOD_FOUND)
    add_library(libgpiod::libgpiodcxx UNKNOWN IMPORTED)
    set_target_properties(libgpiod::libgpiodcxx PROPERTIES
        IMPORTED_LOCATION "${LIBGPIODCXX_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${LIBGPIOD_INCLUDE_DIR}"
    )
endif()
