/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   formax/MLMCrystal.cxx
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
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "MLMCrystal.h"


namespace xraySystem
{

MLMCrystal::MLMCrystal(const std::string& Key) :
  attachSystem::FixedRotate(Key,8),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{}

MLMCrystal::~MLMCrystal()
  /*!
    Destructor
   */
{}

void
MLMCrystal::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("MLMCrystal","populate");

  FixedRotate::populate(Control);
  
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  length=Control.EvalVar<double>(keyName+"Length");

  topSlotXStep=Control.EvalVar<double>(keyName+"TopSlotXStep");
  topSlotLength=Control.EvalVar<double>(keyName+"TopSlotLength");
  topSlotWidth=Control.EvalVar<double>(keyName+"TopSlotWidth");
  topSlotDepth=Control.EvalVar<double>(keyName+"TopSlotDepth");

  baseWidth=Control.EvalVar<double>(keyName+"BaseWidth");
  baseLength=Control.EvalVar<double>(keyName+"BaseLength");
  baseDepth=Control.EvalVar<double>(keyName+"BaseDepth");
  baseFrontHeight=Control.EvalVar<double>(keyName+"BaseFrontHeight");
  baseBackHeight=Control.EvalVar<double>(keyName+"BaseBackHeight");
  baseInnerWidth=Control.EvalVar<double>(keyName+"BaseInnerWidth");
  baseInnerBeamFaceLen=Control.EvalVar<double>(keyName+"BaseInnerBeamFaceLen");
  baseInnerOutFaceLen=Control.EvalVar<double>(keyName+"BaseInnerOutFaceLen");
  baseBackSlotLen=Control.EvalVar<double>(keyName+"BaseBackSlotLen");
  baseOutSlotLen=Control.EvalVar<double>(keyName+"BaseOutSlotLen");

  mirrorMat=ModelSupport::EvalMat<int>(Control,keyName+"MirrorMat");
  baseMat=ModelSupport::EvalMat<int>(Control,keyName+"BaseMat");
  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat","Void");


  return;
}


void
MLMCrystal::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("MLMCrystal","createSurfaces");
  
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*width,X);    
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  SurfMap::makePlane("TopSurf",SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  // top cut
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(topSlotLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(topSlotLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin+X*(topSlotXStep-topSlotWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(topSlotXStep+topSlotWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin+Z*(height/2.0-topSlotDepth),Z);

  ModelSupport::buildCylinder(SMap,buildIndex+17,
     Origin+X*topSlotXStep-Y*(topSlotLength/2.0),Z,topSlotWidth/2.0);
  ModelSupport::buildCylinder(SMap,buildIndex+18,
     Origin+X*topSlotXStep+Y*(topSlotLength/2.0),Z,topSlotWidth/2.0);

  // BASE
  const Geometry::Vec3D midPt(Origin+X*(width/2.0)-Z*(height/2.0));

  ModelSupport::buildPlane
    (SMap,buildIndex+101,midPt-Y*(baseLength/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+102,midPt+Y*(baseLength/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+103,midPt-X*(baseWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+104,midPt+X*(baseWidth/2.0),X);
  SurfMap::makePlane("BlockBase",SMap,buildIndex+105,midPt-Z*baseDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+116,midPt+Z*baseFrontHeight,Z);
  ModelSupport::buildPlane(SMap,buildIndex+126,midPt+Z*baseBackHeight,Z);

  ModelSupport::buildPlane
    (SMap,buildIndex+151,midPt-Y*(baseLength/2.0-baseInnerBeamFaceLen),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+152,midPt+Y*(baseLength/2.0-baseInnerOutFaceLen),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+153,midPt-X*(baseInnerWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+154,midPt+X*(baseInnerWidth/2.0),X);

  ModelSupport::buildPlane
    (SMap,buildIndex+203,midPt-X*(baseOutSlotLen/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+204,midPt+X*(baseOutSlotLen/2.0),X);

  ModelSupport::buildPlane
    (SMap,buildIndex+201,midPt-Y*(baseBackSlotLen/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+202,midPt+Y*(baseBackSlotLen/2.0),Y);

  return; 
}

void
MLMCrystal::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("MLMCrystal","createObjects");

  HeadRule HR;

  // slot:
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"(11:-17) (-12:-18) 13 -14 15 -6");  
  makeCell("TopSlot",System,cellIndex++,0,0.0,HR);

  // xstal
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"1 -2 3 -4 5 -6 ((-11 17):(12 18):-13:14:-15)");  
  makeCell("Xstal",System,cellIndex++,mirrorMat,0.0,HR);

  // base plate
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 103 -104 -5 105");  
  makeCell("BaseFlat",System,cellIndex++,baseMat,0.0,HR);  

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"101 -102 154 -104 5 -126 (-201:202)");  
  makeCell("BaseBack",System,cellIndex++,baseMat,0.0,HR);  
  
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"154 -104 5 -126 201 -202");  
  makeCell("BaseBackGap",System,cellIndex++,voidMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 -153 103 5 -116");  
  makeCell("BaseFront",System,cellIndex++,baseMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -151 -154 153 5 -126");  
  makeCell("BaseBeam",System,cellIndex++,baseMat,0.0,HR);  

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"152 -102 -154 153 5 -126 (-203:204)");  
  makeCell("BaseOut",System,cellIndex++,baseMat,0.0,HR);  

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"2 -102 153 5 -126 203 -204");  
  makeCell("BaseOutGap",System,cellIndex++,voidMat,0.0,HR);  
  
  // base voids:
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"151 -2 -154 153 5 -126 (-1:-3:4)");  
  makeCell("CrysVoid",System,cellIndex++,voidMat,0.0,HR);  

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"101 -102 -153 103 116 -126");  
  makeCell("FrontVoid",System,cellIndex++,voidMat,0.0,HR);  

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"101 -102 103 -104 126 -6 (-1:2:-3:4)");  
  makeCell("TopVoid",System,cellIndex++,voidMat,0.0,HR);  

  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 103 -104 105 -6");
  addOuterSurf(HR);  
  return; 
}

void
MLMCrystal::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("MLMCrystal","createLinks");

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length/2.0),-Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  FixedComp::setConnect(2,Origin,-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*width,X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(height/2.0+baseDepth),-Z);
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+105));

  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  // shifted base point but keeping normal orientation
  FixedComp::setConnect(6,Origin-Z*(height/2.0+baseDepth),Y);
  FixedComp::setLinkSurf(6,SMap.realSurf(buildIndex+105));

  HeadRule HR;

  // slot:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 103 -104");  
  setCutSurf("Surround",HR);
  
  // Points on base:

  nameSideIndex(2,"BeamCentre");
  nameSideIndex(4,"BaseRotPt");
  nameSideIndex(6,"BasePt");


  return;
}

void
MLMCrystal::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("MLMCrystal","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE xraySystem
