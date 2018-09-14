/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsTally/T3DShow.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <iomanip>
#include <fstream>
#include <cmath>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <iterator>
#include <array>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "writeSupport.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Mesh3D.h"

#include "phitsTally.h"
#include "T3DShow.h"

namespace phitsSystem
{

T3DShow::T3DShow(const int outID) :
  phitsTally("mesh"+std::to_string(outID),outID),
  meshType(10),particle("208")
  /*!
    Constructor
    \param outID :: Identity number of tally [fortranOut]
  */
{}

T3DShow::T3DShow(const T3DShow& A) : 
  phitsTally(A),
  meshType(A.meshType),particle(A.particle),
  Pts(A.Pts),minCoord(A.minCoord),
  maxCoord(A.maxCoord)
  /*!
    Copy constructor
    \param A :: T3DShow to copy
  */
{}

T3DShow&
T3DShow::operator=(const T3DShow& A)
  /*!
    Assignment operator
    \param A :: T3DShow to copy
    \return *this
  */
{
  if (this!=&A)
    {
      phitsTally::operator=(A);
      meshType=A.meshType;
      particle=A.particle;
      Pts=A.Pts;
      minCoord=A.minCoord;
      maxCoord=A.maxCoord;
    }
  return *this;
}

  
T3DShow*
T3DShow::clone() const
  /*!
    Clone object
    \return new (this)
  */
{
  return new T3DShow(*this);
}

T3DShow::~T3DShow()
  /*!
    Destructor
  */
{}
  
void
T3DShow::setParticle(const std::string& P)
  /*!
    Set the mesh particle
    \param P :: Partile
  */
{
  particle=P;
  return;
}

void
T3DShow::setCoordinates(const Geometry::Vec3D& A,
			const Geometry::Vec3D& B)
  /*!
    Sets the min/max coordinates
    \param A :: First coordinate
    \param B :: Second coordinate
  */
{
  ELog::RegMethod RegA("T3DShow","setCoordinates");
  
  minCoord=A;
  maxCoord=B;
  // Add some checking here
  for(size_t i=0;i<3;i++)
    {
      if (std::abs(minCoord[i]-maxCoord[i])<Geometry::zeroTol)
	throw ColErr::NumericalAbort(StrFunc::makeString(minCoord)+" ::: "+
				     StrFunc::makeString(maxCoord)+
				     " Equal components");
      if (minCoord[i]>maxCoord[i])
	std::swap(minCoord[i],maxCoord[i]);
    }


  return;
}

  
void
T3DShow::write(std::ostream& OX) const
  /*!
    Write out the mesh tally into the tally region
    \param OX :: Output stream
   */
{
  std::ostringstream cx;
  
  return;
}

}  // NAMESPACE phitsSystem

