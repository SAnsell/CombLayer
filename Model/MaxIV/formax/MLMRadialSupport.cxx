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
  topGap=Control.EvalVar<double>(keyName+"TopGap");
  topThick=Control.EvalVar<double>(keyName+"TopThick");
  topBeamWidth=Control.EvalVar<double>(keyName+"TopBeamWidth");
  topOutWidth=Control.EvalVar<double>(keyName+"TopOutWidth");

  plateThick=Control.EvalVar<double>(keyName+"PlateThick");
  plateLength=Control.EvalVar<double>(keyName+"PlateLength");
  plateBeam=Control.EvalVar<double>(keyName+"PlateBeam");

  sideWidth=Control.EvalVar<double>(keyName+"SideWidth");
  sideLift=Control.EvalVar<double>(keyName+"SideLift");
  sideBlock=Control.EvalVar<double>(keyName+"SideBlock");
  sideBaseWidth=Control.EvalVar<double>(keyName+"SideBaseWidth");
  sideOutWidth=Control.EvalVar<double>(keyName+"SideOutWidth");
  
  bladeDrop=Control.EvalVar<double>(keyName+"BladeDrop");
  bladeThick=Control.EvalVar<double>(keyName+"BladeThick");
  bladeHeight=Control.EvalVar<double>(keyName+"BladeHeight");
  bladeTopGap=Control.EvalVar<double>(keyName+"BladeTopGap");
  bladeBaseGap=Control.EvalVar<double>(keyName+"BladeBaseGap");
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
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(topGap+topThick),Z);

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

  ModelSupport::buildPlane(SMap,buildIndex+303,
			   pOrg-X*(sideWidth/2.0+sideBlock),X);
  ModelSupport::buildPlane(SMap,buildIndex+304,
			   pOrg-X*(sideWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+313,
			   pOrg+X*(sideWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+314,
			   pOrg+X*(sideWidth/2.0+sideBlock),X);
  ModelSupport::buildPlane(SMap,buildIndex+305,pOrg-Z*sideLift,Z);  

  pOrg-=Z*sideLift;
  ModelSupport::buildPlane(SMap,buildIndex+353,
			   pOrg-X*(sideWidth/2.0),
			   pOrg-X*(sideBaseWidth/2.0),
			   pOrg-X*(sideBaseWidth/2.0)+Y,
			   X);
  ModelSupport::buildPlane(SMap,buildIndex+354,
			   pOrg-X*(sideWidth/2.0+sideBlock),
			   pOrg-X*(sideOutWidth/2.0),
			   pOrg-X*(sideOutWidth/2.0)+Y,
			   X);

  ModelSupport::buildPlane(SMap,buildIndex+363,
			   pOrg+X*(sideWidth/2.0+sideBlock),
			   pOrg+X*(sideOutWidth/2.0),
			   pOrg+X*(sideOutWidth/2.0)+Y,
			   X);
  ModelSupport::buildPlane(SMap,buildIndex+364,
			   pOrg+X*(sideWidth/2.0),
			   pOrg+X*(sideBaseWidth/2.0),
			   pOrg+X*(sideBaseWidth/2.0)+Y,
			   X);
  

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
  const HeadRule frontUnit=
    ModelSupport::getHeadRule(SMap,buildIndex,"101 -201");

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"303 -304 -205 305 ");

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
