add_executable(tests_unit_value_action
    ../../../libs/test_helper/runner.cpp
    test_colourmap.cpp
    test_json.cpp
    test_settings_reader.cpp
    ../json_helper.cpp
    ../linearcolourmap.cpp
    ../settings_reader.cpp
)
target_link_libraries(tests_unit_value_action
   PRIVATE
      gtest
      gmock
      nlohmann_json::nlohmann_json
      libconfig::libconfig
)
