#include <bcd/BCD.h>

/******************************************************************************
*
* This file is for the BCD calculation and work behind that. When a BCD object
* is created is should ask for an image file and then calculate the BCD/draw it.
*
**/


/*************************************************************************
 * Constructor
 *
**/
BCD::BCD() {};

/*************************************************************************
 * Constructor that takes in input image directory name and filename
 *
**/
BCD::BCD(string directory, string fileName)
{
    data = imageLoader.open(directory, fileName);
    buildBCD();
//TODO: implement independent constructor based on input data
}


/*************************************************************************
 * Alternate Constructor - needed for when you pass in BCD info
 *
**/
BCD::BCD(string directory, string fileName, RegionData& data, ReebGraph& graph)
{
    data = imageLoader.open(directory, fileName);
    buildBCD(data, graph);
};


/*************************************************************************
 * Destructor
 *
**/
BCD::~BCD() {};


void BCD::buildBCD()
{
buildBCD(data, graph);
}
/*************************************************************************
 * Author: Patrick Virie
 *
 * Function 'buildBCD(RegionData, ReebGraph) throw (const std::string&)'
 *
 * Method taken from CoverAlgo.cpp and re-used here. Calculates the BCD
 * regions and necessary for ReebGraph* graph to be used within
 * the drawBCDRegions() and drawReebGraph() methods. Otherwise graph
 * will be empty and nothing will be drawn overtop the map.
 *
 * Returns:
 *   None
 *
 * Parameters:
 *   None
 *
**/
void BCD::buildBCD(RegionData& data, ReebGraph& graph)
    throw (const std::string&)
{
    //Test
    cv::Mat& map = data.map;
    graph.clear();

#define GETY(a) (a)->first.first.ycoord()
#define ISFREE(a) (a)->first.second
#define EDGE(a) (a)->second

    /* (point,isFree until that point?,edge of the region before that point) */
    std::vector< pair<pair<Point2D,bool>,Edge> > region_bounds_0;
    std::vector< pair<pair<Point2D,bool>,Edge> > region_bounds_1;
    std::vector< pair<pair<Point2D,bool>,Edge> >* region_bounds
        = &region_bounds_0;
    std::vector< pair<pair<Point2D,bool>,Edge> >* last_region_bounds
        = &region_bounds_1;

    std::vector< pair<pair<Point2D,bool>,Edge> >::iterator iTop;
    std::vector< pair<pair<Point2D,bool>,Edge> >::iterator iBot;
    std::vector< pair<pair<Point2D,bool>,Edge> >::iterator iiTop;
    std::vector< pair<pair<Point2D,bool>,Edge> >::iterator iiBot;

    last_region_bounds->clear();
    last_region_bounds->push_back(make_pair(make_pair(Point2D(-1, -1), false),
                ReebGraph::nullEdge()));
    last_region_bounds->push_back(make_pair(make_pair(Point2D(-1, map.rows-1),
                    false), ReebGraph::nullEdge()));

    for (int x=0; x <= map.cols; ++x)
    {
        /* scan line and cluster */
        bool isFree = false;
        region_bounds->clear();
        region_bounds->push_back(make_pair(make_pair(Point2D(x, -1), false),
                    ReebGraph::nullEdge()));
        if (x != map.cols)
        {
            for (int y=0; y < map.rows; ++y)
            {
                unsigned char curP = map.at<unsigned char>(y, x);
                if(curP == WHITE && !isFree)
                {
                    region_bounds->push_back(make_pair(make_pair(Point2D(x, y),
                                    isFree), ReebGraph::nullEdge()));
                    isFree = true;
                }
                else if(curP == BLACK && isFree)
                {
                    region_bounds->push_back(make_pair(make_pair(Point2D(x, y),
                                    isFree), ReebGraph::nullEdge()));
                    isFree = false;
                }
            }
        }

        region_bounds->push_back(make_pair(make_pair(Point2D(x, map.rows-1),
                        isFree), ReebGraph::nullEdge()));

        /* find critical points  */
        /* first scan the last bounds to find possible critical points.*/
        iTop = last_region_bounds->begin();

        for (iBot = last_region_bounds->begin()+1;
                iBot != last_region_bounds->end(); ++iBot)
        {
            iiTop = region_bounds->begin();
            for(iiBot = region_bounds->begin()+1;
                    iiBot != region_bounds->end(); ++iiBot)
            {
                if(GETY(iTop) <= GETY(iiTop) && GETY(iBot) >= GETY(iiBot)
                        && !ISFREE(iBot) && ISFREE(iiBot))
                {
                    /*B_CONCAVE*/
                    //cerr << "B_CONCAVE " << (GETY(iiTop) + GETY(iiBot)) /2 << " at " << x << endl;
                    Vertex _v = graph.addVertex(x, GETY(iiTop), GETY(iiBot),0);
                    Edge _e0 = graph.addEdge(_v, _v, 0);
                    EDGE(iiBot) = _e0;
                }
                else if(GETY(iTop) >= GETY(iiTop) && GETY(iBot) <= GETY(iiBot)
                        && ISFREE(iBot) && !ISFREE(iiBot))
                {
                    /*F_CONCAVE*/
                    //cerr << "F_CONCAVE " << (GETY(iTop) + GETY(iBot)) /2 << " at " << x << endl;
                    Vertex _v = graph.addVertex(x, GETY(iTop), GETY(iBot),0);
                    Edge _e0 = graph.modifyEndNodes(EDGE(iBot), _v);
                    EDGE(iBot) = _e0;
                }
                else if(GETY(iTop) < GETY(iiTop) && GETY(iBot) > GETY(iiBot)
                        && ISFREE(iBot) && !ISFREE(iiBot))
                {
                    /*F_CONVEX*/
                    //cerr << "F_CONVEX " << (GETY(iiTop) + GETY(iiBot)) /2 << " at " << x << endl;
                    Vertex _v = graph.addVertex(x, GETY(iiTop), GETY(iiBot), 0);
                    Edge _e = graph.modifyEndNodes(EDGE(iBot), _v);
                    Edge _e0 = graph.addEdge(_v, _v, 0);
                    EDGE(iiTop) = _e0;
                    Edge _e1 = graph.addEdge(_v, _v, 0);
                    // leave it at the back bound
                    EDGE(iBot) = _e1;
                }
                else if(GETY(iTop) > GETY(iiTop) && GETY(iBot) < GETY(iiBot)
                        && !ISFREE(iBot) && ISFREE(iiBot))
                {
                    /*B_CONVEX*/
                    //cerr << "B_CONVEX " << (GETY(iTop) + GETY(iBot)) /2 << " at " << x << endl;
                    Vertex _v = graph.addVertex(x, GETY(iTop), GETY(iBot), 0);
                    Edge _e1 = graph.modifyEndNodes(EDGE(iBot+1), _v);
                    EDGE(iBot+1) = _e1;
                    Edge _e0 = graph.modifyEndNodes(EDGE(iiBot), _v);
                    Edge _e = graph.addEdge(_v, _v, 0);
                    EDGE(iiBot) = _e; // << leave it at the front bound
                }

                else if ((min(GETY(iBot), GETY(iiBot))
                            - max(GETY(iTop), GETY(iiTop)) > 0)
                        && ISFREE(iBot) && ISFREE(iiBot))
                {
                    if (EDGE(iiBot) == ReebGraph::nullEdge())
                    {
                        EDGE(iiBot) = EDGE(iBot);
                    }
                }

                iiTop = iiBot;
            }

            iTop = iBot;
        }

        /* Fill Boundaries */
        int i = 0;
        iiTop = region_bounds->begin();
        for (iiBot = region_bounds->begin()+1; iiBot != region_bounds->end();
                ++iiBot, ++i)
        {
            if (ISFREE(iiBot))
            {
                if (EDGE(iiBot) != ReebGraph::nullEdge())
                {
                    graph.getEProp(EDGE(iiBot)).topBoundary.push_back(
                            iiTop->first.first);
                    graph.getEProp(EDGE(iiBot)).bottomBoundary.push_back(
                            iiBot->first.first);
                }
                else
                {
                    cerr << "Something must be wrong..."
                        << region_bounds->size() << " " << i << " " << x
                        << endl;
                }
            }

            iiTop = iiBot;
        }

        /* Swap current and last */
        swap(region_bounds, last_region_bounds);

    }

#undef GETY
#undef ISFREE
#undef EDGE

    /* Assign weight to each edge */
    Edge_Iter ei, ei_end;
    Out_Edge_Iter oi, oi_end;
    Vertex_Iter vi, vi_end;
    for (tie(ei, ei_end) = graph.getEdges(); ei != ei_end; ++ei)
    {
        ReebEdge ep = graph.getEProp(*ei);
        double sum_h = 0;

        for(unsigned int i = 0; i < ep.topBoundary.size(); ++i)
        {
            sum_h += abs(ep.topBoundary[i].ycoord()
                    - ep.bottomBoundary[i].ycoord());
        }

        /* The cost is the inverse of the cell's height because we want to minimize the number of turns.
         * Also, if the cell is too wide, we might not prefer to split it.
         * */
        graph.getEProp(*ei).cost = ep.topBoundary.size()
            * ep.topBoundary.size() / sum_h;
        //graph.getEProp(*ei).cost = 1;
    }

    graph.updateCellArea();

    //cerr<<graph.numEdges() << " E : V " <<graph.numVertices()<<endl;
    for (tie(ei, ei_end) = graph.getEdges(); ei != ei_end; ei++)
    {
        std::pair<Vertex,Vertex> _v = graph.getEndNodes(*ei);
        //cerr<< graph.getVProp(_v.first) << " " << graph.getVProp(_v.second) << endl;
    }

    for (tie(vi, vi_end) = graph.getVertices(); vi != vi_end; vi++)
    {
        tie(oi, oi_end) = graph.getEdges(*vi);

        int i = 0;
        for (; oi != oi_end; ++oi)
        {
            ++i;
        }

        //cerr<< graph.getVProp(*vi).Vid <<" "<< i << endl;
    }

    //cerr << "<< Built BCD" << endl;

    // Sanity check: make sure there are some vertices and edges
    if (graph.numVertices() <= 0)
    {
        graph.clear();
        throw string("Graph has no vertices!");
    }
    else if (graph.numVertices() <= 1)
    {
        graph.clear();
        throw string("Graph only has 1 vertex!");
    }

    if (graph.numEdges() <= 0)
    {
        graph.clear();
        throw string("Graph has no edges!");
    }

    // Sanity check: look for edges with no topBoundary or bottomBoundary entries
    bool foundEmptyEdge = false;
    for (tie(ei, ei_end) = graph.getEdges(); ei != ei_end; ei++)
    {
        if (graph.getEProp(*ei).topBoundary.empty()
                || graph.getEProp(*ei).bottomBoundary.empty())
        {
            foundEmptyEdge = true;
            break;
        }
    }

    if (foundEmptyEdge)
    {
        graph.clear();
        throw string("Graph has an edge with no boundary points!");
    }
};



/*************************************************************************
 * viewReebGraph(QString fileName)
 * an interface that creates ReebGraph and saves it as an image in fileName
**/
void BCD::viewReebGraph(QString fileName){

  viewReebGraph(fileName, data, graph);
}

/*************************************************************************
 * Function 'viewReebGraph(QString fileName)'
 *
 * Method creates the ReebGraph. The methods used here each draw over
 * the image buffer.
 *
 * Returns:
 *   None
 *
 * Parameters:
 *   None
 *
**/
void BCD::viewReebGraph(QString fileName, RegionData data, ReebGraph graph)
{
    if (graph.empty() == true)
    {
        std::cout << "graph is empty";
        return;
    }

    DrawImage placeHolder(graph, data);
    placeHolder.drawBCDRegions();
    placeHolder.drawReebGraph();
    placeHolder.saveImageBuffer(fileName);
};

/*************************************************************************
 * Function 'printBCDInfo()'
 *
 * Inteface for printing the BCD ReebGraph info
**/
void BCD::printBCDInfo()
{
    printBCDInfo(graph);
}

/*************************************************************************
 * Function 'printBCDInfo()'
 *
 * Method that gets the general information of a graph such as its edges
 * and vertices.
 *
 * Returns:
 *   None
 *
 * Parameters:
 *   None
 *
**/
void BCD::printBCDInfo(ReebGraph graph)
{
    graph.printEdges();
    std::cout << "\n" << "\n";
    graph.printVertex();
    std::cout << "\n" << "\n";
};
