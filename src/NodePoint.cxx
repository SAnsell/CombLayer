/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   src/NodePoint.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Transform.h"
#include "NodePoint.h"

NodePoint::NodePoint() :
  NodeID(-1),Coord(Geometry::Vec3D(0,0,0)),Morph(Geometry::Vec3D(0,0,0)),
  heat(0.0),cellID(-1),cellName(-1),matType(-1),surfDist(-1.0)
  /*!
    Default Constructor
  */
{}

NodePoint::NodePoint(const Geometry::Vec3D& A,const int id,const double H) :
  NodeID(id),Coord(A),Morph(A),heat(H),cellID(-1),cellName(-1),matType(-1),
  surfDist(-1.0)
  /*!
    Constructor 
    \param A :: Original Point
    \param id :: Node name
    \param H :: current Heating 
  */
{}

NodePoint::NodePoint(const NodePoint& A) :
  NodeID(A.NodeID),Coord(A.Coord),Morph(A.Morph),heat(A.heat),
  cellID(A.cellID),cellName(A.cellName),matType(A.matType),
  surfDist(A.surfDist)
  /*!
    Copy Constructor 
    \param A :: NodePoint to copy
  */
{}

NodePoint&
NodePoint::operator=(const NodePoint& A) 
  /*!
    Standard assignment operator 
    \param A :: NodePoint to copy
    \returns *this
  */
{
  if (this!=&A)
    {
      NodeID=A.NodeID;
      Coord=A.Coord;
      Morph=A.Morph;
      heat=A.heat;
      cellID=A.cellID;
      cellName=A.cellName;
      matType=A.matType;
      surfDist=A.surfDist;
    }
  return *this;
}

void
NodePoint::setPoint(const Geometry::Vec3D& Pt)
  /*! 
    Sets both the Coordinate and the Morph  point 
     \param Pt :: Input point
  */
{
  Coord=Pt;
  Morph=Pt;
  return;
}

void
NodePoint::setMorph(const Geometry::Vec3D& Pt)
  /*! 
    Sets only the Morph point 
    \param Pt :: Input point
  */
{
  Morph=Pt;
  return;
}

void
NodePoint::applyTransform(const Geometry::Transform& TM)
  /*!
    Applies a simple transform to the coordinate and 
    the morph point
    \param TM :: Transform object
  */
{
  Coord.rotate(TM.rotMat());
  Coord+=TM.shift();
  Morph.rotate(TM.rotMat());
  Morph+=TM.shift();
  return;
}

void
NodePoint::write(std::ostream& OFS) const
  /*! 
    General writing of both the morph and coordpoint
    with the cell and heat data
    \param OFS :: Output stream
  */
{
  OFS<<NodeID<<" ["<<cellID<<"] ["<<cellName<<"] "
     <<matType<<" ("<<Coord<<") ("<<Morph<<") "<<heat<<std::endl;
  return;
}

void
NodePoint::applyPressure(const Geometry::Vec3D& Pt,const double Force)
  /*!
    Applies a general pressure to the Morph point
    emminating from Point Pt. 
    - Distance moved is \f[ Force \frac{1}{r^2} \f] where r is the distance
    separating Morph and Pt.
    \param Pt:: The point from which pressure originates
    \param Force:: the scalar for the pressure
  */
{
  Geometry::Vec3D Dvec=Morph-Pt;  // Vector to apply pressure 
  double moveUnit=1.0/Dvec.abs();
  Dvec*=Force*pow(moveUnit,3.0);    // Note this is 1/r2 + 1/r to
                                    // make unit vector
  Morph+=Dvec;
  return;
}
 
