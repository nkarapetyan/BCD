#ifndef REEBGRAPH_H_
#define REEBGRAPH_H_

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <list>
#include <vector>
#include <ostream>
#include <bcd_utils/Point2D.h>
#include <boost/graph/graph_utility.hpp>

using namespace boost;

/**
 *  Defines the object type for vertices in the Reeb graph
**/
struct ReebVertex
{
    double x;
    double y1;
    double y2;
    int color;
    unsigned int Vid;

    // Custom constructors
    // NOTE: use this instead of defining params. manually
    ReebVertex(double _x, double _y1, double _y2, int _color,
            unsigned int _Vid) :
        x(_x), y1(_y1), y2(_y2), color(_color), Vid(_Vid) {};

    // Default constructor
    // NOTE: this is provided to make BGL happy...
    //       try to use custom constructor if possible
    ReebVertex() :
        x(-1), y1(-1), y2(-1), color(-1), Vid(0) {};

};


/**
 *  Defines the object type for edges in the Reeb graph
**/
struct ReebEdge
{
    std::vector<Point2D> topBoundary;
    std::vector<Point2D> bottomBoundary;
    int color;
    double cost;
    unsigned int Eid;
    int area;
    double travelCost;
            //FIXME <N> length of edge - euclidean distance between the source
            // and target vertices

    // Custom constructor
    // NOTE: use this instead of defining params. manually
    ReebEdge(int _color, unsigned int _Eid) :
        topBoundary(), bottomBoundary(), color(_color), cost(-1), Eid(_Eid),
        travelCost(-1) {};

    // Default constructor
    // NOTE: this is provided to make BGL happy...
    //       try to use custom constructor if possible
    ReebEdge() :
        topBoundary(), bottomBoundary(), color(-1), cost(-1), Eid(0),
        travelCost(-1) {};

    // Compute Area
    void updateArea()
    {
        area = 0;
        for (unsigned int i = 0; i < topBoundary.size(); i++)
        {
            area += abs(topBoundary[i].ycoord() - bottomBoundary[i].ycoord()); //FIXME: <N>
        }
    }

    //FIXME: <N> set travel cost
    void setTravelCost(const double tCost) {
        travelCost = tCost;
    }

    //FIXME: <N> set area/coverage cost
	void setAreaCost(const double aCost) {
		area = aCost;
	}
};


// Defines type aliases for graph-related structures
// JUSTIFICATIONS:
// Using std::list for edge container: O(log(E/V)) add_edge() insertion, O(E/V) edge() access
// Using std::list for vertex container: sacrifice space to get better time performance
typedef adjacency_list<listS, listS, undirectedS, ReebVertex, ReebEdge> Graph;//FIXME: listS is changed to vecS
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;

typedef boost::graph_traits<Graph>::vertex_iterator Vertex_Iter;
typedef boost::graph_traits<Graph>::edge_iterator Edge_Iter;
typedef boost::graph_traits<Graph>::adjacency_iterator Adjacency_Iter;
typedef boost::graph_traits<Graph>::out_edge_iterator Out_Edge_Iter;


// This class provides a thin wrapper for Boost.Graph class
class ReebGraph
{

public:

    // Initializes member variables
    ReebGraph();
    ReebGraph(const ReebGraph& _g);
    ReebGraph& operator=( const ReebGraph& _g );

    // Cleans up member variables
    ~ReebGraph();
    void clear();

    bool equals(ReebGraph Graph);
    std::list<Edge> getEdgeList();

    // Adds a new vertex into the current graph : g(), Vpointers(), Epointers(), Vid_free(0), Eid_free(0)
    Vertex addVertex(double x, double y1, double y2, int color);
    Vertex addVertex(double x, double y, int color);

    // Adds a new edge into the current graph
    Edge addEdge(Vertex V1, Vertex V2, int color);
    Edge addEdge(unsigned int Vid1, unsigned int Vid2, int color);

    // Makes a clone of a particular edge (deep copy)
    Edge cloneEdge(Edge old_e);
    Edge cloneEdge(unsigned int Eid);

    // Makes a clone of a particular edge (deep copy)
    void updateCellArea();

    //FIXME <N>: Must be modified to assign shortest geometrical distance between critical points
    /* Computes the travel - Euclidean Distance, for each edge in the graph
    * */
    void updateTravelDistances();

    // Returns the common vertex shared by 2 edges; if not applicable
    // then return ReebGraph::nullVertex()
    Vertex findCommonVertex(Edge e1, Edge e2);

    // Returns the vertex / edge object for a specific vertex / edge pointer
    // WARNING: check if agument is valid BEFORE calling the following functions
    ReebVertex& getVProp(Vertex v);
    ReebVertex& getVProp(unsigned int Vid);
    ReebEdge& getEProp(Edge e);
    ReebEdge& getEProp(unsigned int Eid);

    // Returns the 2 vertices attached to a specific edge
    std::pair<Vertex, Vertex> getEndNodes(Edge e);
    std::pair<Vertex, Vertex> getEndNodes(unsigned int Eid);

    // Modifies the end node(s) attached to a specific edge
    // NOTE: if newV2 == ReebGraph::Vnull, then newV2 will be assigned to
    //       target(e, graph), i.e. one of the end nodes of the previously
    //       specified edge
    Edge modifyEndNodes(Edge e, Vertex newV1, Vertex newV2 = ReebGraph::Vnull);
    Edge modifyEndNodes(unsigned int Eid, unsigned int newV1id, unsigned int newV2id);

    // Returns the first vertex in the graph
    // NOTE: If no vertices exist in graph, then this function returns NULL
    Vertex getFirstVertex();

    // Returns start and past-the-end iterators for vertices / edges
    std::pair<Vertex_Iter, Vertex_Iter> getVertices();
    std::pair<Edge_Iter, Edge_Iter> getEdges();


    // Returns start and past-the-end iterators for edges attached to
    // a specific vertex
    std::pair<Out_Edge_Iter, Out_Edge_Iter> getEdges(Vertex v);
    std::pair<Out_Edge_Iter, Out_Edge_Iter> getEdges(unsigned int Vid);

    Vertex getVertex(unsigned int Vid);
    Edge getEdge(unsigned int Eid);

    // Provides auxiliary informations regarding graphs, vertices, and edges
    unsigned int numVertices();
    unsigned int numEdges();
    unsigned int degree(Vertex v);
    unsigned int degree(int Vid);
    bool empty();

    // Mass-reset member variables
    void resetAllVertexColor();
    void resetAllEdgeColor();
    void resetAllColor();

    // Returns singleton objects for null vertex & null edge pointers
    static Vertex nullVertex();
    static Edge nullEdge();

    // Provides example code for how to use this class (see ReebGraph.cpp)
    void setGraphEdges(std::vector<Edge> edges);
    std::vector<Edge> getGraphEdges();

    void setGraphVertices(std::vector<Vertex> vertices);
    std::vector<Vertex> getGraphVertex();

    void printEdges();
    void printVertex();
    static void runExample();

private:

    // Boost graph object
    Graph g;

    // Internal structures that facilitate accessing vertex and edge pointers
    // when addressed using their IDs
    std::vector<Vertex> Vpointers;
    std::vector<Edge> Epointers;
    //std::pair<Edge_Iter,Edge_Iter> p;

    // Next free vertex & edge ID values
    unsigned int Vid_free;
    unsigned int Eid_free;

    // Singleton objects for null vertex and null edge
    const static Vertex Vnull;
    const static Edge Enull;
};


// Defines ostream debug-print functions
ostream& operator<< (ostream& out, ReebVertex& v);
ostream& operator<< (ostream& out, ReebEdge& e);
ostream& operator<< (ostream& out, ReebGraph& g);

#endif /* REEBGRAPH_H_ */
