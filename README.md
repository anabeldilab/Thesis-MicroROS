# Bachelor's Degree Thesis: Control of Electromechanical Devices using BCI - ROS Part (MicroROS)

## About
This repository contains the ROS (Robot Operating System) component of my Bachelor's Degree Thesis project on controlling electromechanical devices using Brain-Computer Interface (BCI). The entire project is divided into two main parts: a Unity application and ROS nodes. This specific part handles the MicroROS component of the project, which is crucial for communication between the Unity application and the electromechanical devices.

In this repository, what we essentially have is an ESP-IDF (Espressif IoT Development Framework) project with a MicroROS component. MicroROS is a minimized version of ROS2 capable of being run on microcontrollers, and in this case, it helps establish and manage communication with the Unity application.

## Prerequisites
To run this project, you will need:

ROS2 environment installed

## Usage
Usage
This application follows the standard use case of an ESP-IDF project. Once you've met the prerequisites, you can follow these steps to run the application:

* Navigate to the project directory.
* Configure the project by running idf.py menuconfig.
* Build the project by running idf.py build.
* Connect your device, then flash the project onto the device with idf.py -p PORT flash, replacing PORT with your device's port.
Monitor the application's output with idf.py -p PORT monitor.

Please replace PORT with the appropriate port for your system.


## License
This project is licensed under the GNU General Public License Version 3, dated 29 June 2007. For more information, please see the LICENSE file in this repository or visit https://www.gnu.org/licenses/gpl-3.0.html.
