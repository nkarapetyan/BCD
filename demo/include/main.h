#ifndef MAIN_H
#define MAIN_H

#include <QImage>
#include <QString>
#include <iostream>
#include <string>
#include <stdio.h>
#include <cmath>
#include <string>
#include <sstream>
#include <set>
#include <QApplication>
#include <QLabel>

#include <boost/lexical_cast.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#include <time.h>
#include <Boustrophedon/BCD.h>
#include <Boustrophedon/DrawImage.h>
#include <Leo/Point2D.h>
#include <Leo/ReebGraph.h>
#include <Leo/RegionData.h>


#include <fstream>

using namespace std;
using namespace Qt;
using namespace boost;


class Main
{

public:
    int main();
    virtual ~Main();

private:



};

#endif //MAIN_H

