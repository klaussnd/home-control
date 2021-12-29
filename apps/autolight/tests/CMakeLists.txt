add_executable(tests_unit
    runner.cpp
    test_output.cpp
    test_output_functions.cpp
    test_settings_reader.cpp
    helper.cpp
    ../lamp_control.cpp
    ../settings_reader.cpp
)
target_link_libraries(tests_unit
   PRIVATE
      gtest
      gmock
      libconfig::libconfig
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../example.cfg
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
