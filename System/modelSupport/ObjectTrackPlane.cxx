/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   modelSupport/ObjectTrackPlane.cxx
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
#include <set> 
#include <map> 
#include <string>
#include <algorithm>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LineUnit.h"
#include "LineTrack.h"
#include "ObjectTrackAct.h"
#include "ObjectTrackPlane.h"

namespace ModelSupport
{

ObjectTrackPlane::ObjectTrackPlane(const Geometry::Plane& PtA) :
  ObjectTrackAct(),TargetPlane(PtA)
  /*! 
    Constructor 
  */
{}

ObjectTrackPlane::ObjectTrackPlane(const ObjectTrackPlane& A) :
  ObjectTrackAct(A),TargetPlane(A.TargetPlane)
   /*! 
    Copy Constructor 
    \param A :: ObjectTrackPlane to copy
  */
{}


ObjectTrackPlane&
ObjectTrackPlane::operator=(const ObjectTrackPlane& A) 
   /*! 
     Assignment operator
    \param A :: ObjectTrackPlane to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ObjectTrackAct::operator=(A);
      TargetPlane=A.TargetPlane;
    }
  return *this;
}

void
ObjectTrackPlane::addUnit(const Simulation& System,
			const long int objN,
			const Geometry::Vec3D& IPt)
  /*!
    Create a target track between the IPt and the target point
    \param System :: Simulation to use
    \param objN :: Index of object
    \param IPt :: initial point
  */
{
  ELog::RegMethod RegA("ObjectTrackPlane","addUnit");

  // Remove old track
  std::map<long int,LineTrack>::iterator mc=Items.find(objN);
  if (mc!=Items.end())
    Items.erase(mc);

  const Geometry::Vec3D TP=TargetPlane.closestPt(IPt);
  LineTrack A(IPt,TP);
  A.calculate(System);
  Items.insert(std::map<long int,LineTrack>::value_type(objN,A));
  return;
}  

void 
ObjectTrackPlane::write(std::ostream& OX) const
  /*!
    Write out track (mainly debug)
    \param OX :: Output stream
   */
{
  ELog::RegMethod RegA("ObjectTrackPlane","write");

  OX<<"WRITE"<<std::endl;
  std::map<long int,LineTrack>::const_iterator mc;
  for(mc=Items.begin();mc!=Items.end();mc++)
    OX<<mc->first<<" : "<<mc->second<<std::endl;
  return;
}
  
} // Namespace ModelSupport
