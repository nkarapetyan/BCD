#define SHORT_PRINT_OUT
#define LONG_PRINT_OUT
#include "ReebGraph.h"

/*****************************************************************************
 * @Modified date: May 31, 2015
 *
 * Reebgraph class. 
 *
**/

// Declares the singleton null vertex & edge pointers
const Vertex ReebGraph::Vnull = Vertex();
const Edge ReebGraph::Enull = Edge();


/*************************************************************************
 * Constructor
 *
**/
ReebGraph::ReebGraph() : g(), Vpointers(), Epointers(), Vid_free(0), Eid_free(0) {};

/*************************************************************************
 * Copy Constructor
 *
**/
ReebGraph::ReebGraph(const ReebGraph& _g) : g(), Vpointers(), Epointers(), Vid_free(0), Eid_free(0)
{
    g = _g.g;
    Vpointers.resize(_g.Vpointers.size());
    copy(_g.Vpointers.begin(),_g.Vpointers.end(),Vpointers.begin());
    Epointers.resize(_g.Epointers.size());
    copy(_g.Epointers.begin(),_g.Epointers.end(),Epointers.begin());
    Vid_free = _g.Vid_free;
    Eid_free = _g.Eid_free;
};


ReebGraph& ReebGraph::operator=( const ReebGraph& _g ) 
{
    g = _g.g;
    Vpointers.resize(_g.Vpointers.size());
    copy(_g.Vpointers.begin(),_g.Vpointers.end(),Vpointers.begin());
    Epointers.resize(_g.Epointers.size());
    copy(_g.Epointers.begin(),_g.Epointers.end(),Epointers.begin());
    Vid_free = _g.Vid_free;
    Eid_free = _g.Eid_free;
    return *this;
};


/*************************************************************************
 * Destructor
 *
 * Cleans up member variables
 *
**/
ReebGraph::~ReebGraph() 
{
    clear();
};


void ReebGraph::clear() 
{
    Vpointers.clear();
    Epointers.clear();
    g.clear();
    Vid_free = 0;
    Eid_free = 0;
};

std::list<Edge> ReebGraph::getEdgeList()
{
    std::list<Edge> temp(Epointers.begin(), Epointers.end());
    return temp;
}


// Adds a new vertex into the current graph : g(), Vpointers(), Epointers(), Vid_free(0), Eid_free(0)
Vertex ReebGraph::addVertex(double x, double y1, double y2, int color) 
{
    Vertex v = add_vertex(ReebVertex(x, y1, y2, color, Vid_free++), g);
    Vpointers.push_back(v);
    return v;
};


Vertex ReebGraph::addVertex(double x, double y, int color) 
{
    return addVertex(x, y, y, color);
};

// Adds a new edge into the current graph
Edge ReebGraph::addEdge(Vertex V1, Vertex V2, int color) 
{
    Edge e = add_edge(V1, V2, ReebEdge(color, Eid_free++), g).first;
    Epointers.push_back(e);
    return e;
};

Edge ReebGraph::addEdge(unsigned int Vid1, unsigned int Vid2, int color) 
{
    return addEdge(Vpointers[Vid1], Vpointers[Vid2], color);
};


// Makes a clone of a particular edge (deep copy)
Edge ReebGraph::cloneEdge(Edge old_e) 
{
    // Clone properties of the existing edge
    ReebEdge old_eprop = g[old_e];
    ReebEdge new_eprop(old_eprop.color, Eid_free++);
    new_eprop.cost = old_eprop.cost;
    new_eprop.topBoundary = old_eprop.topBoundary;
    new_eprop.bottomBoundary = old_eprop.bottomBoundary;

    // Access the vertices attached to the existing edge
    Vertex V1 = source(old_e, g);
    Vertex V2 = target(old_e, g);

    // Create new edge with the cloned properties and the same vertices
    Edge new_e = add_edge(V1, V2, new_eprop, g).first;
    Epointers.push_back(new_e);
    return new_e;
};

Edge ReebGraph::cloneEdge(unsigned int Eid) 
{
    return cloneEdge(Epointers[Eid]);
};

// Makes a clone of a particular edge (deep copy)
void ReebGraph::updateCellArea() 
{
    for(unsigned int i = 0; i < Epointers.size(); i++)
    {
        g[Epointers[i]].updateArea();
    }
};

//FIXME <N>: Must be modified to assign shortest geometrical distance between critical points
/* Computes the travel - Euclidean Distance, for each edge in the graph
* */
void ReebGraph::updateTravelDistances()
{
    for (unsigned int i = 0; i < Epointers.size(); i++)
    {
        Edge e = Epointers[i];
        ReebVertex V1 = getVProp( source(e, g));
        ReebVertex V2 = getVProp( target(e, g));
        Point2D p1(V1.x, (V1.y1 + V1.y2)/2);
        Point2D p2(V2.x, (V2.y1 + V2.y2)/2);
        g[e].setTravelCost(p1.distance(p2));
        /*FIXME <N> debug
        std::cout << "distance for edge -> " << g[e].Eid << " " << g[e].travelCost << std::endl;
        */
    }
}

// Returns the common vertex shared by 2 edges; if not applicable
// then return ReebGraph::nullVertex()
Vertex ReebGraph::findCommonVertex(Edge e1, Edge e2)
{
    if (e1 != ReebGraph::nullEdge() && e2 != ReebGraph::nullEdge())
    {
        Vertex v1 = source(e1, g), v2 = target(e1, g);
        Vertex v3 = source(e2, g), v4 = target(e2, g);

        if (v1 == v3 || v2 == v3)
        {
            return v3;
        }

        else if (v1 == v4 || v2 == v4)
        {
            return v4;
        }
    }
    return ReebGraph::nullVertex();
};


// Returns the vertex / edge object for a specific vertex / edge pointer
// WARNING: check if agument is valid BEFORE calling the following functions
ReebVertex& ReebGraph::getVProp(Vertex v) 
{
    return g[v];
};

ReebVertex& ReebGraph::getVProp(unsigned int Vid) 
{
    return g[Vpointers[Vid]];
};

ReebEdge& ReebGraph::getEProp(Edge e) 
{
    return g[e];
};

ReebEdge& ReebGraph::getEProp(unsigned int Eid) 
{
    return g[Epointers[Eid]];
};


// Returns the 2 vertices attached to a specific edge
std::pair<Vertex, Vertex> ReebGraph::getEndNodes(Edge e)
{
    if (e != nullEdge())
    {
        return std::make_pair(source(e, g), target(e, g));
    } 
    else
    {
        return std::make_pair(nullVertex(), nullVertex());
    }
};


std::pair<Vertex, Vertex> ReebGraph::getEndNodes(unsigned int Eid)
{
    return getEndNodes(Epointers[Eid]);
};


// Modifies the end node(s) attached to a specific edge
// NOTE: if newV2 == ReebGraph::Vnull, then newV2 will be assigned to
//       target(e, graph), i.e. one of the end nodes of the previously
//       specified edge
Edge ReebGraph::modifyEndNodes(Edge e, Vertex newV1, Vertex newV2) 
{
    ReebEdge eData = getEProp(e);

    if (newV2 == ReebGraph::Vnull) 
    {
        newV2 = target(e, g);
    }

    remove_edge(e, g);
    Edge newE = add_edge(newV1, newV2, eData, g).first;
    Epointers[eData.Eid] = newE;
    return newE;
};


Edge ReebGraph::modifyEndNodes(unsigned int Eid, unsigned int newV1id,
        unsigned int newV2id = 0) 
{
    return modifyEndNodes(Epointers[Eid], Vpointers[newV1id],
            ((newV2id == 0) ? ReebGraph::Vnull : Vpointers[newV2id]));
};


// Returns the first vertex in the graph
// NOTE: If no vertices exist in graph, then this function returns NULL
Vertex ReebGraph::getFirstVertex()
{
    if (num_vertices(g) > 0)
    {
        return Vpointers[0];
    }
    else
    {
        return NULL;
    }
};


// Returns start and past-the-end iterators for vertices / edges
std::pair<Vertex_Iter, Vertex_Iter> ReebGraph::getVertices() 
{
    return vertices(g);
};



std::pair<Edge_Iter, Edge_Iter> ReebGraph::getEdges() 
{
    return edges(g);
};


/*************************************************************************
 * Function 'getEdges(Vertex v)'
 *
 * Returns start and past-the-end iterators for edges attached to
 * a specific vertex
 *
 * Returns:
 *   std::pair<Out_Edge_Iter, Out_Edge_Iter> - edges attached to vertex
 *
 * Parameters:
 *   Vertex v 
 *
**/
std::pair<Out_Edge_Iter, Out_Edge_Iter> ReebGraph::getEdges(Vertex v)
{
    return out_edges(v, g);
};


/*************************************************************************
 * Function 'getEdges(unsigned int Vid)'
 *
 * Returns start and past-the-end iterators for edges attached to
 * a specific vertex
 *
 * Returns:
 *   std::pair<Out_Edge_Iter, Out_Edge_Iter> - edges attached to vertex
 *
 * Parameters:
 *   unsigned int Vid - id of vertex
 *
**/
std::pair<Out_Edge_Iter, Out_Edge_Iter> ReebGraph::getEdges(unsigned int Vid)
{
    return getEdges(Vpointers[Vid]);
};


/*************************************************************************
 * Function 'getVertex(unsigned int Vid)'
 *
 * Looks up and returns a specified vertex based on vertex id
 *
 * Returns:
 *   Vertex - specified vertex
 *
 * Parameters:
 *   unsigned int Vid - id of vertex to be found
 *
**/
Vertex ReebGraph::getVertex(unsigned int Vid)
{
    if (Vid >= Vpointers.size())
    {
        return nullVertex();
    }
    else
    {
        return Vpointers[Vid];
    }
};


/*************************************************************************
 * Function 'getEdge(unsigned int Eid)'
 *
 * Looks up and returns a specified edge based on edge id
 *
 * Returns:
 *   Edge - specified edge
 *
 * Parameters:
 *   unsigned int Eid - id of edge to be found
 *
**/
Edge ReebGraph::getEdge(unsigned int Eid)
{
    if (Eid >= Epointers.size()) 
    {
        return nullEdge();
    }
    else
    {
        return Epointers[Eid];
    }
};


/*************************************************************************
 * Function 'numVertices()'
 *
 * Returns the number of vertices in a graph
 *
 * Returns:
 *   unsigned int - vertices in the graph
 *
 * Parameters:
 *   None
 *
**/
unsigned int ReebGraph::numVertices() 
{
    return num_vertices(g);
};


/*************************************************************************
 * Function 'numEdges()'
 *
 * Returns the number of edges in a graph
 *
 * Returns:
 *   unsigned int - edges in the graph
 *
 * Parameters:
 *   None
 *
**/
unsigned int ReebGraph::numEdges() 
{
    return num_edges(g);
};


/*************************************************************************
 * Function 'degree(Vertex v)'
 *
 * Checks the degree of a vertex
 *
 * Returns:
 *   unsigned int - degree of the vertex
 *
 * Parameters:
 *   Vertex - vertex to be checked
 *
**/
unsigned int ReebGraph::degree(Vertex v) 
{
    return out_degree(v, g);
};


/*************************************************************************
 * Function 'degree(int Vid)'
 *
 * Checks the degree of a vertex
 *
 * Returns:
 *   unsigned int - degree of the vertex
 *
 * Parameters:
 *   Vid - id of the vertex to be checked
 *
**/
unsigned int ReebGraph::degree(int Vid) 
{
    return degree(Vpointers[Vid]);
};


/*************************************************************************
 * Function 'empty()'
 *
 * Checks to see if the graph is empty
 *
 * Returns:
 *   bool - graph is empty
 *
 * Parameters:
 *   None
 *
**/
bool ReebGraph::empty() 
{
    return ((num_vertices(g) == 0) || (num_edges(g) == 0));
};


/*************************************************************************
 * Function 'resetAllVertexColor()'
 *
 * Resets the color of all vertex in the graph
 *
 * Returns:
 *   None
 *
 * Parameters:
 *   None
 *
**/
void ReebGraph::resetAllVertexColor() 
{
    int newColor = 0;
    Vertex_Iter vi, vi_end;
    for (tie(vi, vi_end) = vertices(g); vi != vi_end; vi++) 
    {
        g[*vi].color = newColor;
    }
};


/*************************************************************************
 * Function 'resetAllEdgeColor() '
 *
 * Resets the color of all edges in the graph
 *
 * Returns:
 *   None
 *
 * Parameters:
 *   None
 *
**/
void ReebGraph::resetAllEdgeColor() 
{
    int newColor = 0;
    Edge_Iter ei, ei_end;
    for (tie(ei, ei_end) = edges(g); ei != ei_end; ei++) 
    {
        g[*ei].color = newColor;
    }
};


/*************************************************************************
 * Function 'resetAllColor() '
 *
 * Resets the color of all the vertices and edges in the graph
 *
 * Returns:
 *   None
 *
 * Parameters:
 *   None
 *
**/
void ReebGraph::resetAllColor() 
{
    resetAllVertexColor();
    resetAllEdgeColor();
};


/*************************************************************************
 * Function 'nullVertex()'
 *
 * Returns singleton objects for a null vertex
 *
 * Returns:
 *   Edge - null boost edge
 *
 * Parameters:
 *   None
 *
**/
Vertex ReebGraph::nullVertex() 
{
    return ReebGraph::Vnull;
};


/*************************************************************************
 * Function 'nullEdge()'
 *
 * Returns singleton objects for a null edge pointer
 *
 * Returns:
 *   Edge - null boost edge
 *
 * Parameters:
 *   None
 *
**/
Edge ReebGraph::nullEdge() 
{
    return ReebGraph::Enull;
};


/*************************************************************************
 * Function 'equals(ReebGraph Graph)'
 *
 * Method that checks the equality between two ReebGraph objects
 *
 * Returns:
 *   bool
 *
 * Parameters:
 *   None
 *
**/
bool ReebGraph::equals(ReebGraph Graph)
{
    if ((Epointers.size() != Graph.Epointers.size())
            || (Vpointers.size() != Graph.Vpointers.size()))
    {
        return false;
    }

    else if (Vid_free != Graph.Vid_free)
    {
        return false;
    }

    else if (Eid_free != Graph.Eid_free)
    {
        return false;
    }

    else if(numVertices() != Graph.numVertices())
    {
        return false;
    }

    else if(numEdges() != Graph.numEdges())
    {
        return false;
    }

    for(unsigned int i = 0; i < Epointers.size(); ++i)
    {
        if(Epointers.at(i) != Graph.Epointers.at(i))
        {
            return false;
        }
    }

    for(unsigned int i = 0; i < Vpointers.size(); ++i)
    {
        if(Vpointers.at(i) != Graph.Vpointers.at(i))
        {
            return false;
        }
    }

    return true;
};


/*************************************************************************
 * Operator overload: outstream operator
 *
 * Method that prints out the reeb graph data
 *
 * Returns:
 *      ostream& - outstream containing information about the reeb edges
 *          in this graph
 *
 * Parameters:
 *   ostream& out
 *   ReebGraph& g
 *
**/
ostream& operator<< (ostream& out, ReebGraph& g)
{
    // Declares some local variables
    ReebVertex vprop;
    ReebEdge eprop;
    Vertex currVertex;
    Edge currEdge;
    Vertex_Iter vi, vi_end;
    Edge_Iter ei, ei_end;
    Out_Edge_Iter oi, oi_end;
    Vertex v_first, v_second;
    unsigned int currDegree;

    // Accesses general information about the current graph
    out << "The graph has " << g.numVertices() << " vertices and "
        << g.numEdges() << " edges" << endl;

    // Iterates over the vertices &
    // Accesses parameters for each vertex &
    // Accesses the degree of each vertex (NOT A PROPERTY OF THE VERTEX!) &
    // Accesses edges connected to each vertex 
    out << endl << "The vertices of the graph are:" << endl;

    for (tie(vi, vi_end) = g.getVertices(); vi != vi_end; vi++)
    {
        currVertex = *vi;
        vprop = g.getVProp(currVertex);
        currDegree = g.degree(*vi);
        out << vprop;

        if (currDegree > 0)
        {
            out << ", connected to edges: ";
            for (tie(oi, oi_end) = g.getEdges(currVertex); oi != oi_end; oi++)
            {
                eprop = g.getEProp(*oi);
                out << eprop.Eid << " ";
            }
        }

        out << ", is null? " << (currVertex == ReebGraph::nullVertex())
            << endl;
    }

    // Iterates over the edges &
    // Accesses parameters for each edge
    out << endl << "The edges of the graph are:" << endl;
    for (tie(ei, ei_end) = g.getEdges(); ei != ei_end; ei++) 
    {
        currEdge = *ei;
        eprop = g.getEProp(currEdge);
        tie(v_first, v_second) = g.getEndNodes(*ei); // <---- IMPORTANT
        out << eprop << ", attached to vertices: " << g.getVProp(v_first).Vid
            << " & " << g.getVProp(v_second).Vid << ", is null? "
            << (currEdge == ReebGraph::nullEdge()) << endl;
    }

    return out;
}


/*************************************************************************
 * Function 'getGraphEdges()'
 *
 * Method that gets the edges of a graph such that it can be returned
 * as a vector. 
 *
 * Returns:
 *   std::vector<Edge> - edges to the current graph 
 *
 * Parameters:
 *   None
 *
**/
std::vector<Edge> ReebGraph::getGraphEdges()
{
    return Epointers;
};


/*************************************************************************
 * Function 'setGraphEdges()'
 *
 * Method that sets the edges of a graph.
 *
 * Returns:
 *   None
 *
 * Parameters:
 *   edges - edges of the new graph
 *
**/
void ReebGraph::setGraphEdges(std::vector<Edge> edges)
{
    vector<Edge>::iterator iter;
    for(iter = edges.begin(); iter != edges.end(); ++iter)
    {
        Edge edge = (*iter);
        Epointers.push_back(edge);
    }
};


/*************************************************************************
 * Function 'getGraphVertex()'
 *
 * Method that gets the edges of a graph such that it can be returned
 * as a vector.
 *
 * Returns:
 *   std::vector<Vertex> - edges to the current graph 
 *
 * Parameters:
 *   None
 *
**/
std::vector<Vertex> ReebGraph::getGraphVertex()
{
    return Vpointers;
};


/*************************************************************************
 * Function 'setGraphVertices()'
 *
 * Method that sets the vertices of a graph.
 *
 * Returns:
 *   None
 *
 * Parameters:
 *   vertices - Vertices to be set to
 *
**/
void ReebGraph::setGraphVertices(std::vector<Vertex> vertices)
{
    vector<Vertex>::iterator iter;
    for(iter = vertices.begin(); iter != vertices.end(); ++iter)
    {
        Vertex vertex = (*iter);
        Vpointers.push_back(vertex);
    }
};


/*************************************************************************
 * Operator overload: outstream operator
 *
 * Overloading outstream operator
 *   vertex object v
 *   vertex coordinates x and y1/y2.
 *
 * BGL - boost graph library    
 *   have to use the x,y1,y2,vid,and color conventions
 *   for the constructor
 *
 * printing out the reeb vertex parameters
 *
**/
ostream& operator<< (ostream& out, ReebVertex& v) 
{
    out << v.Vid << " [x " << v.x << ", y1 " << v.y1
        << ", y2 " << v.y2 << "]";

    return out;
};


/*************************************************************************
 * Function 'printVertex()'
 *
 * Method that prints the vertices of a graph. Should print the x, y1, y2
 * and vertex id information.
 *
 * Returns:
 *   None
 *
 * Parameters:
 *   None
 *
**/
void ReebGraph::printVertex()
{
    std::cout << "\n";
    vector<Vertex>::iterator iter;
    for(iter = Vpointers.begin(); iter != Vpointers.end(); ++iter)
    {
        ReebVertex vertex = getVProp(*iter);

        //uses overloaded operator method to print out information
        std::cout << vertex;
        std::cout << " | ";  
    }
    std::cout << "\n";
}


/*************************************************************************
 * Operator overload: outstream operator
 *
 * Method that prints out the reeb edge data
 *
 * Returns:
 *   ostream& - contains information about the reeb edges
 *
 * Parameters:
 *   ostream& out
 *   ReebEdge& e
 *
**/
ostream& operator<< (ostream& out, ReebEdge& e) 
{

#ifdef SHORT_PRINT_OUT
    out << e.Eid;
#endif

#ifdef LONG_PRINT_OUT
    out << "[Eid " << e.Eid << ", travelCost " << e.travelCost
        << ", areaCost " << e.area << ", topBoundary "
        << e.topBoundary.size() << ", bottomBoundary "
        << e.bottomBoundary.size() << "]";
#endif

    return out;
};


/*************************************************************************
 * Function 'printEdges()'
 *
 * Method that prints the edges of a graph. Prints the two vertices of
 * the edges and the edge id.  
 *
 * Returns:
 *   None
 *
 * Parameters:
 *   None
 *
**/
void ReebGraph::printEdges()
{
    std::cout << "\n";
    vector<Edge>::iterator iter;
    for(iter = Epointers.begin(); iter != Epointers.end(); ++iter)
    {
        ReebEdge edge = getEProp(*iter);

        Vertex v1,v2;
        tie(v1,v2) = getEndNodes(*iter);

        ReebVertex vertexOne = getVProp(v1);
        ReebVertex vertexTwo = getVProp(v2);

        Point2D first = Point2D(vertexOne.x,
            (vertexOne.y1 + vertexOne.y2) / 2);
        Point2D second = Point2D(vertexTwo.x,
            (vertexTwo.y1 + vertexTwo.y2) / 2);

        //uses overloaded operator method to print out information
        std::cout << edge;
        std::cout << " " << first << " " << second << " | ";  
    }
    std::cout << "\n";
}


/*************************************************************************
 * Function 'runExample()'
 *
 * Method that runs an example of the ReebGraph.
 *
 * Returns:
 *   None 
 *
 * Parameters:
 *   None
 *
**/
/*
void ReebGraph::runExample() 
{
    // Creates a new graph
    ReebGraph g;

    // Adds vertices by specifying vertex properties
    // NOTE: Vid are automatically assigned by addVertex()
    Vertex v0 = g.addVertex(1, 2, 3, 4);        // Vid == 0
    Vertex v1 = g.addVertex(5, 6, 7);           // Vid == 1
    Vertex v2 = g.addVertex(8, 9, 10);          // Vid == 2
    Vertex v3 = g.addVertex(11, 12, 13, 14);    // Vid == 3
    //Vertex v4 = g.addVertex(15, 16, 17);        // Vid == 4
    Vertex vnull = ReebGraph::nullVertex();

    // Adds edges by either specifying Vid or Vertex end-points
    // and specifying edge properties
    // NOTE: Eid are automatically assigned by addEdge()
    //Edge e0 = g.addEdge(0, 1, 15);              // Eid == 0
    //Edge e1 = g.addEdge(v1, v4, 16);            // Eid == 1
    //Edge e2 = g.addEdge(0, 3, 17);              // Eid == 2
    Edge e3 = g.addEdge(v0, v3, 18);            // Eid == 3
    //Edge e4 = g.cloneEdge(e1);                  // Eid == 4
    Edge e5 = g.addEdge(v0, v0, 19);            // Eid == 5
    Edge enull = ReebGraph::nullEdge();

    // Declares some local variables
    ReebVertex vprop;
    ReebEdge eprop;
    Vertex currVertex;
    Edge currEdge;
    Vertex_Iter vi, vi_end;
    Edge_Iter ei, ei_end;
    Out_Edge_Iter oi, oi_end;
    Vertex v_first, v_second;
    unsigned int currDegree;

    // Accesses general information about the current graph
    std::cout << "The graph has " << g.numVertices() << " vertices and "
        << g.numEdges() << " edges" << std::endl;

    // Accesses the first vertex &
    // Edits parameters of a specific vertex and a specific edge
    g.getVProp(g.getFirstVertex()).color = 20;
    g.getEProp(1).cost = 10;

    // Iterates over the vertices &
    // Accesses parameters for each vertex &
    // Accesses the degree of each vertex (NOT A PROPERTY OF THE VERTEX!) &
    // Accesses edges connected to each vertex
    std::cout << std::endl << "The vertices of the graph are:" << std::endl;

    for (tie(vi, vi_end) = g.getVertices(); vi != vi_end; vi++) 
    {
        currVertex = *vi;
        vprop = g.getVProp(currVertex);
        currDegree = g.degree(*vi);
        std::cout << vprop;

        if (currDegree > 0)
        {
            std::cout << ", connected to edges: ";
            for (tie(oi, oi_end) = g.getEdges(currVertex); oi != oi_end; oi++) 
            {
                eprop = g.getEProp(*oi);
                std::cout << eprop.Eid << " ";
            }
        }

        std::cout << ", is null? " << (currVertex == ReebGraph::nullVertex())
            << std::endl;
    }

    // Iterates over the edges &
    // Accesses parameters for each edge
    std::cout << std::endl << "The edges of the graph are:" << std::endl;

    for (tie(ei, ei_end) = g.getEdges(); ei != ei_end; ei++) 
    {
        currEdge = *ei;
        eprop = g.getEProp(currEdge);
        tie(v_first, v_second) = g.getEndNodes(*ei); // <---- IMPORTANT
        std::cout << eprop << ", attached to vertices: "
            << g.getVProp(v_first).Vid << " & "
            << g.getVProp(v_second).Vid << ", is null? "
            << (currEdge == ReebGraph::nullEdge()) << std::endl;
    }

    std::cout << std::endl;

    // Test null vertex and null edge
    std::cout << "Is vnull a null vertex? "
        << (vnull == ReebGraph::nullVertex()) << std::endl;
    std::cout << "Is enull a null edge? "
        << (enull == ReebGraph::nullEdge()) << std::endl;

    // Modifying the end nodes of an edge and re-printing all edges
    e3 = g.modifyEndNodes(e3, v1, v2);
            // Changing from edge(v0, v3) to edge(v1, v2)
    e5 = g.modifyEndNodes(5, 2);
            // Changing from edge(v0, v0) to edge(v2, v0)
    std::cout << std::endl << "The updated edges of the graph are:"
        << std::endl;

    for (tie(ei, ei_end) = g.getEdges(); ei != ei_end; ei++) 
    {
        currEdge = *ei;
        eprop = g.getEProp(currEdge);
        tie(v_first, v_second) = g.getEndNodes(*ei);
        std::cout << eprop << ", attached to vertices: "
            << g.getVProp(v_first).Vid << " & " << g.getVProp(v_second).Vid
            << std::endl;
    }

    std::cout << std::endl;
};
*/
