# executables
add_executable(autolight
    autolight.cpp
    lamp_settings.h
    settings.h
    lamp_control.h
    lamp_control.cpp
    mqtt.h
    mqtt.cpp
    settings_reader.h
    settings_reader.cpp
)
target_link_libraries(autolight
   PRIVATE
      mqttbase
      libconfig::libconfig
)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
