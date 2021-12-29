# Lamp control via MQTT

This repo contains a Linux application which can automatically control lamps using MQTT according to time and the sunlight illumination as measured outside. It is intended to be run on Linux-based single-board computers like a Raspberry Pi or Beagle Bone. A configuration file is used to define the MQTT topics, timings, and thresholds; an example file can be found [here](apps/autolight/example.cfg). The application can, for example, be used to automate [Tasmota](https://tasmota.github.io/docs/) devices.

Typically lamps are automatically switched on at a given time in the morning, then they are switched off when it gets light, then switched on again when it gets dark, and switched off again at a given time. However, using computed sunset and sunrise times does not work well, because depending on the weather the sunlight can be very different: With clear sky there may easily be a factor of 10 higher irradiance than 
with overcast sky, thus humans switch on the light much later if it is clear sky compared to cloudy conditions. Therefore, a measurement of the outside sunlight intensity leads to a more natural user experience of the lighting automation.

The intended logic is achieved as follows:

* If the measured ambient sunlight falls below a given threshold, and the current time is within the "on" period, the lamp is switched on.
* If the measured ambient sunlight raises above a given threshold, or the time goes outside the "on" period, the lamp is switched off.
* Hysteresis is used to bypass measurement noise.

The program is written in C++. It requires a C++17 compatible compiler and uses CMake. The following 3rd-party libraries are necessary:

* [libmosquitto](https://mosquitto.org/api/) - `apt install libmosquitto-dev`
* [libconfig++](https://github.com/hyperrealm/libconfig) - `apt install libconfig++-dev`
* [Google Test](https://github.com/google/googletest.git) (only for the tests) - `apt install googletest`
