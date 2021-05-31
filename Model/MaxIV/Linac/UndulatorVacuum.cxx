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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "objectRegister.h"
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
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"

#include "FMUndulator.h"
#include "UndulatorVacuum.h"

namespace tdcSystem
{

UndulatorVacuum::UndulatorVacuum(const std::string& Key) :
  attachSystem::FixedRotate(Key,12),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  undulator(new xraySystem::FMUndulator(keyName+"Undulator"))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(undulator);
}

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
  portRadius=Control.EvalVar<double>(keyName+"PortRadius");

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
  magSupportRadius=Control.EvalVar<double>(keyName+"MagSupportRadius");
  
  vacRadius=Control.EvalVar<double>(keyName+"VacRadius");
  vacLength=Control.EvalVar<double>(keyName+"VacLength");
  vacHeight=Control.EvalVar<double>(keyName+"VacHeight");
  vacFlangeRadius=Control.EvalVar<double>(keyName+"VacFlangeRadius");
  vacFlangeLength=Control.EvalVar<double>(keyName+"VacFlangeLength");

  preLength=Control.EvalVar<double>(keyName+"PreLength");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  supportMat=ModelSupport::EvalMat<int>(Control,keyName+"SupportMat");
  
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
  const double wallRadius(radius+wallThick);
  const double height(2.0*(wallRadius+magLength+magBellowLen));
  const double width(2.0*(wallRadius+portOutLength));

  // Inner void
  if (!ExternalCut::isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);    
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  makeShiftedSurf(SMap,"front",buildIndex+11,Y,wallThick);

  // mid plane divider
  ModelSupport::buildPlane(SMap,buildIndex+10,Origin,X);
  ModelSupport::buildPlane(SMap,buildIndex+20,Origin,Z);
  
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(0.8*radius),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(0.8*radius),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  // ACCESS::
  ModelSupport::buildPlane
    (SMap,buildIndex+53,Origin-X*(wallRadius+portOutLength-accessFlangeLength),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+54,Origin+X*(wallRadius+portOutLength-accessFlangeLength),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+63,Origin-X*(wallRadius+portOutLength-2.0*accessFlangeLength),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+64,Origin+X*(wallRadius+portOutLength-2.0*accessFlangeLength),X);
  // SMALL::
  ModelSupport::buildPlane
    (SMap,buildIndex+153,Origin-X*(wallRadius+portOutLength-smallFlangeLength),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+154,Origin+X*(wallRadius+portOutLength-smallFlangeLength),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+163,Origin-X*(wallRadius+portOutLength-2.0*smallFlangeLength),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+164,Origin+X*(wallRadius+portOutLength-2.0*smallFlangeLength),X);
  // MagLift::
  ModelSupport::buildPlane
    (SMap,buildIndex+205,Origin-Z*(wallRadius+magLength),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+215,Origin-Z*(wallRadius+magLength-magFlangeLength),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+225,Origin-Z*(wallRadius+magLength+magFlangeLength),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+206,Origin+Z*(wallRadius+magLength),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+216,Origin+Z*(wallRadius+magLength-magFlangeLength),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+226,Origin+Z*(wallRadius+magLength+magFlangeLength),Z);

   
  if (!ExternalCut::isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);    
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }
  makeShiftedSurf(SMap,"back",buildIndex+12,Y,-wallThick);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+wallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,flangeRadius);

  ModelSupport::buildCylinder(SMap,buildIndex+37,Origin,Y,portRadius);

  int BI(buildIndex);
  for(size_t i=0;i<nSegment;i++)
    {
      // MAIN TUBE
      BI+=2000;
      const Geometry::Vec3D segOrg
	(Origin+Y*(static_cast<double>(i)*segLength+preLength));
      ModelSupport::buildPlane(SMap,BI+1,segOrg,Y);
      ModelSupport::buildPlane(SMap,BI+2,segOrg+Y*segLength,Y);    
      // flange
      ModelSupport::buildPlane(SMap,BI+11,segOrg+Y*flangeLength,Y);
      ModelSupport::buildPlane(SMap,BI+12,segOrg+Y*(segLength-flangeLength),Y);

      // HORIZONTAL PORTS (1/5 separation):
      Geometry::Vec3D POrg(segOrg+Y*(segLength/6.0));

      // Main view
      int PI(BI+100);
      for(size_t i=0;i<3;i++)
	{
	  ModelSupport::buildCylinder(SMap,PI+7,POrg,X,accessRadius);
	  ModelSupport::buildCylinder(SMap,PI+17,POrg,X,accessRadius+wallThick);
	  ModelSupport::buildCylinder(SMap,PI+27,POrg,X,accessFlangeRadius);
	  POrg+=Y*(2.0*segLength/6.0);
	  PI+=100;
	}
      // SMALL PIPES
      POrg=segOrg+Y*(2.0*segLength/6.0);
      for(size_t i=0;i<2;i++)
	{
	  ModelSupport::buildCylinder(SMap,PI+7,POrg,X,smallRadius);
	  ModelSupport::buildCylinder(SMap,PI+17,POrg,X,smallRadius+wallThick);
	  ModelSupport::buildCylinder(SMap,PI+27,POrg,X,smallFlangeRadius);
	  POrg+=Y*(2.0*segLength/6.0);
	  PI+=100;
	}
      // Up/Down [1/12 + 5 * 1/6]
      POrg=segOrg+Y*(segLength/12.0);
      for(size_t i=0;i<6;i++)
	{
	  ModelSupport::buildCylinder(SMap,PI+7,POrg-X*(magGap/2.0),Z,magRadius);
	  ModelSupport::buildCylinder(SMap,PI+17,POrg-X*(magGap/2.0),Z,magRadius+wallThick);
	  ModelSupport::buildCylinder(SMap,PI+27,POrg-X*(magGap/2.0),Z,magFlangeRadius);
	  ModelSupport::buildCylinder(SMap,PI+8,POrg+X*(magGap/2.0),Z,magRadius);
	  ModelSupport::buildCylinder(SMap,PI+18,POrg+X*(magGap/2.0),Z,magRadius+wallThick);
	  ModelSupport::buildCylinder(SMap,PI+28,POrg+X*(magGap/2.0),Z,magFlangeRadius);

	  ModelSupport::buildCylinder(SMap,PI+57,POrg-X*(magGap/2.0),Z,magSupportRadius);
	  ModelSupport::buildCylinder(SMap,PI+58,POrg+X*(magGap/2.0),Z,magSupportRadius);
	  
	  POrg+=Y*(segLength/6.0);
	  PI+=100;
	}
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
  
  Out=ModelSupport::getComposite(SMap,buildIndex," -11 -17 37 ");
  makeCell("frontFace",System,cellIndex++,wallMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," -11 -37 ");
  makeCell("frontPort",System,cellIndex++,voidMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 7 -17 -2001");
  makeCell("frontUnit",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -2001 3 -4 5 -6 17 ");
  makeCell("frontOuter",System,cellIndex++,0,0.0,Out+frontStr);

  int BI(buildIndex);
	
  for(size_t i=0;i<nSegment;i++)
    {
      BI+=2000;
      const std::string sNum(std::to_string(i));
      
      // MAIN TUBE AND FLANGES:
      Out=ModelSupport::getComposite(SMap,BI,buildIndex," 1 -2 7M -17M 107 207 307 407 507");
      makeCell("Unit"+sNum,System,cellIndex++,wallMat,0.0,Out);

      Out=ModelSupport::getComposite(SMap,BI,buildIndex," 1 -11 17M -27M ");
      makeCell("FFlange"+sNum,System,cellIndex++,wallMat,0.0,Out);

      Out=ModelSupport::getComposite(SMap,BI,buildIndex," -2 12 17M -27M ");
      makeCell("BFlange"+sNum,System,cellIndex++,wallMat,0.0,Out);

      Out=ModelSupport::getComposite(SMap,BI,buildIndex,
				     " 11 -12 17M -27M 127 227 327 427 527");
      makeCell("OuterVoid"+sNum,System,cellIndex++,0,0.0,Out);

      HeadRule outerAcc;
      int PI(BI+100);
      for(size_t j=0;j<3;j++)
	{
	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," 53M -54M -7 7M");
	  makeCell("A"+sNum+"Void",System,cellIndex++,voidMat,0.0,Out);

	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," -10M 53M 7 -17 17M");
	  makeCell("AL"+sNum+"Wall",System,cellIndex++,wallMat,0.0,Out);

	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," -63M 53M -27 17 ");
	  makeCell("AL"+sNum+"Flange",System,cellIndex++,wallMat,0.0,Out);
	  
	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," 3M -27 -53M ");
	  makeCell("AL"+sNum+"Plate",System,cellIndex++,wallMat,0.0,Out);

	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," 63M 17 -27 -10M 17M");
	  makeCell("AL"+sNum+"Outer",System,cellIndex++,0,0.0,Out);

	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," 10M -54M 7 -17 17M");
	  makeCell("AR"+sNum+"Wall",System,cellIndex++,wallMat,0.0,Out);

	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," 64M -54M -27 17 ");
	  makeCell("AR"+sNum+"Flange",System,cellIndex++,wallMat,0.0,Out);
	  
	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," -4M -27 54M ");
	  makeCell("AR"+sNum+"Plate",System,cellIndex++,wallMat,0.0,Out);

	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," -64M 17 -27 10M 17M");
	  makeCell("AR"+sNum+"Outer",System,cellIndex++,0,0.0,Out);

	  outerAcc.addIntersection(SMap.realSurf(PI+27));
	  PI+=100;
	}

      // SMALL pipes:
      for(size_t j=0;j<2;j++)
	{
	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," 153M -154M -7 7M");
	  makeCell("S"+sNum+"Void",System,cellIndex++,voidMat,0.0,Out);

	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," -10M 153M 7 -17 17M");
	  makeCell("SL"+sNum+"Wall",System,cellIndex++,wallMat,0.0,Out);

	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," -163M 153M -27 17 ");
	  makeCell("SL"+sNum+"Flange",System,cellIndex++,wallMat,0.0,Out);
	  
	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," 3M -27 -153M ");
	  makeCell("SL"+sNum+"Plate",System,cellIndex++,wallMat,0.0,Out);

	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," 163M 17 -27 -10M 17M");
	  makeCell("SL"+sNum+"Outer",System,cellIndex++,0,0.0,Out);

	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," 10M -154M 7 -17 17M");
	  makeCell("SR"+sNum+"Wall",System,cellIndex++,wallMat,0.0,Out);

	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," 164M -154M -27 17 ");
	  makeCell("SR"+sNum+"Flange",System,cellIndex++,wallMat,0.0,Out);
	  
	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," -4M -27 154M ");
	  makeCell("SR"+sNum+"Plate",System,cellIndex++,wallMat,0.0,Out);
	  
	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," -164M 17 -27 10M 17M");
	  makeCell("SR"+sNum+"Outer",System,cellIndex++,0,0.0,Out);


	  outerAcc.addIntersection(SMap.realSurf(PI+27));
	  PI+=100;
	}
      HeadRule outerMag;
      HeadRule outerMagVoid;
      for(size_t j=0;j<6;j++)
	{
	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," 5M -20M  -7 7M");
	  makeCell("LowLM"+sNum+"Void",System,cellIndex++,voidMat,0.0,Out);
	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," 5M -20M  -8 7M");
	  makeCell("LowRM"+sNum+"Void",System,cellIndex++,voidMat,0.0,Out);

	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," 5M -20M  -17 7 17M");
	  makeCell("LowLM"+sNum+"Wall",System,cellIndex++,wallMat,0.0,Out);
	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," 5M -20M  -18 8 17M");
	  makeCell("LowRM"+sNum+"Wall",System,cellIndex++,wallMat,0.0,Out);

	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," 225M -215M  -27 17 ");
	  makeCell("LowLM"+sNum+"Flange",System,cellIndex++,wallMat,0.0,Out);
	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," 225M -215M  -28 18 ");
	  makeCell("LowRM"+sNum+"Flange",System,cellIndex++,wallMat,0.0,Out);

	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," 215M  -27 17 -20M 17M");
	  makeCell("LowLM"+sNum+"Outer",System,cellIndex++,0,0.0,Out);
	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," 215M  -28 18 -20M 17M");
	  makeCell("LowRM"+sNum+"Outer",System,cellIndex++,0,0.0,Out);

	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," -225M  -27 17 5M");
	  makeCell("LowLM"+sNum+"Outer",System,cellIndex++,0,0.0,Out);
	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," -225M  -28 18 5M");
	  makeCell("LowRM"+sNum+"Outer",System,cellIndex++,0,0.0,Out);
	  
	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," -6M 20M  -7 7M");
	  makeCell("TopLM"+sNum+"Void",System,cellIndex++,voidMat,0.0,Out);
	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," -6M 20M  -8 7M");
	  makeCell("TopRM"+sNum+"Void",System,cellIndex++,voidMat,0.0,Out);

	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," -6M 20M  -17 7 17M");
	  makeCell("TopLM"+sNum+"Wall",System,cellIndex++,wallMat,0.0,Out);
	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," -6M 20M  -18 8 17M");
	  makeCell("TopRM"+sNum+"Wall",System,cellIndex++,wallMat,0.0,Out);

	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," -226M 216M  -27 17 ");
	  makeCell("TopLM"+sNum+"Flange",System,cellIndex++,wallMat,0.0,Out);
	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," -226M 216M  -28 18 ");
	  makeCell("TopRM"+sNum+"Flange",System,cellIndex++,wallMat,0.0,Out);

	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," -216M  -27 17 20M 17M");
	  makeCell("TopLM"+sNum+"Outer",System,cellIndex++,0,0.0,Out);
	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," -216M  -28 18 20M 17M");
	  makeCell("TopRM"+sNum+"Outer",System,cellIndex++,0,0.0,Out);

	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," 226M  -27 17 -6M");
	  makeCell("TopLM"+sNum+"Outer",System,cellIndex++,0,0.0,Out);
	  Out=ModelSupport::getComposite(SMap,PI,buildIndex," 226M  -28 18 -6M");
	  makeCell("TopRM"+sNum+"Outer",System,cellIndex++,0,0.0,Out);


	  outerMag.addIntersection(SMap.realSurf(PI+27));
	  outerMag.addIntersection(SMap.realSurf(PI+28));

	  outerMagVoid.addIntersection(SMap.realSurf(PI+7));
	  outerMagVoid.addIntersection(SMap.realSurf(PI+8));
	  PI+=100;
        }      
      // exclude magnet from outer
      CellMap::insertComponent(System,"OuterVoid"+sNum,outerMag);

      // exclude magnet from outer
      CellMap::insertComponent(System,"Unit"+sNum,outerMagVoid);
      
      // OUTER:
      Out=ModelSupport::getComposite
	(SMap,BI,buildIndex," 1 -2 3M -4M 5M -6M 27M (-13M:14M)");
      makeCell("BoxVoid"+sNum,System,cellIndex++,0,0.0,Out+outerAcc.display());

      Out=ModelSupport::getComposite
	(SMap,BI,buildIndex," 1 -2 13M -14M 5M -20M 27M ");
      makeCell("BoxVoid"+sNum,System,cellIndex++,0,0.0,Out+outerMag.display());

      Out=ModelSupport::getComposite
	(SMap,BI,buildIndex," 1 -2 13M -14M -6M 20M 27M ");
      makeCell("BoxVoid"+sNum,System,cellIndex++,0,0.0,Out+outerMag.display());

    }  
  
  Out=ModelSupport::getComposite(SMap,buildIndex," 12 -17 37 ");
  makeCell("backFace",System,cellIndex++,wallMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 12 -37 ");
  makeCell("backPort",System,cellIndex++,voidMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,BI," 2M 7 -17 -12");
  makeCell("backUnit",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,BI," 2M 3 -4 5 -6 17 ");
  makeCell("backOuter",System,cellIndex++,0,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,BI," 3 -4 5 -6");
  addOuterSurf(Out+frontStr+backStr);
  
  return;
}

void
UndulatorVacuum::createSupport(Simulation& System)
  /*!
    Create the support rods
    \param System :: Simulation
   */
{
  ELog::RegMethod RegA("UndulatorVacuum","createSupport");

  std::string Out;
  const HeadRule topHR=undulator->getFullRule("top");
  const HeadRule baseHR=undulator->getFullRule("base");

  for(size_t i=0;i<nSegment;i++)
    {
      const std::string sNum(std::to_string(i));
      int PI(buildIndex+600+(static_cast<int>(i)+1)*2000);
      for(size_t j=0;j<6;j++)
	{
	  Out=ModelSupport::getComposite(SMap,buildIndex,PI," 5 -57M ");
	  makeCell("BTube",System,cellIndex++,supportMat,0.0,Out+baseHR.display());
	  Out=ModelSupport::getComposite(SMap,buildIndex,PI," -6 -57M ");
	  makeCell("TTube",System,cellIndex++,supportMat,0.0,Out+topHR.display());

	  Out=ModelSupport::getComposite(SMap,buildIndex,PI," 5 -58M ");
	  makeCell("BTube",System,cellIndex++,supportMat,0.0,Out+baseHR.display());
	  Out=ModelSupport::getComposite(SMap,buildIndex,PI," -6 -58M ");
	  makeCell("TTube",System,cellIndex++,supportMat,0.0,Out+topHR.display());

	  Out=ModelSupport::getComposite(SMap,PI," 57 ");
	  CellMap::insertComponent(System,"LowLM"+sNum+"Void",j,Out);
	  CellMap::insertComponent(System,"TopLM"+sNum+"Void",j,Out);
	  CellMap::insertComponent(System,"Void",Out);
	  
	  Out=ModelSupport::getComposite(SMap,PI," 58 ");
	  CellMap::insertComponent(System,"LowRM"+sNum+"Void",j,Out);
	  CellMap::insertComponent(System,"TopRM"+sNum+"Void",j,Out);
	  CellMap::insertComponent(System,"Void",Out);
	  PI+=100;
	}
    }
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

  const double length
    (segLength*static_cast<double>(nSegment)+preLength*2.0);

  ExternalCut::createLink("front",*this,0,Origin,-Y);
  ExternalCut::createLink("back",*this,1,Origin,Y);
  // Note outer links done in 

  FixedComp::setConnect(6,Origin+Y*(length/2.0),Y);
  FixedComp::nameSideIndex(6,"centre");
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

  // note undulator is a centre based system
  undulator->addInsertCell(CellMap::getCell("Void"));

  undulator->createAll(System,*this,"centre");
  createSupport(System);
  
  return;
}
  
}  // NAMESPACE xraySystem
