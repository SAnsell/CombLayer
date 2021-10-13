/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/RingDoor.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include "surfRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "RingDoor.h"


namespace xraySystem
{

RingDoor::RingDoor(const std::string& Key) :
  attachSystem::ContainedGroup("Door","Tubes"),
  attachSystem::FixedOffset(Key,6),
  attachSystem::CellMap(),
  attachSystem::ExternalCut()
  /*!
    Default constructor
    \param Key :: Key name for variables
  */
{}
  
void
RingDoor::populate(const FuncDataBase& Control)
  /*!
    Sets the size of the object
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("RingDoor","populate");

  FixedOffset::populate(Control);
  
  innerHeight=Control.EvalVar<double>(keyName+"InnerHeight");
  innerWidth=Control.EvalVar<double>(keyName+"InnerWidth");

  innerTopGap=Control.EvalVar<double>(keyName+"InnerTopGap");
  outerTopGap=Control.EvalVar<double>(keyName+"OuterTopGap");
  gapSpace=Control.EvalVar<double>(keyName+"GapSpace");
  innerThick=Control.EvalVar<double>(keyName+"InnerThick");

  outerHeight=Control.EvalVar<double>(keyName+"OuterHeight");
  outerWidth=Control.EvalVar<double>(keyName+"OuterWidth");

  tubeRadius=Control.EvalVar<double>(keyName+"TubeRadius");
  tubeXStep=Control.EvalVar<double>(keyName+"TubeXStep");
  tubeZStep=Control.EvalVar<double>(keyName+"TubeZStep");

  underStepHeight=Control.EvalVar<double>(keyName+"UnderStepHeight");
  underStepWidth=Control.EvalVar<double>(keyName+"UnderStepWidth");
  underStepXSep=Control.EvalVar<double>(keyName+"UnderStepXSep");

  underAMat=ModelSupport::EvalDefMat(Control,keyName+"UnderAMat",0);
  underBMat=ModelSupport::EvalDefMat(Control,keyName+"UnderBMat",0);
  tubeMat=ModelSupport::EvalMat<int>(Control,keyName+"TubeMat");
  doorMat=ModelSupport::EvalMat<int>(Control,keyName+"DoorMat");
  
  return;
}

void
RingDoor::createUnitVector(const attachSystem::FixedComp& FC,
				 const long int sideIndex)
  /*!
    Create the unit vectors: Note only to construct front/back surf
    Note that Y points OUT of the ring
    \param FC :: Centre point
    \param sideIndex :: Side index
  */
{
  ELog::RegMethod RegA("RingDoor","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  const HeadRule& HR=ExternalCut::getRule("innerWall");

  const int SN=HR.getPrimarySurface();
  const Geometry::Plane* PPtr=
    dynamic_cast<const Geometry::Plane*>(HR.getSurface(SN));

  if (PPtr)
    {
      Geometry::Vec3D PAxis=PPtr->getNormal();
      if (Y.dotProd(PAxis)*SN < -Geometry::zeroTol)
	PAxis*=-1;
      FixedComp::reOrientate(1,PAxis);
    }
  applyOffset();
  return;
}

void
RingDoor::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("RingDoor","createSurface");

  // InnerWall and OuterWall MUST be set
  if (!ExternalCut::isActive("innerWall") ||
      !ExternalCut::isActive("outerWall") ||
      !ExternalCut::isActive("floor"))
    throw ColErr::InContainerError<std::string>
      ("InnerWall/OuterWall/floor","Door:"+keyName);

  // origin in on outer wall:
  
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(innerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(innerWidth/2.0),X);
  ExternalCut::makeShiftedSurf(SMap,"floor",buildIndex+6,Z,innerHeight);

  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(gapSpace+innerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(gapSpace+innerWidth/2.0),X);
  ExternalCut::makeShiftedSurf(SMap,"floor",buildIndex+16,Z,
			       innerTopGap+innerHeight);

  
  ModelSupport::buildPlane(SMap,buildIndex+23,Origin-X*(outerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*(outerWidth/2.0),X);

  ExternalCut::makeShiftedSurf(SMap,"floor",buildIndex+26,Z,outerHeight);
  
  ModelSupport::buildPlane(SMap,buildIndex+33,
			   Origin-X*(gapSpace+outerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+34,
			   Origin+X*(gapSpace+outerWidth/2.0),X);
  // ModelSupport::buildPlane(SMap,buildIndex+36,
  // 			   Origin+Z*(outerTopGap+outerHeight/2.0),Z);
  ExternalCut::makeShiftedSurf(SMap,"floor",buildIndex+36,Z,
			       outerTopGap+outerHeight);
  
  // Y Points out of ring:
  ExternalCut::makeShiftedSurf
    (SMap,"innerWall",buildIndex+200,Y,innerThick);
  ExternalCut::makeShiftedSurf
    (SMap,"innerWall",buildIndex+201,Y,innerThick+gapSpace);

  // lift step
  ExternalCut::makeShiftedSurf
    (SMap,"floor",buildIndex+1005,Z,underStepHeight);

  ModelSupport::buildPlane
    (SMap,buildIndex+1003,Origin-X*(underStepWidth+underStepXSep/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+1004,Origin-X*(underStepXSep/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+1013,Origin+X*(underStepXSep/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+1014,Origin+X*(underStepWidth+underStepXSep/2.0),X);
  
  
  // Tubes:
  ModelSupport::buildCylinder
    (SMap,buildIndex+507,Origin-X*tubeXStep+Z*tubeZStep,Y,tubeRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+517,Origin+Z*tubeZStep,Y,tubeRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+527,Origin+X*tubeXStep+Z*tubeZStep,Y,tubeRadius);
    
  return;
}

void
RingDoor::createObjects(Simulation& System) 
  /*!
    Creates the colllimator block
    \param System :: Simuation for object
  */
{
  ELog::RegMethod RegA("RingDoor","createObjects");

  HeadRule HR;
  const HeadRule innerHR=ExternalCut::getRule("innerWall");
  const HeadRule outerHR=ExternalCut::getRule("outerWall");
  const HeadRule floorHR=ExternalCut::getRule("floor");

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-200 3 -4 -6 (-1003:1004:1005) (-1013:1014:1005)");
  makeCell("InnerDoor",System,cellIndex++,doorMat,0.0,HR*innerHR*floorHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-200 (-3:4:6) 13 -14 -16 ");
  makeCell("InnerGap",System,cellIndex++,0,0.0,HR*innerHR*floorHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-200 (-13:14:16) 33 -34 -36 ");
  makeCell("InnerExtra",System,cellIndex++,doorMat,0.0,HR*innerHR*floorHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"200 -201 3 -4 -6 (-1003:1004:1005) (-1013:1014:1005) ");
  makeCell("OuterStrip",System,cellIndex++,doorMat,0.0,HR*floorHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"200 -201 23 -24 -26 (-3:4:6) ");
  makeCell("MidGap",System,cellIndex++,0,0.0,HR*floorHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"201 23 -24 -26 (-1003:1004:1005) (-1013:1014:1005)");
  makeCell("OuterDoor",System,cellIndex++,doorMat,0.0,HR*outerHR*floorHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"200 (-23:24:26) 33 -34 -36 ");
  makeCell("OuterGap",System,cellIndex++,0,0.0,HR*outerHR*floorHR);

  // Tubes
  HR=ModelSupport::getHeadRule(SMap,buildIndex," -507 ");
  makeCell("OuterGap",System,cellIndex++,tubeMat,0.0,HR*outerHR*innerHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," -517 ");
  makeCell("OuterGap",System,cellIndex++,tubeMat,0.0,HR*outerHR*innerHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex," -527 ");
  makeCell("OuterGap",System,cellIndex++,tubeMat,0.0,HR*outerHR*innerHR);

  // Lift points
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1003 -1004 -1005 ");
  makeCell("LiftA",System,cellIndex++,underAMat,
	   0.0,HR*outerHR*innerHR*floorHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1013 -1014 -1005 ");
  makeCell("LiftB",System,cellIndex++,underBMat,
	   0.0,HR*outerHR*innerHR*floorHR);


  
  // main door
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 33 -34 -36 ");
  addOuterSurf("Door",HR);

  // extra tubes
  HR=ModelSupport::getHeadRule(SMap,buildIndex," (-507 : -517 : -527) ");
  addOuterSurf("Tubes",HR);
  return;
}

void
RingDoor::createLinks()
  /*!
    Construct the links for the system
    Note that Y points OUT of the ring
  */
{
  ELog::RegMethod RegA("RingDoor","createLinks");

  ExternalCut::createLink("innerWall",*this,0,Origin,-Y);
  ExternalCut::createLink("outerWall",*this,1,Origin,Y);
  
  return;
}

void
RingDoor::createAll(Simulation& System,
                     const attachSystem::FixedComp& FC,
                     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation 
    \param FC :: Fixed component to set axis etc
    \param sideIndex :: position of linkpoint
  */
{
  ELog::RegMethod RegA("RingDoor","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

  
}  // NAMESPACE xraySystem
