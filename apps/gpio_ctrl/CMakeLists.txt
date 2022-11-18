# executables
add_executable(gpio_ctrl
    gpio_ctrl.cpp
    mqtt_gpio.h
    mqtt_gpio.cpp
    settings.h
    settings_reader.h
    settings_reader.cpp
)
target_link_libraries(gpio_ctrl
   PRIVATE
      mqttbase
      libconfig::libconfig
      libgpiod::libgpiodcxx
)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
