/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geometry/Vertex.cxx
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

namespace Geometry
{

std::ostream&
operator<<(std::ostream& OX,const Vertex& A)
  /*!
    Standard output stream
    \param OX :: Output stream
    \param A :: Vertex to write
    \return Stream State
  */
{
  A.write(OX);
  return OX;
}

Vertex::Vertex() :
  done(0),onHull(1),vIndex(0),coneEdge(0)
  /*!
    Constructor
  */
{
  ELog::RegMethod RegA("Vertex","Constructor()");
}

Vertex::Vertex(const int N,const Geometry::Vec3D& PVec) :
  done(0),onHull(1),vIndex(N),V(PVec),coneEdge(0)
  /*!
    Constructor 
    \param N :: vertex index (identifier)
    \param PVec :: Position of vertex
  */
{
  ELog::RegMethod RegA("Vertex","Constructor(N,PVec)");
}

Vertex::Vertex(const Vertex& A) :
  done(A.done),onHull(A.onHull),
  vIndex(A.vIndex),V(A.V),coneEdge(A.coneEdge)
  /*!
    Copy Constructor 
    \param A :: Vertex object 
  */
{
  ELog::RegMethod RegA("Vertex","Copy Constructor");
}

Vertex&
Vertex::operator=(const Vertex& A) 
  /*! 
    Assignment constructor
    \param A :: Vertex object 
    \return *this
  */
{
  ELog::RegMethod RegA("Vertex","operator=");
  if (this!=&A)
    {
      done=A.done;
      onHull=A.onHull;
      vIndex=A.vIndex;
      V=A.V;
      coneEdge=A.coneEdge;
    }
  return *this;
}

void
Vertex::write(std::ostream& OX) const
  /*!
    Debug write out1
    \param OX :: Output stream
   */
{
  std::ios_base::fmtflags originalFlags=OX.flags();
  OX<<V<<" (IHD) "<<vIndex<<" "<<onHull<<" "<<done<<" ";
  OX<<std::hex<<reinterpret_cast<long int>(coneEdge);
  OX.flags(originalFlags);
  return;
}


} // NAMESPACE Geometry
  
