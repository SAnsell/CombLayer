/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   modelSupport/LineUnit.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#include <complex> 
#include <vector>
#include <list>
#include <set>
#include <map> 
#include <string>
#include <algorithm>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Surface.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "ObjSurfMap.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "particle.h"
#include "eTrack.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LineUnit.h"


namespace ModelSupport
{

std::ostream&
operator<<(std::ostream& OX,const LineUnit& A)
  /*!
    Write out to a stream
    \param OX :: Output stream
    \param A :: LineUnit to write
    \return Stream
  */
{
  A.write(OX);
  return OX;
}

LineUnit::LineUnit(MonteCarlo::Object* OPtr,
		   const Geometry::Surface* surfacePtr,
		   const double Dist) :
  cellNumber((OPtr) ? OPtr->getName() : 0),
  objPtr(OPtr),
  sPtr(surfacePtr),
  surfNumber((surfacePtr) ? surfacePtr->getName() : 0),
  segmentLength(Dist)
{}

LineUnit::LineUnit(MonteCarlo::Object* OPtr,
		   const int SN,
		   const Geometry::Surface* surfacePtr,
		   const double Dist) :
  cellNumber((OPtr) ? OPtr->getName() : 0),
  objPtr(OPtr),
  sPtr(surfacePtr),
  surfNumber(SN),
  segmentLength(Dist)
{}

LineUnit::LineUnit(MonteCarlo::Object* OPtr,
		   const int SN,
		   const Geometry::Surface* surfacePtr,
		   Geometry::Vec3D endPt,
		   const double Dist) :
  cellNumber((OPtr) ? OPtr->getName() : 0),
  exitPoint(std::move(endPt)),
  objPtr(OPtr),
  sPtr(surfacePtr),
  surfNumber(SN),
  segmentLength(Dist)
{}

LineUnit::LineUnit(MonteCarlo::Object* OPtr,
		   const Geometry::Surface* surfacePtr,
		   Geometry::Vec3D endPt,
		   const double Dist) :
  cellNumber((OPtr) ? OPtr->getName() : 0),
  exitPoint(std::move(endPt)),
  objPtr(OPtr),
  sPtr(surfacePtr),
  surfNumber((surfacePtr) ? surfacePtr->getName() : 0),
  segmentLength(Dist)
{}
  
  
  
void
LineUnit::write(std::ostream& OX) const
  /*!
    Write out the track
    \param OX :: Output stream
  */
{
  OX<<"Obj["<<cellNumber<<"] ="<<exitPoint
    <<" Surf "<<surfNumber
    <<" Dist == "<<segmentLength;
  return;
}

  
} // Namespace ModelSupport
