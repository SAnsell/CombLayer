/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/itemConstruct.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <functional>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "stringCombine.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Line.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedGroup.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "inputParam.h"

#include "pointConstruct.h" 
#include "itemConstruct.h" 

namespace tallySystem
{


void
itemConstruct::processItem(SimMCNP& System,
			   const mainSystem::inputParam& IParam,
			   const size_t Index)
  /*!
    Add point tally (s) as needed
    \param System :: SimMCNP to add tallies
    \param IParam :: Main input parameters
    \param Index :: index of the -T card
   */
{
  ELog::RegMethod RegA("itemConstruct","processItem");

  const size_t NItems=IParam.itemCnt("tally",Index);
  if (NItems<2)
    throw ColErr::IndexError<size_t>(NItems,2,
				     "Insufficient items for tally");

  const std::string PType(IParam.getValue<std::string>("item",Index,1)); 
  
  if (PType=="beamline")  // beamline : Number
    {
      const int beamNum=IParam.getValueError<int>("tally",Index,2,
					     "beamline number not given");

      const std::string modName=
	IParam.getDefValue<std::string>("","tally",Index,3);
      const long int viewIndex=
	IParam.getDefValue<long int>(0,"tally",Index,4);
      const double beamDist=
	IParam.getDefValue<double>(1000.0,"tally",Index,5);
      const double windowOffset=
	IParam.getDefValue<double>(0.0,"tally",Index,6);
      const double pointZRot=
	IParam.getDefValue<double>(0.0,"tally",Index,7);

      addBeamLineItem(System,beamNum-1,beamDist,modName,
		       viewIndex,windowOffset,pointZRot);
      return;
    }
  return;
}

void 
itemConstruct::addBeamLineItem(SimMCNP& System,
			       const int beamNum,
			       const double beamDist,
			       const std::string& modName,
			       const long int viewSurface,
			       const double windowOffset,
			       const double pointZRot) 
/*!
    Adds a beamline tally Item to the system
    \param System :: SimMCNP to add tallies
    \param beamNum :: Beamline to use [1-18]
    \param beamDist :: Distance from moderator face
    \param modName :: Moderator Name to view
    \param viewSurface :: Surface index [FC object]
    \param windowOffset :: Distance to move window towards tally point
    \param pointZRot :: Z axis rotation of the beamline
  */
{
  ELog::RegMethod RegA("itemConstruct","addBeamLineItem");
  
  std::vector<int> Planes;

  //int VSign((viewSurface<0) ? -1 : 1);

  const attachSystem::FixedComp* ModPtr;
  const attachSystem::FixedComp* ShutterPtr;

  ModPtr=System.getObject<attachSystem::FixedComp>(modName);  
  ShutterPtr=System.getObject<attachSystem::FixedComp>
    (StrFunc::makeString(std::string("shutter"),beamNum));

  if (!ShutterPtr)    
    throw ColErr::InContainerError<int>(beamNum,"Shutter Object not found");
  if (!ModPtr)    
    throw ColErr::InContainerError<std::string>
      (modName,"Moderator Object not found");

  // MODERATOR PLANE

  const int masterPlane=ModPtr->getExitWindow(viewSurface,Planes);
  
  const attachSystem::FixedGroup* TwinPtr=
    dynamic_cast<const attachSystem::FixedGroup*>(ShutterPtr);

  Geometry::Vec3D BAxis;
  Geometry::Vec3D shutterPoint;
  if (TwinPtr && TwinPtr->hasKey("Beam"))
    {
      BAxis=TwinPtr->getKey("Beam").getY()*-1.0;
      shutterPoint=TwinPtr->getKey("Beam").getLinkPt(1);
    }
  else
    {
      BAxis=ShutterPtr->getLinkAxis(1);
      shutterPoint=ShutterPtr->getLinkPt(1);
    }
  // CALC Intercept between Moderator boundary
  std::vector<Geometry::Vec3D> Window=
    pointConstruct::calcWindowIntercept(masterPlane,Planes,shutterPoint);

  // CALC intesect between shutter axis / shutter point and moderator plane
  std::vector<Geometry::Vec3D> OutPts;
  Geometry::Line LN(shutterPoint,-BAxis);
  Geometry::Plane SX(0,0);
  SX.setPlane(Window[0],Window[1],Window[2]);
  if (!LN.intersect(OutPts,SX))
    ELog::EM<<"Failed to intersect moderator window"<<ELog::endErr;
  const Geometry::Vec3D MidPt=OutPts.front();

  // Apply rotation
  if (fabs(pointZRot)>Geometry::zeroTol)
    {
      const Geometry::Vec3D Z=ShutterPtr->getZ();
      const Geometry::Quaternion Qxy=
	Geometry::Quaternion::calcQRotDeg(pointZRot,Z);
      Qxy.rotate(BAxis);
    }
  const Geometry::Vec3D tallyPoint=MidPt-BAxis*beamDist;
  
  const int tNum=System.nextTallyNum(5);

  std::transform(Window.begin(),Window.end(),Window.begin(),
		 std::bind(std::minus<Geometry::Vec3D>(),std::placeholders::_1,
			   BAxis*windowOffset));
  
  ELog::EM<<"Tally: "<<tNum<<" "<<MidPt-BAxis*beamDist<<ELog::endTrace;
  // (NORMAL VIEW):
  // tallySystem::setF5Position(System,tNum,RefPtr->getViewOrigin(beamNum),
  // 			     GS->getBeamAxis(),beamDist,0.0);
  return;
}


void
itemConstruct::writeHelp(std::ostream& OX) 
  /*!
    Write out help
    \param OX :: Output stream
  */
{
  OX<<
    "beamline [number] {modName,viewindex,beamDist,"
    "windowOffset,ZRotAngle}\n";
  return;
}


}  // NAMESPACE tallySystem
