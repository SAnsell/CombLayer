/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   formax/MLMRadialSupport.cxx
 *
 * Copyright (c) 2004-2025 by Stuart Ansell
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
  sideFullWidth=Control.EvalVar<double>(keyName+"SideFullWidth");

  supportHeight=Control.EvalVar<double>(keyName+"SupportHeight");
  supportOuterHeight=Control.EvalVar<double>(keyName+"SupportOuterHeight");
  supportInnerLift=Control.EvalVar<double>(keyName+"SupportInnerLift");
  
  bladeThick=Control.EvalVar<double>(keyName+"BladeThick");
  bladeTopGap=Control.EvalVar<double>(keyName+"BladeTopGap");
  bladeBaseGap=Control.EvalVar<double>(keyName+"BladeBaseGap");

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
  Geometry::Vec3D pOrg(Origin-Z*(topGap+topThick));
  // support block
  ModelSupport::buildPlane
    (SMap,buildIndex+201,pOrg-Y*(length/2.0-plateLength),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+202,pOrg+Y*(length/2.0-plateLength),Y);

  ModelSupport::buildPlane(SMap,buildIndex+203,pOrg-X*plateBeam,X);
  ModelSupport::buildPlane(SMap,buildIndex+205,pOrg-Z*plateThick,Z);


  // side supports of Radial : supports are on plane of 205:
  pOrg+=X*(plateBeam/2.0)-Z*plateThick;
  ModelSupport::buildPlane(SMap,buildIndex+303,
			   pOrg-X*(sideWidth/2.0+sideBlock),X);
  ModelSupport::buildPlane(SMap,buildIndex+304,
			   pOrg-X*(sideWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+313,
			   pOrg+X*(sideWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+314,
			   pOrg+X*(sideWidth/2.0+sideBlock),X);
  ModelSupport::buildPlane(SMap,buildIndex+305,pOrg-Z*sideLift,Z);  

  
  ModelSupport::buildPlane
    (SMap,buildIndex+403,pOrg-X*(sideFullWidth/2.0),X);  
  ModelSupport::buildPlane
    (SMap,buildIndex+404,pOrg+X*(sideFullWidth/2.0),X);  


  ModelSupport::buildPlane
    (SMap,buildIndex+506,pOrg-Z*(supportHeight-supportOuterHeight),Z);  
  ModelSupport::buildPlane(SMap,buildIndex+505,pOrg-Z*supportHeight,Z);


  const double h=supportInnerLift;
  const double S=sideFullWidth/2.0;
  const double R=(h*h+S*S)/(2.0*h);

  const Geometry::Vec3D cylOrg=pOrg-Z*(R+supportHeight-supportOuterHeight);
      
  ModelSupport::buildCylinder(SMap,buildIndex+507,cylOrg,Y,R);  

  pOrg-=Z*sideLift;
  const double supZDist(supportHeight-supportInnerLift);

  ModelSupport::buildPlane(SMap,buildIndex+353,
			   pOrg-X*(sideWidth/2.0+sideBlock),
			   pOrg-X*(sideWidth/2.0+sideBlock)+Y,
			   pOrg-X*(sideOutWidth/2.0+sideBlock)-Z*supZDist,
			   X);
  ModelSupport::buildPlane(SMap,buildIndex+354,
			   pOrg-X*(sideWidth/2.0),
			   pOrg-X*(sideWidth/2.0)+Y,
			   pOrg-X*(sideOutWidth/2.0)-Z*supZDist,
			   X);
  ModelSupport::buildPlane(SMap,buildIndex+363,
			   pOrg+X*(sideWidth/2.0),
			   pOrg+X*(sideWidth/2.0)+Y,
			   pOrg+X*(sideOutWidth/2.0)-Z*supZDist,
			   X);
  ModelSupport::buildPlane(SMap,buildIndex+364,
			   pOrg+X*(sideWidth/2.0+sideBlock),
			   pOrg+X*(sideWidth/2.0+sideBlock)+Y,
			   pOrg+X*(sideOutWidth/2.0+sideBlock)-Z*supZDist,
			   X);
  

  // BLADES:
  Geometry::Vec3D topPt(pOrg-X*(2.0*bladeTopGap));
  Geometry::Vec3D basePt(pOrg-X*(2.0*bladeBaseGap)-Z*supZDist);
  int BI=buildIndex+1000;
  for(size_t i=0;i<5;i++)
    {
      ModelSupport::buildPlane(SMap,BI+3,
			       topPt-X*(bladeThick/2.0),
			       basePt-X*(bladeThick/2.0),
			       basePt-X*(bladeThick/2.0)+Y,
			       X);
      ModelSupport::buildPlane(SMap,BI+4,
			       topPt+X*(bladeThick/2.0),
			       basePt+X*(bladeThick/2.0),
			       basePt+X*(bladeThick/2.0)+Y,
			       X);
      topPt+=X*bladeTopGap;
      basePt+=X*bladeBaseGap;
      BI+=10;
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

  // Top plate
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 103 -104 105 -106");
  makeCell("TopPlate",System,cellIndex++,plateMat,0.0,HR);    
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 403 -404  (-103 : 104) 105 -106");
  makeCell("TopPlateVoid",System,cellIndex++,voidMat,0.0,HR);    
  
  const HeadRule frontHR=
    ModelSupport::getHeadRule(SMap,buildIndex,"101 -201");
  const HeadRule backHR=
    ModelSupport::getHeadRule(SMap,buildIndex,"202 -102");

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"203 -104 -105 205");
  makeCell("FrontPlate",System,cellIndex++,baseMat,0.0,HR*frontHR);  
  makeCell("BackPlate",System,cellIndex++,baseMat,0.0,HR*backHR);  

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"403 -404 (-203 : 104) 205 -105");
  makeCell("MidPlateVoid",System,cellIndex++,voidMat,0.0,HR*frontHR);
  makeCell("MidPlateVoid",System,cellIndex++,voidMat,0.0,HR*backHR);
  // mid void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"201 -202 403 -404 -105 505");
  makeCell("MidVoid",System,cellIndex++,voidMat,0.0,HR);  

  // side supports:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"303 -304 -205 305");
  makeCell("fBlock",System,cellIndex++,baseMat,0.0,HR*frontHR);
  makeCell("bBlock",System,cellIndex++,baseMat,0.0,HR*backHR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"313 -314 -205 305");
  makeCell("fBlock",System,cellIndex++,baseMat,0.0,HR*frontHR);  
  makeCell("bBlock",System,cellIndex++,baseMat,0.0,HR*backHR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"353 -354 -305 505");
  makeCell("fAngle",System,cellIndex++,baseMat,0.0,HR*frontHR);  
  makeCell("bBlock",System,cellIndex++,baseMat,0.0,HR*backHR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"363 -364 -305 505");
  makeCell("fAngle",System,cellIndex++,baseMat,0.0,HR*frontHR);  
  makeCell("bAngle",System,cellIndex++,baseMat,0.0,HR*backHR);
  
  // base connector:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-353 403 505 -506");
  makeCell("fConnector",System,cellIndex++,baseMat,0.0,HR*frontHR);
  makeCell("bConnector",System,cellIndex++,baseMat,0.0,HR*backHR);  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"364 -404 505 -506");
  makeCell("fConnector",System,cellIndex++,baseMat,0.0,HR*frontHR);
  makeCell("bConnector",System,cellIndex++,baseMat,0.0,HR*backHR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-363 354 505 -506 -507");
  makeCell("fConnector",System,cellIndex++,baseMat,0.0,HR*frontHR);
  makeCell("bConnector",System,cellIndex++,baseMat,0.0,HR*backHR);  

  
  // edge void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"364 314 -404 -205 506");
  makeCell("fEdgeVoid",System,cellIndex++,voidMat,0.0,HR*frontHR);
  makeCell("bEdgeVoid",System,cellIndex++,voidMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-353 -303 403 -205 506");
  makeCell("fEdgeVoid",System,cellIndex++,voidMat,0.0,HR*frontHR);
  makeCell("bEdgeVoid",System,cellIndex++,voidMat,0.0,HR*backHR);

  // BLADES
  HeadRule sideHR=
      ModelSupport::getHeadRule(SMap,buildIndex,"354:304");
  int BI=buildIndex+1000;
  for(size_t i=0;i<5;i++)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,BI,"505 507 -205 -3M");
      makeCell("fBladeVoid",System,cellIndex++,voidMat,0.0,HR*frontHR*sideHR);
      makeCell("bBladeVoid",System,cellIndex++,voidMat,0.0,HR*backHR*sideHR);
      HR=ModelSupport::getHeadRule(SMap,buildIndex,BI,"505 507 -205 3M -4M ");
      makeCell("fBlade",System,cellIndex++,plateMat,0.0,HR*frontHR);
      makeCell("bBlade",System,cellIndex++,plateMat,0.0,HR*backHR);
      sideHR=HeadRule(SMap,BI,4);
      BI+=10;
    }
  sideHR=ModelSupport::getHeadRule(SMap,buildIndex,"-363:-313");
  HR=ModelSupport::getHeadRule(SMap,buildIndex,BI-10,"505 507 -205 4M");
  makeCell("fBladeVoid",System,cellIndex++,voidMat,0.0,HR*frontHR*sideHR);
  makeCell("bBladeVoid",System,cellIndex++,voidMat,0.0,HR*backHR*sideHR);

  

  // VOID VOLUMES : OUTER
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 403 -404 505 -106");  
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

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE xraySystem
