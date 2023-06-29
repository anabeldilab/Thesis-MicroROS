# Bachelor's Degree Thesis: Control of Electromechanical Devices using BCI - ROS Part (MicroROS)

## About
This repository contains the ROS (Robot Operating System) component of my Bachelor's Degree Thesis project on controlling electromechanical devices using Brain-Computer Interface (BCI). The entire project is divided into two main parts: a Unity application and ROS nodes. This specific part handles the MicroROS component of the project, which is crucial for communication between the Unity application and the electromechanical devices.

In this repository, what we essentially have is an ESP-IDF (Espressif IoT Development Framework) project with a MicroROS component. MicroROS is a minimized version of ROS2 capable of being run on microcontrollers, and in this case, it helps establish and manage communication with the Unity application.

## Prerequisites
To run this project, you will need:

ROS2 environment installed

It's important to note that the micro_ros_espidf_component must be in the same version as your ROS2 installation. For example, if you have ROS2 Humble installed, you must use the micro_ros_espidf_component in the Humble branch.

## Usage
Usage
This application follows the standard use case of an ESP-IDF project. Once you've met the prerequisites, you can follow these steps to run the application:

* Navigate to the project directory.
* Run this command to set target the ESP32 microcontroller idf.py set-target esp32
* Configure the project by running idf.py menuconfig.
* Build the project by running idf.py build.
* Connect your device, then flash the project onto the device with idf.py -p PORT flash, replacing PORT with your device's port.
Monitor the application's output with idf.py -p PORT monitor.

Please replace PORT with the appropriate port for your system.

## PWM Theory in servomotors
En general, el rango de duty cycle posible es de 0 a 32767 (para una resolución de 15 bits). Sin embargo, para controlar un servomotor, es probable que no uses todo el rango.

En la mayoría de los servomotores, un pulso de 1ms (milisegundo) corresponde a un ángulo de 0 grados, y un pulso de 2ms corresponde a un ángulo de 180 grados. Estos pulsos se aplican sobre un periodo total de 20ms (correspondiente a una frecuencia de 50Hz, común en servomotores).

Si estás usando la resolución máxima de 15 bits, el valor de duty para un pulso de 1ms puede ser calculado como sigue:

```c
duty_1ms = (1ms/20ms) * (2^15 - 1) ≈ 1638
```
Y para un pulso de 2ms:

```c
duty_2ms = (2ms/20ms) * (2^15 - 1) ≈ 3277
```
Por lo tanto, para la mayoría de los servos, el rango útil de valores de duty cycle estaría entre aproximadamente 1638 y 3277 (para una resolución de 15 bits y una frecuencia de 50Hz). Estos valores pueden variar dependiendo de las especificaciones del servo.

## ESP32 as access point
In order to connect the camera to the Unity application, the ESP32 must be configured as an access point. We need it because we want to have a direct connection between the camera and the Unity application, without the need for a router. To do this, we need to configure the ESP32 as an access point and set up a static IP address for the camera.

* NVS (Non-Volatile Storage) is a key-value storage system used by ESP-IDF to store persistent data. It is used to store Wi-Fi credentials and other configuration data. NVS is implemented using flash memory, and its API is included in the esp_system.h header file.



## Testing
To test the application, you can use the Unity project available in this repository. The Unity project is a simple application that mocks ROS2 String Messages to test the behavior of the pan-tilt servomotors.

To test the application, you can follow these steps:

* Go to the Unity project directory. cd test
* Build the project.
* Flash the project onto your ESP32 device.
* Use Monitor mode to see tests results.


## License
This project is licensed under the GNU General Public License Version 3, dated 29 June 2007. For more information, please see the LICENSE file in this repository or visit https://www.gnu.org/licenses/gpl-3.0.html.


## References
* PWM https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/ledc.html#_CPPv419ledc_timer_config_t

* Componente MicroROS https://github.com/micro-ROS/micro_ros_espidf_component/tree/humble
