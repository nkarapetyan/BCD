/************************************************************************

     File:                              Point2D.cc

     Author(s):                     Luiza Solomon, Ioannis Rekleitis and Ai Peng New

     Created:                           11 Jun 2003

     Last Revision:             $Date: 2005/09/28 06:40:51 $

     Description: Defines the necessary point functionality to
                                replace LEDA library calls. 

************************************************************************/

#define POINT2D_C

#include "Point2D.h"

// copy constructor, ensure the data is copied
Point2D::Point2D(const Point2D &rhs)
{
    x = rhs.x;
    y = rhs.y;
}

//Checking equality between points
bool Point2D::operator==(const Point2D &rhs) const
{
    if (x != rhs.x) 
        return false;
    if (y != rhs.y)
        return false;
    return true;
}

//Checking  that two points aren't equal
bool Point2D::operator!=(const Point2D &rhs) const
{
    if (x != rhs.x) 
        return true;
    if (y != rhs.y)
        return true;
    return false;
}

 // ensure the data is copied
Point2D &Point2D::operator=(const Point2D &rhs)
{
    if (this == &rhs)
        return *this;
    x = rhs.x;
    y = rhs.y;
    return *this;
}

//Adds the values of two points together
Point2D Point2D::operator+(const Point2D &rhs) const
{
    return Point2D(x+rhs.x, y+rhs.y);
}

//Subtracts the values of two points
Point2D Point2D::operator-(const Point2D &rhs) const
{
    return Point2D(x-rhs.x, y-rhs.y);
}

//multiplies the values of two points
Point2D Point2D::operator*(double coeff) const
{
    return Point2D(x*coeff, y*coeff);
}

//divides the values of two points
Point2D Point2D::operator/(double denom) const
{
    return Point2D(x / denom, y / denom);
}

//Prints outs the values of a point
ostream &operator<<(ostream &os, const Point2D &p)
{
    os << "(" << p.x << ", " << p.y << ")";
    return os;
}

//Regular print method
void Point2D::Print(ostream &os)
{
    os << "(" << x << ", " << y << ")";
}

//Input stream overload, resetting values
istream &operator>>(istream &is, Point2D &p)
{
    double px, py;

    is >> px >> py;
    p.x = px;
    p.y = py;

    return is;
}

// returns the squared distance between current point and point q
double Point2D::sdist(const Point2D& q) const
{
    double dx = x - q.x;
    double dy = y - q.y;

    return dx*dx + dy*dy;
}

// returns the distance between current point and point q
double Point2D::distance(const Point2D& q) const
{
    double dx = x - q.x;
    double dy = y - q.y;

    return sqrt(dx*dx + dy*dy);
}

// returns the manhatten distance between current point and point q
double Point2D::mdistance(const Point2D& q) const
{
    double dx = fabs(x - q.x);
    double dy = fabs(y - q.y);

    return dx+dy;
}


// returns the smaller angle formed by vectors (p1 - this) and (this - p2)
// using law of cosines
double Point2D::cosineAngle(const Point2D& p1, const Point2D& p2) const
{
    double distP1P0 = distance(p1);
    double distP2P0 = distance(p2);
    double distP1P2 = p1.distance(p2);
    if (distP1P0 == 0 || distP2P0 == 0) {
        return 0;
    }
    return acos((distP1P0*distP1P0 + distP2P0*distP2P0 - distP1P2*distP1P2)
            / (2*distP1P0*distP2P0)) / M_PI * 180;
};


//  computes the orientation of points a, b, and c as the sign of
//  the determinant  $ \left\vert\begin{array}{ccc} a_x & a_y & 1
//  b_x & b_y & 1\\ c_x & c_y & 1 \end{array} \right\vert$
//  i.e., it returns +1 if point c lies left of the directed line
//  through a and b, 0 if a,b, and c are collinear, and -1 otherwise.

int Point2D::orientation(const Point2D b, const Point2D c) const
{
    double z = (b.x-x)*(c.y-y) - (c.x-x)*(b.y-y);
    if (z < 0)
        return (-1);
    else if (z > 0)
        return(1);
    else return(0);
}

// returns the point rotated about origin by angle fi.
Point2D Point2D::rotate(const Point2D &origin, double fi) const
{
    double cx = origin.xcoord();
    double cy = origin.ycoord();
    double sinfi = sin(fi);
    double cosfi = cos(fi);
    double dx = xcoord() - cx;
    double dy = ycoord() - cy;
    return Point2D(cx + dx*cosfi - dy*sinfi, cy + dx*sinfi + dy*cosfi);
}

// returns rotate(point(0, 0), fi).
Point2D Point2D::rotate(double fi) const
{
    double sinfi = sin(fi);
    double cosfi = cos(fi);
    double x = xcoord();
    double y = ycoord();
    return Point2D(x*cosfi - y*sinfi, x*sinfi + y*cosfi);
}

// returns the point rotated about p by an angle of 90 degrees. 
Point2D Point2D::rotate90(const Point2D& p) const
{
    double px = p.xcoord();
    double py = p.ycoord();
    double dx = x - px;
    double dy = y - py;
    return Point2D(px-dy, py+dx);
}

// returns rotate90(point(0, 0)).
Point2D Point2D::rotate90() const
{
    return Point2D(-ycoord(), xcoord()); 
}

/////////////////////////////////////////////////////////////////////////////
// EOF
 /***********************************************************************
     $Revision: 1.7 $

     $Log: Point2D.cc,v $
     Revision 1.7  2005/09/28 06:40:51  multi
     Code has been updated to improve Coverage phase and implement Auction Mechanism

     Revision 1.6  2004/08/11 15:42:41  multi
     Changes made are for improved functionality with PL

     Revision 1.5  2003/11/26 16:52:46  multi
     Added getObsDirection for 360 laser sensor i.r.

     Revision 1.4  2003/11/26 16:07:29  multi
     Added log mechanism i.r.

     Copyright (c) 2003 by Ioannis Rekleitis, CMU.  
     Please see the copyright notice
     included in this distribution for full details.

 ***********************************************************************/
