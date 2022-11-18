if(LIBCONFIG_FOUND)
    return()
endif()

find_path(LIBCONFIG_INCLUDE_DIR NAMES libconfig.h++)
find_library(
  LIBCONFIG_LIBRARY
  NAMES config++
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libconfig DEFAULT_MSG LIBCONFIG_INCLUDE_DIR LIBCONFIG_LIBRARY)
mark_as_advanced(LIBCONFIG_INCLUDE_DIR LIBCONFIG_LIBRARY)

if(LIBCONFIG_FOUND)
    add_library(libconfig::libconfig UNKNOWN IMPORTED)
    set_target_properties(libconfig::libconfig PROPERTIES
        IMPORTED_LOCATION "${LIBCONFIG_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${LIBCONFIG_INCLUDE_DIR}"
    )
endif()
