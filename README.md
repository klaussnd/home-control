# Home control helpers

This repo contains applications to help you control your home. They are intended to be run on Linux-based single-board computers like a Raspberry Pi or Beagle Bone and use MQTT.

## Lamp control via MQTT

`autolight` can automatically control lamps using MQTT according to time and the sunlight illumination as measured outside. A configuration file is used to define the MQTT topics, timings, and thresholds; an example file can be found [here](apps/autolight/example.cfg). The application can, for example, be used to automate [Tasmota](https://tasmota.github.io/docs/) devices.

Typically lamps are automatically switched on at a given time in the morning, then they are switched off when it gets light, then switched on again when it gets dark, and switched off again at a given time. However, using computed sunset and sunrise times does not work well, because depending on the weather the sunlight can be very different: With clear sky there may easily be a factor of 10 higher irradiance than 
with overcast sky, thus humans switch on the light much later if it is clear sky compared to cloudy conditions. Therefore, a measurement of the outside sunlight intensity leads to a more natural user experience of the lighting automation.

The intended logic is achieved as follows:

* If the measured ambient sunlight falls below a given threshold, and the current time is within the "on" period, the lamp is switched on.
* If the measured ambient sunlight raises above a given threshold, or the time goes outside the "on" period, the lamp is switched off.
* Hysteresis is used to bypass measurement noise.

Additionally, movement detectors can switch on lamps for a given time, if the ambient sunlight measurement shows that it is dark.

Optionally there is the possibility to randomly switch lights to simulate presence while being on holiday.
For each light, you configure the average presence time and its variance, and the number of times you are there.
The presence time is drawn from a normal distribution with the given mean and variance.
The time between subsequent presence periods is drawn from a uniform distribution. This simulates your presence in a given room.
Then the light is switched on if the ambient light is below the configured threshold during the random presence time.
It can happen that a presence time period is competely during the light day, in which case the light is not switched on,
while the light could be switched on during the day if it gets very dark outside, e.g. during a thunderstorm,
just as if you would do if you would be physically present in the given room.

## Control GPIOs via MQTT

`gpio_ctrl` switches GPIOs based on MQTT messages. It allows you to use the GPIOs of your single-board computer to control lamps or other devices. A configuration file is used to define the GPIOs and MQTT topics; an example file can be found [here](apps/gpio_ctrl/example.cfg). The topic can be configured to be compatible with [Tasmota](https://tasmota.github.io/docs/MQTT/#command-flow); the payload is assumed to be `ON` or `OFF`. The user which runs the app must have read-write access to the respective `/dev/gpiochip?` device, which typically requires the user to be a member of the group `gpio`; running as root is not recomended.

## How to build

The program is written in C++. It requires a C++17 compatible compiler and uses CMake. The following 3rd-party libraries are necessary:

* [libmosquitto](https://mosquitto.org/api/) - `apt install libmosquitto-dev`
* [libconfig++](https://github.com/hyperrealm/libconfig) - `apt install libconfig++-dev`
* [libgpiod](https://git.kernel.org/pub/scm/libs/libgpiod/libgpiod.git/about/) - `apt install libgpiod-dev`

Only for the tests:

* [Google Test](https://github.com/google/googletest.git) - `apt install googletest`
