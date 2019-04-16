/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/RingDoor.cxx
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
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
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
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "SurInter.h"
#include "RingDoor.h"


namespace xraySystem
{

RingDoor::RingDoor(const std::string& Key) :
  attachSystem::ContainedComp(),
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
      !ExternalCut::isActive("outerWall"))
    throw ColErr::InContainerError<std::string>
      ("InnerWall/OuterWall","Door:"+keyName);

  // origin in on outer wall:
  
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(innerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(innerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(innerHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(innerHeight/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(gapSpace+innerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(gapSpace+innerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(gapSpace+innerHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(innerTopGap+innerHeight/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+23,Origin-X*(outerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*(outerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+25,Origin-Z*(outerHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,Origin+Z*(outerHeight/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+33,
			   Origin-X*(gapSpace+outerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+34,
			   Origin+X*(gapSpace+outerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+35,
			   Origin-Z*(gapSpace+outerHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+36,
			   Origin+Z*(outerTopGap+outerHeight/2.0),Z);


  ExternalCut::makeShiftedSurf
    (SMap,"innerWall",buildIndex+200,-1,Y,innerThick);
  ExternalCut::makeShiftedSurf
    (SMap,"innerWall",buildIndex+201,-1,Y,innerThick+gapSpace);
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

  std::string Out;
  const std::string innerStr=ExternalCut::getRuleStr("innerWall");
  const std::string outerStr=ExternalCut::getRuleStr("outerWall");
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"200 3 -4 5 -6 ");
  makeCell("InnerDoor",System,cellIndex++,doorMat,0.0,Out+innerStr);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"200 (-3:4:-5:6) 13 -14 15 -16 ");
  makeCell("InnerGap",System,cellIndex++,0,0.0,Out+innerStr);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"200 (-13:14:-15:16) 33 -34 35 -36 ");
  makeCell("InnerExtra",System,cellIndex++,doorMat,0.0,Out+innerStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-200 201 3 -4 5 -6 ");
  makeCell("OuterStrip",System,cellIndex++,doorMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"-200 201 23 -24 25 -26 (-3:4:-5:6) ");
  makeCell("MidGap",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-201 23 -24 25 -26 ");
  makeCell("OuterDoor",System,cellIndex++,doorMat,0.0,Out+outerStr);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"-200 (-23:24:-25:26) 33 -34 35 -36 ");
  makeCell("OuterGap",System,cellIndex++,0,0.0,Out+outerStr);
  
  // needs to be group
  Out=ModelSupport::getComposite(SMap,buildIndex," 33 -34 35 -36 ");
  addOuterSurf(Out);
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
