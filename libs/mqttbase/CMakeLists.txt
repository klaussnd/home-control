# MQTT helper library
add_library(mqttbase STATIC
   include/mqttbase/MqttClient.h
   include/mqttbase/MqttCallback.h
   include/mqttbase/MultiMqttCallback.h
   include/mqttbase/helper.h
   src/MqttClient.cpp
   src/helper.cpp
)
target_include_directories(mqttbase
   PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(mqttbase
   PRIVATE
      mosquitto::mosquitto
)
