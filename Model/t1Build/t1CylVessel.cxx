/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Build/t1CylVessel.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "PointOperation.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "Window.h"
#include "World.h"
#include "t1CylVessel.h"

namespace shutterSystem
{

t1CylVessel::t1CylVessel(const std::string& Key)  : 
  attachSystem::FixedComp(Key,3),attachSystem::ContainedComp(),
  voidIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(voidIndex+1),populated(0),steelCell(0),voidCell(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key to use
  */
{}

t1CylVessel::t1CylVessel(const t1CylVessel& A) : 
  attachSystem::FixedComp(A),attachSystem::ContainedComp(A),
  voidIndex(A.voidIndex),cellIndex(A.cellIndex),
  populated(A.populated),voidYoffset(A.voidYoffset),
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
      attachSystem::FixedComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
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
t1CylVessel::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("t1CylVessel","populate");

  const FuncDataBase& Control=System.getDataBase();

  voidYoffset=Control.EvalVar<double>("voidYoffset");   
  radius=Control.EvalVar<double>(keyName+"Radius");   
  clearance=Control.EvalVar<double>(keyName+"Clearance");   
  height=Control.EvalVar<double>(keyName+"Height");   
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  baseRadius=Control.EvalVar<double>(keyName+"BaseRadius");
  topRadius=Control.EvalVar<double>(keyName+"TopRadius");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
      
  populated = 1;
  return;
}

void
t1CylVessel::createUnitVector()
  /*!
    Create the unit vectors
  */
{
  ELog::RegMethod RegA("t1CylVessel","createUnitVector");

  attachSystem::FixedComp::createUnitVector(World::masterOrigin());
  applyShift(0.0,voidYoffset,0.0);
  
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
  
  ModelSupport::buildCylinder(SMap,voidIndex+7,Origin,Z,radius);  
  // Top/base cut plane:
  ModelSupport::buildPlane(SMap,voidIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,voidIndex+6,Origin+Z*height/2.0,Z);
  ModelSupport::buildSphere(SMap,voidIndex+8,Origin-Z*baseH,baseRadius);
  ModelSupport::buildSphere(SMap,voidIndex+9,Origin+Z*topH,topRadius);

  // Inner wall layer
  ModelSupport::buildCylinder(SMap,voidIndex+17,Origin,Z,radius+wallThick);
  // Top/base cut plane:
  ModelSupport::buildSphere(SMap,voidIndex+18,Origin-Z*baseH,
			    baseRadius+wallThick);
  ModelSupport::buildSphere(SMap,voidIndex+19,Origin+Z*topH,
			    topRadius+wallThick);

  ModelSupport::buildCylinder(SMap,voidIndex+27,
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
  Out=ModelSupport::getComposite(SMap,voidIndex,
				 "-7 ((-8 -5) : (-9 6) : (5 -6)) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  voidCell=cellIndex-1;
  
  // Steel layers [in components]
  Out=ModelSupport::getComposite(SMap,voidIndex, "5 -6 -17 7 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  steelCell=cellIndex-1;

  Out=ModelSupport::getComposite(SMap,voidIndex, " -19 9 6 -17 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,voidIndex, " -18 8 -5 -17 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  // clearance layer
  Out=ModelSupport::getComposite(SMap,voidIndex, "(5:-18) (-6:-19) -27 17 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  
  // Outer Boundary : 
  Out=ModelSupport::getComposite(SMap,voidIndex,
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
      ports.push_back(constructSystem::Window
		      (StrFunc::makeString("t1Port",i+1)));
      Geometry::Vec3D PCent;
      Geometry::Vec3D PAxis;
      // Centres:
      if (Control.hasVariable(StrFunc::makeString(CKey,i+1)))
	PCent=Control.EvalVar<Geometry::Vec3D>(StrFunc::makeString(CKey,i+1));
      else
	PCent=Z*Control.EvalVar<double>(StrFunc::makeString(CHgh,i+1));
      // Angles:
      if (Control.hasVariable(StrFunc::makeString(AKey,i+1)))
	PAxis=Control.EvalVar<Geometry::Vec3D>(StrFunc::makeString(AKey,i+1));
      else
	{
	  const double angle=
	    Control.EvalVar<double>(StrFunc::makeString(AAng,i+1));
	  PAxis=Y;
	  Geometry::Quaternion::calcQRotDeg(angle,Z).rotate(PAxis);
	}
      // Height / Width

      const double PHeight=Control.EvalPair<double>
	(StrFunc::makeString(HKey,i+1),HKey);
      const double PWidth=Control.EvalPair<double>
	(StrFunc::makeString(WKey,i+1),WKey);
      ports.back().setCentre(PCent+Origin,PAxis);
      ports.back().setSize(PHeight,PWidth);
    }
  // CREATE OBJECTS
  std::vector<constructSystem::Window>::iterator vc;
  for(vc=ports.begin();vc!=ports.end();vc++)
    {
      vc->setBaseCell(steelCell);
      vc->addInsertCell(steelCell);
      vc->createAll(System,*this);
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

  FixedComp::setLinkSurf(0,SMap.realSurf(voidIndex+27));
  FixedComp::setLinkSurf(1,SMap.realSurf(voidIndex+18));  // base
  FixedComp::addLinkSurf(1,-SMap.realSurf(voidIndex+5));
  FixedComp::setLinkSurf(2,SMap.realSurf(voidIndex+19));  // top
  FixedComp::addLinkSurf(2,SMap.realSurf(voidIndex+6));

  return;
}

void
t1CylVessel::createStatus(const Simulation& System)
  /*!
    Create the void vessel status so origins etc are correctly moved
    \param System :: Simulation to process
  */
{
  ELog::RegMethod RegA("t1CylVessel","createStatus");
  populate(System);
  createUnitVector();
  return;
}

void
t1CylVessel::createAll(Simulation& System)
  /*!
    Create the void vessel
    \param System :: Simulation to process
  */
{
  ELog::RegMethod RegA("t1CylVessel","createAll");
  createStatus(System);
  createSurfaces();
  createObjects(System);
  createWindows(System);
  createLinks();
  insertObjects(System);
  return;
}
  
}  // NAMESPACE shutterSystem
