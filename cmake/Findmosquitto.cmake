# Find the mosquitto library

if(MOSQUITTO_FOUND)
    return()
endif()

find_path(MOSQUITTO_INCLUDE_DIR NAMES mosquitto.h)
find_library(
  MOSQUITTO_LIBRARY
  NAMES mosquitto
)

# Validate that include dirs and library have valid paths. Sets MOSQUITTO_FOUND.
# Stops running the finder if the library was not found.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(mosquitto DEFAULT_MSG MOSQUITTO_INCLUDE_DIR MOSQUITTO_LIBRARY)
mark_as_advanced(MOSQUITTO_INCLUDE_DIR MOSQUITTO_LIBRARY)

add_library(mosquitto::mosquitto UNKNOWN IMPORTED)
set_target_properties(mosquitto::mosquitto PROPERTIES
    IMPORTED_LOCATION "${MOSQUITTO_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${MOSQUITTO_INCLUDE_DIR}"
)
