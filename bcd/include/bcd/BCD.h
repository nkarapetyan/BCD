#ifndef BCD_H_
#define BCD_H_

/******************************************************************************
 *
 * Header file for the BCD calculation.
 *
**/

#include <iostream>
#include <sstream>
#include <QString>
#include <bcd_utils/ReebGraph.h>
#include <bcd_utils/RegionData.h>
#include <bcd/OpenImage.h>
#include <bcd/DrawImage.h>

class BCD {

public:

    BCD();
    BCD(string directory, string fileName);
    BCD(string directory, string fileName, RegionData& data, ReebGraph& graph);
    ~BCD();

    void buildBCD();
    void viewReebGraph(QString fileName);
    void printBCDInfo();

private:
    void buildBCD(RegionData& data, ReebGraph& graph)
        throw (const std::string&);
    void viewReebGraph(QString fileName, RegionData data, ReebGraph graph);
    void printBCDInfo(ReebGraph graph);

private:

    OpenImage imageLoader;
    RegionData data; //FIXME
    ReebGraph graph;

    // Constants
    const static unsigned char BLACK = 0;
    const static unsigned char WHITE = 255;
};
#endif
