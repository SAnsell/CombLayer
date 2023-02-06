/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   formax/MLMRadialSupport.cxx
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
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "Vec3D.h"
#include "Quaternion.h"
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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "MLMRadialSupport.h"


namespace xraySystem
{

MLMRadialSupport::MLMRadialSupport(const std::string& Key) :
  attachSystem::FixedRotate(Key,8),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{}



MLMRadialSupport::~MLMRadialSupport()
  /*!
    Destructor
   */
{}

void
MLMRadialSupport::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("MLMRadialSupport","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");

  topThick=Control.EvalVar<double>(keyName+"TopThick");
  topWdith=Control.EvalVar<double>(keyName+"TopWdith");

  midPlateThick=Control.EvalVar<double>(keyName+"MidPlateThick");
  midPlateExtra=Control.EvalVar<double>(keyName+"MidPlateExtra");

  bladeThick=Control.EvalVar<double>(keyName+"BladeThick");
  bladeHeight=Control.EvalVar<double>(keyName+"BladeHeight");
  bladeTopGap=Control.EvalVar<double>(keyName+"BladeTopGap");
  bladeBaseGap=Control.EvalVar<double>(keyName+"BladeBaseGap");

  sideWidth=Control.EvalVar<double>(keyName+"SideWidth");
  sideLift=Control.EvalVar<double>(keyName+"SideLift");
  sideOffset=Control.EvalVar<double>(keyName+"SideOffset");

  baseThick=Control.EvalVar<double>(keyName+"BaseThick");

  plateMat=ModelSupport::EvalMat<int>(Control,keyName+"PlateMat");
  baseMat=ModelSupport::EvalMat<int>(Control,keyName+"BaseMat");
  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat","Void");

  return;
}


void
MLMRadialSupport::createSurfaces()
  /*!
    Create planes for mirror block and support
    +x AWAY from beam
  */
{
  ELog::RegMethod RegA("MLMRadialSupport","createSurfaces");

  // top block [gapped to crystal mount]
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*topBeamWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*topOutWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin-Z*topGap,Z);
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(topGap+topHeight),Z);

  // Support plate [not gap to stuff]
  // Split
  Geometry::Vec3D pOrg(Origin-Z*(topGap+topHeight));
  // support block
  ModelSupport::buildPlane
    (SMap,buildIndex+201,pOrg-Y*(length/2.0-plateLength),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+202,pOrg+Y*(length/2.0-plateLength),Y);

  ModelSupport::buildPlane(SMap,buildIndex+203,pOrg-X*plateBeam,X);
  ModelSupport::buildPlane(SMap,buildIndex+205,pOrg-Z*plateThick,Z);


  // side supports of Radial :
  // side supports are on plane of 205:
  pOrg-=Z*plateThick;

  ModelSupport::buildPlane(SMap,buildIndex+303,pOrg-Z*sideLift,X);
  ModelSupport::buildPlane(SMap,buildIndex+304,pOrg-Z*sideBeam,X);
  ModelSupport::buildPlane(SMap,buildIndex+305,pOrg-Z*sideLift,Z);  
  

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,wheelHubRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,wheelRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,wheelOuterRadius);

  const double angleStep=(2.0*M_PI)/static_cast<double>(nSpokes);
  double angle(0.0);
  int BI(buildIndex+100);
  for(size_t i=0;i<nSpokes;i++)
    {
      const Geometry::Vec3D PX=X*std::cos(angle)+Z*std::sin(angle);
      const Geometry::Vec3D PY=X*std::sin(-angle)+Z*std::cos(angle);
      
      ModelSupport::buildPlane(SMap,BI+1,
			       Origin-PX*(spokeThick/2.0),PX);
      ModelSupport::buildPlane(SMap,BI+2,
			       Origin+PX*(spokeThick/2.0),PX);

      const Geometry::Vec3D innerPt=
	Origin+PY*(wheelHubRadius+spokeCornerRadius+microShift);
      const Geometry::Vec3D outerPt=
	Origin+PY*(wheelRadius-spokeCornerRadius-microShift);
      ModelSupport::buildCylinder
	(SMap,BI+7,innerPt-PX*(spokeCornerRadius+spokeCornerGap),
	 Y,spokeCornerRadius);
      ModelSupport::buildCylinder
	(SMap,BI+8,innerPt+PX*(spokeCornerRadius+spokeCornerGap),
	 Y,spokeCornerRadius);
      ModelSupport::buildCylinder
	(SMap,BI+9,outerPt-PX*(spokeCornerRadius+spokeCornerGap),
	 Y,spokeCornerRadius);
      ModelSupport::buildCylinder
	(SMap,BI+10,outerPt+PX*(spokeCornerRadius+spokeCornerGap),
	 Y,spokeCornerRadius);

      // divider
      ModelSupport::buildPlane(SMap,BI+3,Origin,PY);
      angle+=angleStep;
      BI+=20;
    }

  return; 
}

void
MLMRadialSupport::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("MLMRadialSupport","createObjects");

  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -6 -7");
  makeCell("Hub",System,cellIndex++,mat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -6 -27 17");
  makeCell("Rim",System,cellIndex++,mat,0.0,HR);  

  const HeadRule baseHR=
    ModelSupport::getHeadRule(SMap,buildIndex,"5 -6 7 -17");
  const HeadRule tbHR=
    ModelSupport::getHeadRule(SMap,buildIndex,"5 -6");
  int BI(buildIndex+100);
  // spokes
  HeadRule prevHR=HeadRule
    (SMap,BI+20*static_cast<int>(nSpokes-1),2);
  for(size_t i=0;i<nSpokes;i++)
    {
      HR=ModelSupport::getHeadRule(SMap,BI,"1 -2 3 7 8 9 10");
      makeCell("Spoke",System,cellIndex++,mat,0.0,HR*baseHR);  

      HR=ModelSupport::getHeadRule(SMap,BI,"1 -7");
      makeCell("SpokeCut",System,cellIndex++,voidMat,0.0,HR*tbHR);  
      HR=ModelSupport::getHeadRule(SMap,BI,"1 -9");
      makeCell("SpokeCut",System,cellIndex++,voidMat,0.0,HR*tbHR);  

      HR=ModelSupport::getHeadRule(SMap,BI,"-2 -8");
      makeCell("SpokeCut",System,cellIndex++,voidMat,0.0,HR*tbHR);  
      HR=ModelSupport::getHeadRule(SMap,BI,"-2 -10");
      makeCell("SpokeCut",System,cellIndex++,voidMat,0.0,HR*tbHR);  

      HR=HeadRule(SMap,BI,-1);
      makeCell("Void",System,cellIndex++,voidMat,0.0,HR*baseHR*prevHR);
      prevHR=HeadRule(SMap,BI,2);
      BI+=20;
    }
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -6 -27");
  addOuterSurf(HR);

  return; 
}

void
MLMRadialSupport::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("MLMRadialSupport","createLinks");

  HeadRule HR;

  return;
}

void
MLMRadialSupport::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("MLMRadialSupport","createAll");
  populate(System.getDataBase());

  createCentredUnitVector(FC,sideIndex,wheelHeight/2.0);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE xraySystem
