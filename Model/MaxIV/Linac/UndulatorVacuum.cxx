/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   Linac/UndulatorVacuum.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include "FixedRotate.h"
#include "FixedOffsetUnit.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"

#include "subPipeUnit.h"
#include "UndulatorVacuum.h"

namespace tdcSystem
{

UndulatorVacuum::UndulatorVacuum(const std::string& Key) :
  attachSystem::FixedRotate(Key,12),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

UndulatorVacuum::~UndulatorVacuum() 
  /*!
    Destructor
  */
{}

void
UndulatorVacuum::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("UndulatorVacuum","populate");
  
  FixedRotate::populate(Control);

  nSegment=Control.EvalVar<size_t>(keyName+"NSegment");
  
  radius=Control.EvalVar<double>(keyName+"Radius");
  segLength=Control.EvalVar<double>(keyName+"SegLength");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  
  portOutLength=Control.EvalVar<double>(keyName+"PortOutLength");

  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");

  accessRadius=Control.EvalVar<double>(keyName+"AccessRadius");
  accessFlangeRadius=Control.EvalVar<double>(keyName+"AccessFlangeRadius");
  accessFlangeLength=Control.EvalVar<double>(keyName+"AccessFlangeLength");

  smallRadius=Control.EvalVar<double>(keyName+"SmallRadius");
  smallFlangeRadius=Control.EvalVar<double>(keyName+"SmallFlangeRadius");
  smallFlangeLength=Control.EvalVar<double>(keyName+"SmallFlangeLength");

  magGap=Control.EvalVar<double>(keyName+"MagGap");
  magRadius=Control.EvalVar<double>(keyName+"MagRadius");
  magLength=Control.EvalVar<double>(keyName+"MagLength");
  magBellowLen=Control.EvalVar<double>(keyName+"MagBellowLen");
  magFlangeRadius=Control.EvalVar<double>(keyName+"MagFlangeRadius");
  magFlangeLength=Control.EvalVar<double>(keyName+"MagFlangeLength");

  vacRadius=Control.EvalVar<double>(keyName+"VacRadius");
  vacLength=Control.EvalVar<double>(keyName+"VacLength");
  vacHeight=Control.EvalVar<double>(keyName+"VacHeight");
  vacFlangeRadius=Control.EvalVar<double>(keyName+"VacFlangeRadius");
  vacFlangeLength=Control.EvalVar<double>(keyName+"VacFlangeLength");

  preLength=Control.EvalVar<double>(keyName+"PreLength");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  
  return;
}

void
UndulatorVacuum::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("UndulatorVacuum","createSurfaces");

  const double length
    (segLength*static_cast<double>(nSegment)+preLength*2.0);

  // full height
  const double height(2.0*(radius+wallThick+magLength+magBellowLen));
  const double width(2.0*(radius+wallThick+portOutLength));

  // Inner void
  if (!ExternalCut::isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);    
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  makeShiftedSurf(SMap,"front",buildIndex+11,Y,wallThick);
  
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(0.7*radius),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(0.7*radius),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

   
  if (!ExternalCut::isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);    
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+1));
    }
  makeShiftedSurf(SMap,"back",buildIndex+12,Y,-wallThick);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+wallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,flangeRadius);

  int BI(buildIndex);
  for(size_t i=0;i<nSegment;i++)
    {
      BI+=100;
      const Geometry::Vec3D segOrg
	(Origin+Y*(static_cast<double>(i)*segLength+preLength));
      ModelSupport::buildPlane(SMap,BI+1,segOrg,Y);
      ModelSupport::buildPlane(SMap,BI+2,segOrg+Y*segLength,Y);    
      // flange
      ModelSupport::buildPlane(SMap,BI+11,segOrg-Y*flangeLength,Y);
      ModelSupport::buildPlane(SMap,BI+12,segOrg+Y*(segLength-flangeLength),Y);

    }
  
  return;
}

void
UndulatorVacuum::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("UndulatorVacuum","createObjects");

  std::string Out;
  const std::string frontStr=getRuleStr("front");
  const std::string backStr=getRuleStr("back");
  
  // front/back pre object:
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 11 -12 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex," -11 -17 ");
  makeCell("frontWall",System,cellIndex++,wallMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 12 -17 ");
  makeCell("backWall",System,cellIndex++,wallMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 7 -17 -101");
  makeCell("frontUnit",System,cellIndex++,wallMat,0.0,Out);

  int BI(buildIndex);

  for(size_t i=0;i<nSegment;i++)
    {
      BI+=100;
      const std::string sNum(std::to_string(i));

      Out=ModelSupport::getComposite(SMap,BI,buildIndex," 1 -2 7M -17M ");
      makeCell("Unit"+sNum,System,cellIndex++,wallMat,0.0,Out);

      Out=ModelSupport::getComposite(SMap,BI,buildIndex," 1 -11 17M -27M ");
      makeCell("FFlange"+sNum,System,cellIndex++,wallMat,0.0,Out);

      Out=ModelSupport::getComposite(SMap,BI,buildIndex," -2 12 17M -27M ");
      makeCell("BFlange"+sNum,System,cellIndex++,wallMat,0.0,Out);

      Out=ModelSupport::getComposite(SMap,BI,buildIndex," -11 12 17M -27M ");
      makeCell("OuterVoid"+sNum,System,cellIndex++,0,0.0,Out);

      Out=ModelSupport::getComposite
	(SMap,BI,buildIndex," 1 -2 3M -4M 5M -6M -27M ");
      makeCell("BoxVoid"+sNum,System,cellIndex++,0,0.0,Out);
    }

  
  Out=ModelSupport::getComposite(SMap,buildIndex,BI," 2M 7 -17 -12");
  makeCell("frontUnit",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 7 -17 -101");
  makeCell("frontUnit",System,cellIndex++,wallMat,0.0,Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,BI," 2M  7 -17 -12 ");
  makeCell("backUnit",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,BI," 3 -4 5 -6");
  addOuterSurf(Out);
  
  return;
}

void
UndulatorVacuum::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("UndulatorVacuum","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,-Y);
  ExternalCut::createLink("back",*this,1,Origin,Y);
  // Note outer links done in 

  return;
}
    
void
UndulatorVacuum::createAll(Simulation& System,
			   const attachSystem::FixedComp& FC,
			   const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("UndulatorVacuum","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
