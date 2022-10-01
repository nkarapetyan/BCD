#ifndef DrawImage_H_
#define DrawImage_H_

/******************************************************************************
 *
 * Header file for the DrawImage calculation.
 *
**/

#include <iostream>
#include <QImage>
#include <QString>
#include <QPaintDevice>
#include <QPainter>
#include <boost/lexical_cast.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>


#include <bcd_utils/ReebGraph.h>
#include <bcd_utils/RegionData.h>


class DrawImage
{

public:

    DrawImage();
    DrawImage(ReebGraph g, RegionData d);
    ~DrawImage();

    void setImageBuffer(cv::Mat* const source);
    void setImageBuffer(QImage var);
    QImage getImageBuffer();

    void clearImage();
    void clearDevice();

    void drawBCDRegions();
    void drawReebGraph();
    void saveImageBuffer(QString fileName);

private:
    cv::Mat matBuffer;
    QImage imageBuffer;

    QPaintDevice* device;
    RegionData d;
    ReebGraph g;

    const static unsigned int WAYPOINT_NORMAL_RADIUS = 6;
    const static unsigned int WAYPOINT_HIGHLIGHT_RADIUS = 8;

    // Constants
    const static unsigned char BLACK = 0;
    const static unsigned char WHITE = 255;

};
#endif
