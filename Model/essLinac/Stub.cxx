/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essLinac/Stub.cxx
 *
 * Copyright (c) 2018-2021 by Konstantin Batkov
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
#include "Line.h"
#include "Rules.h"
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "AttachSupport.h"
#include "Stub.h"

namespace essSystem
{

Stub::Stub(const std::string& Key,const size_t Index) :
  attachSystem::ContainedGroup(),
  attachSystem::FixedOffset(Key+std::to_string(Index),15),
  attachSystem::CellMap(),
  attachSystem::FrontBackCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
    \param Index :: Index 
  */
{}

Stub::Stub(const Stub& A) :
  attachSystem::ContainedGroup(A),
  attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),width(A.width),height(A.height),
  wallThick(A.wallThick),
  mainMat(A.mainMat),wallMat(A.wallMat),
  nDucts(A.nDucts)
  /*!
    Copy constructor
    \param A :: Stub to copy
  */
{}

Stub&
Stub::operator=(const Stub& A)
  /*!
    Assignment operator
    \param A :: Stub to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      length=A.length;
      width=A.width;
      height=A.height;
      wallThick=A.wallThick;
      mainMat=A.mainMat;
      wallMat=A.wallMat;
      nDucts=A.nDucts;
    }
  return *this;
}

Stub*
Stub::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new Stub(*this);
}

Stub::~Stub()
  /*!
    Destructor
  */
{}

void
Stub::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("Stub","populate");

  FixedOffset::populate(Control);

  const size_t Nlegs(3);
  for (size_t i=0; i<Nlegs-1; i++)
    {
      const double L = Control.EvalVar<double>
	(keyName+"Length"+std::to_string(i+1));
      length.push_back(L);
    }

  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  nDucts=Control.EvalDefVar<size_t>(keyName+"NDucts",0);

  return;
}


void
Stub::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Stub","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(width/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(width/2.0),Y);

  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(length[0]),X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(width/2.0+wallThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(width/2.0+wallThick),Y);

  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(length[0]+wallThick),X);

  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(height/2.0+wallThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(height/2.0+wallThick),Z);

  ModelSupport::buildShiftedPlane(SMap,buildIndex+104,
				  SMap.realPtr<Geometry::Plane>(buildIndex+4),
				  -height);

  ModelSupport::buildShiftedPlane(SMap,buildIndex+105,
				  SMap.realPtr<Geometry::Plane>(buildIndex+5),
				  length[1]-height);

  ModelSupport::buildShiftedPlane(SMap,buildIndex+106,
				  SMap.realPtr<Geometry::Plane>(buildIndex+5),
				  length[1]);

  ModelSupport::buildShiftedPlane(SMap,buildIndex+114,
				  SMap.realPtr<Geometry::Plane>(buildIndex+104),
				  -wallThick);

  ModelSupport::buildShiftedPlane(SMap,buildIndex+115,
				  SMap.realPtr<Geometry::Plane>(buildIndex+105),
				  -wallThick);

  ModelSupport::buildShiftedPlane(SMap,buildIndex+116,
				  SMap.realPtr<Geometry::Plane>(buildIndex+106),
				  wallThick);
  return;
}

void
Stub::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Stub","createObjects");

  const HeadRule frontHR=getFrontRule();
  const HeadRule backHR=getBackRule();
  
  HeadRule HR;
  
  attachSystem::ContainedGroup::addCC("Full");
  attachSystem::ContainedGroup::addCC("Leg1");
  attachSystem::ContainedGroup::addCC("Leg2");
  attachSystem::ContainedGroup::addCC("Leg3");
    
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -4 5 -6");
  makeCell("Leg1",System,cellIndex++,mainMat,0.0,HR*backHR);

  if (wallThick>Geometry::zeroTol)
    {
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"11 -12 -4 15 -16 (-1:2:4:-5)");
      makeCell("Leg1",System,cellIndex++,wallMat,0.0,HR*backHR);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -104 6 -16");
      makeCell("Leg1",System,cellIndex++,wallMat,0.0,HR*backHR);
    }

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 -14 15 -16");
  addOuterSurf("Leg1",HR*backHR);
  addOuterUnionSurf("Full",HR*backHR);

  // LEG 2:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 104 -4 6 -106 ");
  makeCell("Leg2",System,cellIndex++,mainMat,0.0,HR*backHR);

  if (wallThick>Geometry::zeroTol)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 4 -14 15 -105");
      makeCell("Leg2",System,cellIndex++,wallMat,0.0,HR);

      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"11 -12 114 -4 16 -105 (-1:2:-104:4:-16:105)");
      makeCell("Leg2",System,cellIndex++,wallMat,0.0,HR);
    }
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 114 -14 6 -116");
  addOuterSurf("Leg2",HR);
  addOuterUnionSurf("Full",HR);

  // LEG 3:
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 4 105 -106");
  makeCell("Leg3",System,cellIndex++,mainMat,0.0,HR*frontHR);

  if (wallThick>Geometry::zeroTol)
    {
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"11 -12 114 105 -116 (-1:2:-104:-105:106)");
      makeCell("Leg3",System,cellIndex++,wallMat,0.0,HR*frontHR);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 14 115 -105");
      makeCell("Leg3",System,cellIndex++,wallMat,0.0,HR*frontHR);
    }
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 4 115 -116");
  HR*=frontHR;
  addOuterSurf("Leg3",HR);
  addOuterUnionSurf("Full",HR);

  return;
}


void
Stub::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("Stub","createLinks");

  // this creates first two links:
  FrontBackCut::createLinks(*this, Origin, X);

  // Leg 1
  FixedComp::setConnect(2,Origin+X*(length[0]+wallThick),X);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+14));

  FixedComp::setConnect(3,Origin+X*(length[0]-height)-Y*(width/2.0+wallThick),-Y);
  FixedComp::setLinkSurf(3,-SMap.realSurf(buildIndex+11));

  FixedComp::setConnect(4,Origin+X*(length[0]-height)+Y*(width/2.0+wallThick),Y);
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+12));

  FixedComp::setConnect(5,Origin+X*(length[0]-height)-Z*(height/2.0+wallThick),-Z);
  FixedComp::setLinkSurf(5,-SMap.realSurf(buildIndex+15));

  FixedComp::setConnect(6,Origin+X*(length[0]-height-wallThick*2)+Z*(height/2.0+wallThick),Z);
  FixedComp::setLinkSurf(6,SMap.realSurf(buildIndex+16));

  // Leg 2
  FixedComp::setConnect(7,Origin+X*(length[0]-height/2.0)+
			Z*(length[1]/2.0+height/2.0)-Y*(width/2.0+wallThick),-Y);
  FixedComp::setLinkSurf(7,-SMap.realSurf(buildIndex+11));

  FixedComp::setConnect(8,Origin+X*(length[0]-height/2.0)+
			Z*(length[1]/2.0+height/2.0)+Y*(width/2.0+wallThick),-Y);
  FixedComp::setLinkSurf(8,SMap.realSurf(buildIndex+12));

  FixedComp::setConnect(9,Origin+X*(length[0]-height/2.0)+
			Z*(length[1]-height/2+wallThick),Z);
  FixedComp::setLinkSurf(9,SMap.realSurf(buildIndex+116));

  // Leg 3
  FixedComp::setConnect(10,Origin+X*(length[0]-height-wallThick)+
			Z*(length[1]-height),-X);
  FixedComp::setLinkSurf(10,-SMap.realSurf(buildIndex+114));

  FixedComp::setConnect(11,Origin+X*(length[0]+2*wallThick)+
			Z*(length[1]-height)-Y*(width/2.0+wallThick),-Y);
  FixedComp::setLinkSurf(11,-SMap.realSurf(buildIndex+11));

  FixedComp::setConnect(12,Origin+X*(length[0]+2*wallThick)+
			Z*(length[1]-height)+Y*(width/2.0+wallThick),Y);
  FixedComp::setLinkSurf(12,SMap.realSurf(buildIndex+12));

  FixedComp::setConnect(13,Origin+X*(length[0]+2*wallThick)+
			Z*(length[1]-height-height/2-wallThick),-Z);
  FixedComp::setLinkSurf(13,-SMap.realSurf(buildIndex+115));

  FixedComp::setConnect(14,Origin+X*(length[0]+2*wallThick)+
			Z*(length[1]-height+height/2+wallThick),Z);
  FixedComp::setLinkSurf(14,SMap.realSurf(buildIndex+116));

  return;
}




void
Stub::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("Stub","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  for (size_t i=0; i<nDucts; i++)
    {
      std::shared_ptr<Stub> duct(new Stub(keyName+"Duct", i+1));
      OR.addObject(duct);
      duct->setFront(*this);
      duct->setBack(*this);
      duct->createAll(System,*this,0);
      attachSystem::addToInsertSurfCtrl(System,*this,duct->getCC("Full"));
    }

  return;
}

}  // essSystem
