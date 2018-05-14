/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/MeshGrid.cxx
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
#include <iterator>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "MeshGrid.h"

namespace Geometry
{

MeshGrid::MeshGrid(const Geometry::Vec3D& CPt,
		   const Geometry::Vec3D& AxisX,
		   const Geometry::Vec3D& AxisY) :
  Origin(CPt-(AxisX+AxisY)/2.0),XAxis(AxisX),
  YAxis(AxisY),level(1),type(0),
  AIndex(0),BIndex(0)
  /*!
    Constructor for Mesh
    The grid goes from Centre Point +/- Axis values
    \param CPt :: Centre point
    \param AxisX :: X axis
    \param AxisY :: Y axis
  */
{
  partUnits.push_back(0.0);
  partUnits.push_back(1.0);
}

MeshGrid::MeshGrid(const MeshGrid& A) : 
  Origin(A.Origin),XAxis(A.XAxis),YAxis(A.YAxis),level(A.level),
  type(A.type),fracUnits(A.fracUnits),partUnits(A.partUnits),
  AIndex(A.AIndex),BIndex(A.BIndex)
  /*!
    Copy constructor
    \param A :: MeshGrid to copy
  */
{}

MeshGrid&
MeshGrid::operator=(const MeshGrid& A)
  /*!
    Assignment operator
    \param A :: MeshGrid to copy
    \return *this
  */
{
  if (this!=&A)
    {
      level=A.level;
      type=A.type;
      fracUnits=A.fracUnits;
      partUnits=A.partUnits;
      AIndex=A.AIndex;
      BIndex=A.BIndex;
    }
  return *this;
}

void
MeshGrid::updateAB(const size_t ASize,const size_t BSize)
  /*!
    Simple update of a cyclic pair
    \param ASize :: Size of AIndex
    \param BSize :: Size of BIndex 
  */
{
  ELog::RegMethod RegA("MeshGrid","updateAB");

  BIndex++;
  if (BIndex==BSize)
    {
      BIndex=0;
      AIndex++;
      if (AIndex==ASize)
	{
	  AIndex=0;
	  type++;
	}
    }

  return;
} 


Vec3D
MeshGrid::getNext() 
  /*!
    Calculate the new centre
    \return bisection centre
  */
{
  ELog::RegMethod RegA("MeshGrid","getNext");


  Geometry::Vec3D Out=Origin;
  if (level<2 && type>0) type=3;
    
  switch (type)
    {
    case 0:            /// New:New
      Out+=(XAxis*partUnits[AIndex])+(YAxis*partUnits[BIndex]);
      updateAB(partUnits.size(),partUnits.size());
      return Out;

    case 1:              /// Old:New
      Out+=XAxis*fracUnits[AIndex]+YAxis*partUnits[BIndex];
      updateAB(fracUnits.size(),partUnits.size());
      return Out;

    case 2:              /// New:Old
      Out+=XAxis*partUnits[AIndex]+YAxis*fracUnits[BIndex];
      updateAB(partUnits.size(),fracUnits.size());
      return Out;

    case 3:
      // reset / recall
      createNextLevel();
      Out=getNext();
      return Out;
    }
  throw ColErr::ExitAbort("Type misassigned :: "+
			  ELog::RegMethod::getFull());
}

void
MeshGrid::createNextLevel()
  /*!
    Generates the next level
  */
{
  ELog::RegMethod RegA("MeshGrid","createNextLevel");
  // Copy old to new
  fracUnits.insert(fracUnits.end(),partUnits.begin(),partUnits.end());

  const int twoPower(1<<level);

  const double unit(1.0/ twoPower);
  partUnits.clear();
  for(int i=0;i<twoPower/2;i++)
    partUnits.push_back(unit+2.0*i*unit);
  // reset to internal
  type=0;
  level++;
  return;
}

} // NAMESPACE Geometry
