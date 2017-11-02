/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/Edge.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Vertex.h"
#include "Face.h"
#include "Edge.h"

namespace Geometry
{

std::ostream&
operator<<(std::ostream& OX,const Edge& A)
  /*!
    Standard output stream
    \param OX :: Output stream
    \param A :: Edge to write
    \return Stream State
   */
{
  A.write(OX);
  return OX;
}

//-------------------------------------------
//               EDGE 
//-------------------------------------------

Edge::Edge()  : 
  deleteRequired(0)
  /*!
    Constructor
  */
{
  ELog::RegMethod RegA("Edge","Constructor");
  endPts[0]=0;
  endPts[1]=0;
  adjFace[0]=0;
  adjFace[1]=0;
  newFace=0;
}

Edge::Edge(const Edge& A)  : 
  deleteRequired(A.deleteRequired)
  /*!
    Copy Constructor
    \param A :: Edge object
  */
{
  ELog::RegMethod RegA("Edge","Copy Constructor");
  endPts[0]=A.endPts[0];
  endPts[1]=A.endPts[1];
  adjFace[0]=A.adjFace[0];
  adjFace[1]=A.adjFace[1];
  newFace=A.newFace;
}

Edge&
Edge::operator=(const Edge& A)  
  /*!
    Assignment operator
    \param A :: Edge to copy
    \return *this
  */
{
  ELog::RegMethod RegA("Edge","operator=");
  
  if (this!=&A)
    {
      deleteRequired=A.deleteRequired;
      endPts[0]=A.endPts[0];
      endPts[1]=A.endPts[1];
      adjFace[0]=A.adjFace[0];
      adjFace[1]=A.adjFace[1];
      newFace=A.newFace;
    }
  return *this;
}

Edge::~Edge()
  /*!
    Destructor
  */
{}

void
Edge::setEndPts(Vertex* A,Vertex* B)
  /*!
    Set the end points
    \param A :: First vertex
    \param B :: second vertex
  */
{
  endPts[0]=A;
  endPts[1]=B;
  return;
}

int
Edge::oneVisible() const
  /*!
    Detemine if one or more faces are visible
    \return 1/0 on success/failure
  */
{
  if (!(adjFace[0] && adjFace[1]))
    throw ColErr::EmptyValue<Face*>("Edge::oneVisible");
  if (adjFace[0]->isVisible())
    return 1;
  if (adjFace[1]->isVisible())
    return 2;
  return 0;
}

int
Edge::bothVisible() const
  /*!
    Detemine if both faces are visible		
    \return 1/0 on success/failure
  */
{
  if (!(adjFace[0] && adjFace[1]))
    {
      write(std::cerr);
      throw ColErr::EmptyValue<Face*>("Edge::bothVisible");
    }
  return (adjFace[0]->isVisible() && adjFace[1]->isVisible());
}

Vertex*
Edge::getEndPt(const int Index) 
  /*!
    Access a vertex
    \param Index :: Index value of edge [0-1]
    \return Vertex Ptr
  */
{
  if (Index<0 || Index>1)
    throw ColErr::IndexError<int>(Index,2,"Edge::getEndPt");
  return endPts[Index];
}

const Vertex*
Edge::getEndPt(const int Index) const
  /*!
    Access a vertex
    \param Index :: Index value of edge [0-1]
    \return Vertex Ptr
  */
{
  if (Index<0 || Index>1)
    throw ColErr::IndexError<int>(Index,2,"Edge::getEndPt const");
  return endPts[Index];
}

Face*
Edge::getVisibleFace()
  /*!
    Gets a single visible face
    \throw MisMatch if 0 or 2 visible faces
    \return Face 
   */
{
  const int vA=(adjFace[0]) ? adjFace[0]->isVisible() : 0;
  const int vB=(adjFace[1]) ? adjFace[1]->isVisible() : 0;
  if (vA && !vB) return adjFace[0];
  if (!vA && vB) return adjFace[1];
  throw ColErr::MisMatch<int>(vA+vB,1,"Edge::getVisibleFace");
}

Face*
Edge::getAdjFace(const int Index)
  /*!
    Sets an adjacent face
    \param Index :: Index value of face [0-1]
    \return Adjacent Face
  */
{
  if (Index<0 || Index>1)
    throw ColErr::IndexError<int>(Index,2,"Edge::getAdjFace");
  return adjFace[Index];
}

void
Edge::setOnHull()
  /*!
    Set on hull for both faces across edge
   */
{
  for(int i=0;i<2;i++)
    if (endPts[i])
      endPts[i]->setOnHull(1);
  return;
}

void
Edge::setNewFace(Face* FPtr)
 /*!
   Assign the new face
   \param FPtr
 */
{
  newFace=FPtr;
  return;
}

void
Edge::setAdjFace(const int Index,Face* FPtr)
  /*!
    Sets an adjacent face
    \param Index :: Index value of face [0-1]
    \param FPtr :: Face Pointer
  */
{
  if (Index<0 || Index>1)
    throw ColErr::IndexError<int>(Index,2,"Edge::setAdjFace");
  adjFace[Index]=FPtr;
  return;
}

void
Edge::setAdjacentFaces(Face* FA,Face* FB)
  /*!
    Sets an adjacent face
    \param FA :: Face Pointer
    \param FB :: Face Pointer
  */
{
  adjFace[0]=FA;
  adjFace[1]=FB;
  return;
}

void
Edge::setVisibleFace()
  /*!
    Sets the visible face to the new face 
    and clears the system
  */
{
  if (newFace)
    {
      const int oneV(oneVisible());
      adjFace[oneV-1]=newFace;
      newFace=0;
    }
  return;
}

void
Edge::write(std::ostream& OX) const
  /*!
    Write out an edge
    \param OX :: Output stream
  */
{
  for(int i=0;i<2;i++)
    {
      OX<<"V["<<i<<"]=";
      if (endPts[i])
	OX<<endPts[i]->getID()<<" ";
      else
	OX<<"Null ";
    }
  for(int i=0;i<2;i++)
    {
      OX<<"F["<<i<<"]=";
      if (adjFace[i])
	OX<<std::hex<<reinterpret_cast<long int>(adjFace[i])<<" ";
      else
	OX<<0<<" ";
    }
  OX<<"F[New]="<<std::hex<<reinterpret_cast<long int>(newFace)<<std::endl;
  return;
}

} // NAMESPACE Geometry
  
