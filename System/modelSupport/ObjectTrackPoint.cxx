/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   modelSupport/ObjectTrackPoint.cxx
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
#include <algorithm>
#include <cmath>
#include <complex> 
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map> 
#include <memory>
#include <set> 
#include <string>
#include <utility>
#include <vector>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LineUnit.h"
#include "LineTrack.h"
#include "ObjectTrackAct.h"
#include "ObjectTrackPoint.h"

namespace ModelSupport
{

ObjectTrackPoint::ObjectTrackPoint(Geometry::Vec3D  PtA) :
  ObjectTrackAct(),TargetPt(std::move(PtA))
  /*! 
    Constructor 
    \param PtA :: Target point to track from
  */
{}

ObjectTrackPoint::ObjectTrackPoint(const ObjectTrackPoint& A) :
  ObjectTrackAct(A),TargetPt(A.TargetPt)
   /*! 
    Copy Constructor 
    \param A :: ObjectTrackPoint to copy
  */
{}


ObjectTrackPoint&
ObjectTrackPoint::operator=(const ObjectTrackPoint& A) 
   /*! 
     Assignment operator
    \param A :: ObjectTrackPoint to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ObjectTrackAct::operator=(A);
      TargetPt=A.TargetPt;
    }
  return *this;
}

void
ObjectTrackPoint::addUnit(const Simulation& System,
			  const long int objN,
			  const Geometry::Vec3D& IPt)
  /*!
    Create a target track between the IPt and the target point
    \param System :: Simulation to use
    \param objN :: Index of object
    \param IPt :: initial point
  */
{
  ELog::RegMethod RegA("ObjectTrackPoint","addUnit");

  // Remove old track
  std::map<long int,LineTrack>::iterator mc=Items.find(objN);
  if (mc!=Items.end())
    Items.erase(mc);

  LineTrack A(IPt,TargetPt);
  A.calculate(System);
  Items.insert(std::map<long int,LineTrack>::value_type(objN,A));
  
  return;
}  

void 
ObjectTrackPoint::write(std::ostream& OX) const
  /*!
    Write out track (mainly debug)
    \param OX :: Output stream
   */
{
  ELog::RegMethod RegA("ObjectTrackPoint","write");

  OX<<"WRITE"<<std::endl;
  std::map<long int,LineTrack>::const_iterator mc;
  for(mc=Items.begin();mc!=Items.end();mc++)
    OX<<mc->first<<" : "<<mc->second<<std::endl;
  return;
}
  
} // Namespace ModelSupport
