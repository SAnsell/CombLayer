/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Build/t1CylVessel.cxx
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
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "Window.h"
#include "t1CylVessel.h"

namespace shutterSystem
{

t1CylVessel::t1CylVessel(const std::string& Key)  : 
  attachSystem::FixedOffset(Key,3),attachSystem::ContainedComp(),
  steelCell(0),voidCell(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key to use
  */
{}

t1CylVessel::t1CylVessel(const t1CylVessel& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  voidYoffset(A.voidYoffset),
  radius(A.radius),clearance(A.clearance),baseRadius(A.baseRadius),
  topRadius(A.topRadius),wallThick(A.wallThick),
  height(A.height),wallMat(A.wallMat),ports(A.ports),
  steelCell(A.steelCell),voidCell(A.voidCell)
  /*!
    Copy constructor
    \param A :: t1CylVessel to copy
  */
{}

t1CylVessel&
t1CylVessel::operator=(const t1CylVessel& A)
  /*!
    Assignment operator
    \param A :: t1CylVessel to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      voidYoffset=A.voidYoffset;
      radius=A.radius;
      clearance=A.clearance;
      baseRadius=A.baseRadius;
      topRadius=A.topRadius;
      wallThick=A.wallThick;
      height=A.height;
      wallMat=A.wallMat;
      ports=A.ports;
      steelCell=A.steelCell;
      voidCell=A.voidCell;
    }
  return *this;
}

t1CylVessel::~t1CylVessel() 
  /*!
    Destructor
  */
{}

void
t1CylVessel::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Database to use
  */
{
  ELog::RegMethod RegA("t1CylVessel","populate");

  FixedOffset::populate(Control);
  voidYoffset=Control.EvalVar<double>("voidYoffset");
  yStep=voidYoffset;
  radius=Control.EvalVar<double>(keyName+"Radius");   
  clearance=Control.EvalVar<double>(keyName+"Clearance");   
  height=Control.EvalVar<double>(keyName+"Height");   
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  baseRadius=Control.EvalVar<double>(keyName+"BaseRadius");
  topRadius=Control.EvalVar<double>(keyName+"TopRadius");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
      
  return;
}

void
t1CylVessel::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("t1CylVessel","createSurfaces");
  //
  // OUTER VOID
  //
  const double baseH=height/2.0-sqrt(baseRadius*baseRadius-radius*radius);
  const double topH=height/2.0-sqrt(topRadius*topRadius-radius*radius);

  FixedComp::setConnect(1,Origin-Z*(baseH+baseRadius+wallThick),-Z);
  FixedComp::setConnect(2,Origin+Z*(topH+topRadius+wallThick),Z);
  
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,radius);  
  // Top/base cut plane:
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height/2.0,Z);
  ModelSupport::buildSphere(SMap,buildIndex+8,Origin-Z*baseH,baseRadius);
  ModelSupport::buildSphere(SMap,buildIndex+9,Origin+Z*topH,topRadius);

  // Inner wall layer
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Z,radius+wallThick);
  // Top/base cut plane:
  ModelSupport::buildSphere(SMap,buildIndex+18,Origin-Z*baseH,
			    baseRadius+wallThick);
  ModelSupport::buildSphere(SMap,buildIndex+19,Origin+Z*topH,
			    topRadius+wallThick);

  ModelSupport::buildCylinder(SMap,buildIndex+27,
			      Origin,Z,radius+wallThick+clearance);

  return;
}

void
t1CylVessel::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("t1CylVessel","createObjects");
  
  std::string Out;
  // Inner voids
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-7 ((-8 -5) : (-9 6) : (5 -6)) ");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  voidCell=cellIndex-1;
  
  // Steel layers [in components]
  Out=ModelSupport::getComposite(SMap,buildIndex, "5 -6 -17 7 ");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  steelCell=cellIndex-1;

  Out=ModelSupport::getComposite(SMap,buildIndex, " -19 9 6 -17 ");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex, " -18 8 -5 -17 ");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));

  // clearance layer
  Out=ModelSupport::getComposite(SMap,buildIndex, "(5:-18) (-6:-19) -27 17 ");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  
  // Outer Boundary : 
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-27 ((-18 -5) : (-19 6) : (5 -6)) ");
  addOuterSurf(Out);
  
  return;
}

void
t1CylVessel::createWindows(Simulation& System)
  /*!
    Adds the windows to the system
    \param System :: Simulation object ot add
  */
{
  ELog::RegMethod RegA("t1CylVessel","createWindows");


  const FuncDataBase& Control=System.getDataBase();
  const size_t nWin=Control.EvalVar<size_t>(keyName+"NPorts");

  const std::string CKey=keyName+"PortCentre";
  const std::string CHgh=keyName+"PortCHeight";
  const std::string AKey=keyName+"PortAxis";
  const std::string AAng=keyName+"PortAngle";
  const std::string HKey=keyName+"PortHeight";
  const std::string WKey=keyName+"PortWidth";

  for(size_t i=0;i<nWin;i++)
    {
      const std::string IndexStr(std::to_string(i+1));
      ports.push_back
	(constructSystem::Window("t1Port"+std::to_string(i+1)));
      
      Geometry::Vec3D PCent;
      Geometry::Vec3D PAxis;
      // Centres:
      PCent=Control.EvalTail<Geometry::Vec3D>(CKey,CHgh,IndexStr);
      // Angles:
      if (Control.hasVariable(AKey+IndexStr))
	PAxis=Control.EvalVar<Geometry::Vec3D>(AKey+IndexStr);
      else
	{
	  const double angle=Control.EvalVar<double>(AAng+IndexStr);
	  PAxis=Y;
	  Geometry::Quaternion::calcQRotDeg(angle,Z).rotate(PAxis);
	}
      // Height / Width

      const double PHeight=Control.EvalPair<double>(HKey+IndexStr,HKey);
      const double PWidth=Control.EvalPair<double>(WKey+IndexStr,WKey);
      ports.back().setCentre(PCent+Origin,PAxis);
      ports.back().setSize(PHeight,PWidth);
    }
  // CREATE OBJECTS
  std::vector<constructSystem::Window>::iterator vc;
  for(vc=ports.begin();vc!=ports.end();vc++)
    {
      vc->setBaseCell(steelCell);
      vc->addInsertCell(steelCell);
      vc->createAll(System,*this,0);
    }
  return;
}

void
t1CylVessel::createLinks()
  /*!
    Create links
   */
{
  ELog::RegMethod RegA("t1CylVessel","createLinks");

  FixedComp::setConnect(0,Origin,-X);

  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+27));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+18));  // base
  FixedComp::addLinkSurf(1,-SMap.realSurf(buildIndex+5));
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+19));  // top
  FixedComp::addLinkSurf(2,SMap.realSurf(buildIndex+6));

  return;
}

void
t1CylVessel::createStatus(const Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Create the void vessel status so origins etc are correctly moved
    \param System :: Simulation to process
  */
{
  ELog::RegMethod RegA("t1CylVessel","createStatus");
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  return;
}

void
t1CylVessel::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Create the void vessel
    \param System :: Simulation to process
  */
{
  ELog::RegMethod RegA("t1CylVessel","createAll");
  createStatus(System,FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createWindows(System);
  createLinks();
  insertObjects(System);
  return;
}
  
}  // NAMESPACE shutterSystem
