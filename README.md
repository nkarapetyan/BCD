## About

This is implementation of Boustrophedon Decompotion algorithm. 
It includes separated utility library and a demo with sample usage.
______

## Structure:
 
* BCD/  - Boustrophedon Decomposition Algorithm Implementation
* LeoLibs/ - utility class implementing ReebGraph
* demo/ - contains and example use of BCD

______
## Required Packages

- Opencv 2.x 
- Boost 1.58a [sudo apt-get install libboost-all-dev]
- QT 5 [https://wiki.qt.io/Install_Qt_5_on_Ubuntu] or sudo apt-get install qt5-default
 If Qt is not in default package then specify CMAKE_PREFIX_PATH=/path/to/where/qt5/lives
______
## Installation

* to build library do ./run_all
* to install do sudo ./run install

this will compile and install LibLeo and BCD libraries

______
## Usage

* To run demo see README in demo folder
* For full method description check README in BCD folder.

______
## Licence

Released under the MIT License.

* When using this code please use the following citation:

Anqi Xu, Chatavut Viriyasuthee, Ioannis Rekleitis. Efficient complete coverage of a known arbitrary environment with applications to aerial operations. Autonomous Robots, 36(4):365-381, 2014. [26 citations]

* When refering to Multirobot systems refer to following work:

Nare Karapetyan, Kelly Benson, Chris McKinney, Perouz Taslakian, Ioannis Rekleitis. Efficient Multi-Robot Coverage of a Known Environment. In IEEE/RSJ International Conference on Intelligent Robots and Systems (IROS), 2017.
