/////////////////////////////////////////////////////////////////////////////
//   File:                Point2D.h
//   Author(s):           Luiza Solomon, Ioannis Rekleitis
//
//   Created:             11 Jun 2003
//
//   Last Revision:       August 1st, 2003
//
//   Description:         Supplies the necessary point functionality to
//                        replace LEDA library calls. 
//
//   Copyright (c) 2003 by Ioannis Rekleitis, Centre for Intelligent Machines,
//   McGill University, Montreal, QC.  Please see the copyright notice
//   included in this distribution for full details.
//
///////////////////////////////////////////////////////////////////////

#ifndef POINT2D_H
#define POINT2D_H

#include <iostream>
#include <math.h>
#include <algorithm>
using namespace std;

//////////////////////////////////////////////////////////////////
//Declaring the Point2D class

class Point2D
{

public:

    //Sets up a 2d object with x and y coordinates
    Point2D() { x = 0; y = 0; }
    Point2D(double xcoord, double ycoord) { x = xcoord; y = ycoord; }
    Point2D(std::pair<double, double> xycoord)
        { x = xycoord.first; y = xycoord.second; }
    ~Point2D() {}

    //copy constructor
    Point2D(const Point2D &rhs);

    //operator overloading
    bool operator==(const Point2D &rhs) const;
    bool operator!=(const Point2D &rhs) const;
    Point2D operator+(const Point2D &rhs) const;
    Point2D operator-(const Point2D &rhs) const;
    Point2D operator*(double coeff) const;
    Point2D operator/(double denom) const;
    Point2D& operator=(const Point2D &rhs);
    friend ostream &operator<<(ostream &os, const Point2D &rline);
    void Print(ostream &os);
    friend istream &operator>>(istream &is, Point2D &rline);

    //returning the coordinates of the object
    double xcoord() const { return x; }
    double ycoord() const { return y; }

    // returns the squared distance between current point and point q
    double sdist(const Point2D& q) const;

    // returns the distance between current point and point q
    double distance(const Point2D& q) const;

    // returns Manhattan distance
    double mdistance(const Point2D& q) const;

    // returns the smaller angle formed by vectors (p1 - this) and (this - p2)
    double cosineAngle(const Point2D& p1, const Point2D& p2) const;

    // computes the orientation of points a, b, and c 
    int orientation(const Point2D q, const Point2D r) const;

    // returns the point rotated about origin by angle fi
    Point2D rotate(const Point2D &origin, double fi) const;

    // returns rotate(point(0, 0), fi)
    Point2D rotate(double fi) const;

    // returns the point rotated about p by an angle of 90 degrees
    Point2D rotate90(const Point2D &p) const;

    // returns rotate90(point(0, 0))
    Point2D rotate90() const;

protected:

    double x, y;   // x, y coordinates of a point
};

#endif

////////////////////////////////////////////////////////////////////////
// EOF
