/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   construct/DomeConnector.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include <array>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "Importance.h"
#include "Object.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "portItem.h"
#include "portSet.h"

#include "DomeConnector.h"

namespace constructSystem
{

DomeConnector::DomeConnector(const std::string& Key) :
  attachSystem::FixedRotate(Key,2),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut(),
  PSet(*this)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


DomeConnector::~DomeConnector()
  /*!
    Destructor
  */
{}

void
DomeConnector::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("DomeConnector","populate");

  FixedRotate::populate(Control);

  // Void + Fe special:
  curveRadius=Control.EvalVar<double>(keyName+"CurveRadius");
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  curveStep=Control.EvalVar<double>(keyName+"CurveStep");
  joinStep=Control.EvalVar<double>(keyName+"JoinStep");
  flatLen=Control.EvalVar<double>(keyName+"FlatLen");
  plateThick=Control.EvalVar<double>(keyName+"PlateThick");
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");

  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");


  return;
}

void
DomeConnector::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("DomeConnector","createSurfaces");

  /*
    Create a sphere with radius R and cord at c(curveRadius) from the
    top and x from the centre so R=c+x, let L be the horrizontal distance
   */
  const double& c=curveStep;
  const double& L=curveRadius;
  const double radius= (L*L-c*c)/(2.0*c);
  const double x=Radius-c;
  const Geometry::Vec3D rCentre=Origin+Y*x; 
  
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildSphere(SMap,buildIndex+8,rCentre,Radius);
  ModelSupport::buildSphere(SMap,buildIndex+18,rCentre+Y*plateThick,Radius);


  // length of short cone (curveRadius at origin)
  const double sConeLen=(L*joinLen)/(innerRadius-joinLen);
  const double tTheta=atan(L/sConeLen)*180/M_PI;
  const Geometry::Vec3D coneCentre=Origin-Y*sConeLen;
  ModelSupport::buildCone(SMap,buildIndex+28,coneCentre,Y,tTheta);
  /*
    Create a Cone with radius of 2*L at Origin and 2*innerRadius at
    joinLen lower.
   */  
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,innerRadius);

  
  return;
}

void
DomeConnector::createObjects(Simulation& System)
  /*!

    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("DomeConnector","createObjects");

  HeadRule HR;

  const HeadRule frontHR=getRule("plate");
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex," -8 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"(-18:-2) 8 -207 ");
  makeCell("Dome",System,cellIndex++,mat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"(-18:-2) -207");
  addOuterSurf(HR*frontHR);

  return;
}


void
DomeConnector::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("DomeConnector","createLinks");

  ExternalCut::createLink("plate",*this,0,Origin,-Y);  //front and back
  ExternalCut::createLink("plate",*this,1,Origin,Y);  //front and back

  return;
}

void
DomeConnector::createPorts(Simulation& System)
  /*!
    Construct port
    \parma System :: Simulation to use
   */
{
  ELog::RegMethod RegA("DomeConnector","createPorts");

  const HeadRule frontHR=getRule("plate");
  
  MonteCarlo::Object* insertObj= 
    this->getCellObject(System,"Dome");
  const HeadRule innerHR=
    ModelSupport::getHeadRule(SMap,buildIndex,"8")*frontHR;
  const HeadRule outerHR=
    ModelSupport::getHeadRule(SMap,buildIndex,"18")*frontHR;

  for(const auto CN : insertCells)
    PSet.addInsertPortCells(CN);

  PSet.createPorts(System,insertObj,innerHR,outerHR);

  return;
}

void
DomeConnector::insertInCell(MonteCarlo::Object& outerObj) const
  /*!
    Insert both the flange adn the ports in a cell 
    \param outerObj :: Cell to insert
   */
{
  ELog::RegMethod RegA("DomeConnector","insertInCell");

  ContainedComp::insertInCell(outerObj);
  PSet.insertAllInCell(outerObj);
  
  return;
}

void
DomeConnector::insertInCell(Simulation& System,
			 const int CN) const
  /*!
    Insert both the flange adn the ports in a cell 
    \param System :: Simulation
    \param CN :: Cell to insert
   */
{
  ELog::RegMethod RegA("DomeConnector","insertInCell");
  
  
  MonteCarlo::Object* outerObj=System.findObjectThrow(CN,"CN");
  outerObj->addIntersection(outerSurf.complement());

  ContainedComp::insertInCell(*outerObj);
  PSet.insertAllInCell(System,CN);
  
  return;
}

const portItem&
DomeConnector::getPort(const size_t index) const
  /*!
    Accessor to the portItem
    \param index :: Index of port
    \return portItem
  */
{
  ELog::RegMethod RegA("DomeConnector","getPort");

  return PSet.getPort(index);
}

void
DomeConnector::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("DomeConnector","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  createPorts(System);
  insertObjects(System);  

  
  return;
}

}  // NAMESPACE constructSystem
