# executables
add_executable(value_action
    value_action.cpp
    json_helper.h
    json_helper.cpp
    linearcolourmap.h
    linearcolourmap.cpp
    mqtt.h
    mqtt.cpp
    settings.h
    settings_reader.h
    settings_reader.cpp
)
target_link_libraries(value_action
   PRIVATE
      mqttbase
      nlohmann_json::nlohmann_json
      libconfig::libconfig
)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
