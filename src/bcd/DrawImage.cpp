#include <bcd/DrawImage.h>

/******************************************************************************
 * @maintainer: Kelly Benson
 * @email: bensonke@email.sc.edu
 * @date: May 16, 2015
 *
 * This file is for the DrawImage calculation and work behind that. When a
 * DrawImage object is created it should ask for an image file and then
 * calculate the DrawImage/draw it.
 *
**/


/*************************************************************************
 * Constructor
 *
**/
DrawImage::DrawImage() {};


/*************************************************************************
 * Alt Constructor
 *
**/
DrawImage::DrawImage(ReebGraph graph, RegionData data)
{
    g = graph;
    d = data;

    //If the device is empty set it to the image buffer. QPaintDevice as a
    //  class takes in things like widgets and QImages to draw overtop.
    //  So the paint device has to be set to a QImage. Especially since
    //  QPainter draws overtop paint devices.
    setImageBuffer(&(d.image));
    device = &imageBuffer;
};

/*************************************************************************
 * Destructor
 *
**/
DrawImage::~DrawImage() {};

/*************************************************************************
 * Function 'setImageBuffer(QImage)'
 *
 * Method that can initialize the QImage imageBuffer. Allows QImages
 * to be passed into this class.
 *
 * Returns:
 *   None
 *
 * Parameters:
 *   var - QImage being taken in and set to the imageBuffer
 *
**/
void DrawImage::setImageBuffer(QImage var)
{
    imageBuffer = var;
}


/*************************************************************************
 * Function 'setImageBuffer()'
 *
 * Method that sets the QImage variable imageBuffer from the image file.
 *
 * Returns:
 *   None
 *
 * Parameters:
 *   cv::Mat* source - Image being converted to QImage format
 *
**/
void DrawImage::setImageBuffer(cv::Mat* const source)
{
    matBuffer = cv::Mat();
    QImage::Format imgFormat = QImage::Format_Invalid;
    if (source == NULL)
    {
        return;
    }

    if ((source->depth() != CV_8U) && (source->depth() != CV_8S))
    {
        std::cout<< "ERROR > Image depth is not 8U/8S!";
        return;
    }
    else
    {
        switch(source->channels())
        {
            case 1:
                cv::cvtColor(*source, matBuffer, cv::COLOR_GRAY2RGB);
                imgFormat = QImage::Format_RGB888;
                break;

            case 3:
                cv::cvtColor(*source, matBuffer, cv::COLOR_BGR2RGB);
                imgFormat = QImage::Format_RGB888;
                break;

            case 4:
                cv::cvtColor(*source, matBuffer, cv::COLOR_BGR2RGB);
                imgFormat = QImage::Format_ARGB32;
                break;

            default:
                imgFormat = QImage::Format_Invalid;
                std::cout << "Format Invalid";
                return;
        }
    }

    imageBuffer = QImage(matBuffer.data, matBuffer.cols, matBuffer.rows,\
        matBuffer.step, imgFormat);
};


/*************************************************************************
 * Function 'getImageBuffer()'
 *
 * Method that gets the QImage variable imageBuffer and returns it so it
 * can be used elsewhere.
 *
 * Returns:
 *   None
 *
 * Parameters:
 *   source - Image source being converted to QImage format
 *
**/
QImage DrawImage::getImageBuffer()
{
    return imageBuffer;
};


/*************************************************************************
 * Function 'clearImage()'
 *
 * Method that clears the imageBuffer
 *
 * Returns:
 *   None
 *
 * Parameters:
 *   None
 *
**/
void DrawImage::clearImage()
{
    imageBuffer = QImage();
};


/*************************************************************************
 * Function 'clearDevice()'
 *
 * Method that clears the QPaintDevice
 *
 * Returns:
 *   None
 *
 * Parameters:
 *   None
 *
**/
void DrawImage::clearDevice()
{
    device = NULL;
};


/*************************************************************************
 * Function 'drawBCDRegions()'
 *
 * Method that draws the BCD Regions. It uses the ReebGraph Class
 * to create a graph.
 *
 * Returns:
 *   None
 *
 * Parameters:
 *   None
 *
**/
void DrawImage::drawBCDRegions()
{
    ReebEdge eprop;
    Edge_Iter ei, ei_end;
    QPainter painter;
    QPolygon polygon;
    int i;
    int midX, midY;

    //Begin painting the buffer
    painter.begin(device);

    //Blue = BCD regions
    QPen pen;
    pen.setColor(Qt::blue);
    pen.setWidth(1);
    painter.setPen(pen);

    painter.setFont(QFont("Arial", 16));

    //Getting the edges and looping through them.
    for (tie(ei, ei_end) = g.getEdges(); ei != ei_end; ei++)
    {
        //Getting a reeb edge
        eprop = g.getEProp(*ei);
        polygon.clear();


        //Creating the top section of a BCD region
        for (i = 0; i < (int) eprop.topBoundary.size(); i++)
        {
            polygon << QPoint(eprop.topBoundary[i].xcoord(),
                    eprop.topBoundary[i].ycoord());
        }


        //Creating the bottom section of a BCD region
        for (i = eprop.bottomBoundary.size()-1; i >= 0; i--)
        {
            polygon << QPoint(eprop.bottomBoundary[i].xcoord(),
                    eprop.bottomBoundary[i].ycoord());
        }


        //getting the midpoint point
        midX = eprop.topBoundary.size()/2;
        midY = (eprop.topBoundary[midX].ycoord()
                + eprop.bottomBoundary[midX].ycoord()) / 2;
        midX = eprop.topBoundary[midX].xcoord();


        //painter.setBrush(colorSamples[eprop.Eid % numColorSamples]);
        painter.setBrush(Qt::white);
        painter.drawPolygon(polygon);
    }

    painter.end();
};


/*************************************************************************
 * Function 'drawReebGraph()'
 *
 * Method that draws a reeb graph on the image buffer.
 *
 * Returns:
 *   None
 *
 * Parameters:
 *   None
 *
**/
void DrawImage::drawReebGraph()
{
    //Found in ReebGraph header under boost::graph_traits
    Edge_Iter ei, ei_end;
    Vertex_Iter vi, vi_end;
    Vertex v_first, v_second;
    Edge prevEdge = ReebGraph::nullEdge();

    //Defined in header file of ReebGraph
    ReebEdge* eprop;
    ReebVertex* vprop;

    //Under QT libraries
    QPainter painter;
    QVector<QLineF> lines;
    int index;

    //start painting on the image buffer
    painter.begin(device);

    //it would be nice to know what these values were for?
    double x0, y0, x1, y1, x2, y2, x3, y3, px, py, cx, cy;
    double a3x, a2x, a1x, a0x, a3y, a2y, a1y, a0y;

    //tie() - boost method that makes it easier to work with
    //  functions that return a std::pair<> such as vertices.
    //  Allows you to mess with things individually.

    //Getting the vertices and then looping through them.
    for (tie(vi,vi_end) = g.getVertices(); vi != vi_end; ++vi)
    {
        //getting the reeb vertex
        vprop = &(g.getVProp(*vi));

        //Is this drawing the circle to represent the vertex or is it drawing
        //  the lines inbetween the points?
        painter.setPen(Qt::green);
        painter.drawEllipse(QPoint(vprop->x, (vprop->y1 + vprop->y2) / 2),
                WAYPOINT_NORMAL_RADIUS, WAYPOINT_NORMAL_RADIUS);
    }

    //Getting all the edges. From starting point to end point
    //  and then looping through.
    for (tie(ei, ei_end) = g.getEdges(); ei != ei_end; ++ei)
    {
        //getting the reeb edge
        eprop = &(g.getEProp(*ei));

        //getting the starting and ending nodes to an edge
        tie(v_first, v_second) = g.getEndNodes(*ei);

        //if the first vertices x value is larger than the
        //  second vertices x value.
        if(g.getVProp(v_first).x < g.getVProp(v_second).x)
        {
            //dealing with the first vertex
            //what is happening here with y1 and y2?
            y0 = (g.getVProp(v_first).y1 + g.getVProp(v_first).y2) / 2;
            x0 = g.getVProp(v_first).x;

            //dealing with second vertex
            y3 = (g.getVProp(v_second).y1 + g.getVProp(v_second).y2) / 2;
            x3 = g.getVProp(v_second).x;
        }

        // if the second vetices x value is larger than the first vertex's
        // x value
        else
        {
            //Now the values are reversed.
            //Dealing with the first vertex
            y3 = (g.getVProp(v_first).y1 + g.getVProp(v_first).y2) / 2;
            x3 = g.getVProp(v_first).x;

            //dealing with the second vertex
            y0 = (g.getVProp(v_second).y1 + g.getVProp(v_second).y2) / 2;
            x0 = g.getVProp(v_second).x;
        }

        //Shouldn't need to mess with this much since its all math.
        //Index used for x1 and y1
        index = max((int)(min(eprop->topBoundary.size()/3,
                        eprop->topBoundary.size()-1)), 0);
        y1 = (eprop->topBoundary[index].ycoord()
                + eprop->bottomBoundary[index].ycoord()) / 2;
        x1 = eprop->topBoundary[index].xcoord();

        //Index being re-used and reset for x2 and y2
        index = max((int)(min(eprop->topBoundary.size()*2/3,
                        eprop->topBoundary.size()-1)), 0);
        y2 = (eprop->topBoundary[index].ycoord()
                + eprop->bottomBoundary[index].ycoord()) / 2;
        x2 = eprop->topBoundary[index].xcoord();

        //No idea where these values are coming from and
        //  I assume they are either arbitrary or explainable
        //  somehow.
        a3x = 5.33333*x3 - 10.66667*x2 + 10.66667*x1 - 5.33333*x0;
        a2x = -5.33333*x3 + 13.33333*x2 - 18.66667*x1 + 10.66667*x0;
        a1x =  x3 - 2.66667*x2 + 8.00000*x1 - 6.33333*x0;
        a0x = x0;

        a3y = 5.33333*y3 - 10.66667*y2 + 10.66667*y1 - 5.33333*y0;
        a2y = -5.33333*y3 + 13.33333*y2 - 18.66667*y1 + 10.66667*y0;
        a1y = y3 - 2.66667*y2 + 8.00000*y1 - 6.33333*y0;
        a0y = y0;

        px = x0;
        py = y0;
        lines.clear();

        for(double i = 0; i <= 1.05; i += 0.05)
        {
            cx = ((a3x*i + a2x)*i + a1x)*i + a0x;
            cy = ((a3y*i + a2y)*i + a1y)*i + a0y;

            // creating a vector of lines that need to be drawn?
            // pretty sure all this weird math and the creation of these
            // lines is drawing the lines between the verticies.
            lines.push_back(QLineF(cx, cy, px, py));
            px = cx;
            py = cy;
        }

        painter.setPen(Qt::green);
        painter.drawLines(lines);
    }

    painter.end();
};



/*************************************************************************
 * Function 'saveImageBuffer(QString)'
 *
 * Method saves the current QImage to a file in the output folder.
 *
 * Returns:
 *   None
 *
 * Parameters:
 *   fileName - name of the file to be saved as
 *
**/
void DrawImage::saveImageBuffer(QString fileName)
{
    if (imageBuffer.isNull())
    {
        return;
    }

    imageBuffer.save(fileName);
};
