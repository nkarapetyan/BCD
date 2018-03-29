## About

This is BCD library that provides following classes:
- BCD
- DrawImage
- OpenImage
______
## Required Packages

- Opencv 2.x 
- Boost 1.58a [sudo apt-get install libboost-all-dev]
- QT 5 [https://wiki.qt.io/Install_Qt_5_on_Ubuntu] or sudo apt-get install qt5-default
 If Qt is not in default package then specify CMAKE_PREFIX_PATH=/path/to/where/qt5/lives
______
## Installation

- to build library do ./run
- to install do sudo ./run install

This will install library in /usr/local/lib/Boustrophedon/Boustrophedon.so
and the header files in /usr/local/include/Boustrophedon

______
## Methods
----------------------
*  BCD(string directory, string fileName, RegionData& data, ReebGraph& graph);

Parametrized constructor that takes in input binray image and name of the directory
----------------------
* void buildBCD(RegionData& data, ReebGraph& graph);

Runs Boustrophedon algorithm and creats ReebGraph 
----------------------
* void viewReebGraph(QString fileName, RegionData data, ReebGraph graph);
  
Method draws the given ReebGraph. The methods used here each draw over
  the image buffer.
----------------------
* void printBCDInfo(ReebGraph graph);

-  Method that prints the general information of a given reeb graph such as its edges and vertices.
----------------------
