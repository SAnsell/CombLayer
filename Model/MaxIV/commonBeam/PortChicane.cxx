/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/PortChicane.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "SpaceCut.h"
#include "ContainedSpace.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "SurInter.h"
#include "PortChicane.h"


namespace xraySystem
{

PortChicane::PortChicane(const std::string& Key) :
  attachSystem::ContainedGroup("Main","Inner"),
  attachSystem::FixedOffset(Key,12),
  attachSystem::CellMap(),attachSystem::SurfMap(),
  attachSystem::ExternalCut()
  /*!
    Default constructor
    \param Key :: Key name for variables
  */
{}

  
void
PortChicane::populate(const FuncDataBase& Control)
  /*!
    Sets the size of the object
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("PortChicane","populate");

  FixedOffset::populate(Control);
  
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");
  clearGap=Control.EvalVar<double>(keyName+"ClearGap");
  overHang=Control.EvalVar<double>(keyName+"OverHang");

  innerSkin=Control.EvalVar<double>(keyName+"InnerSkin");
  innerPlate=Control.EvalVar<double>(keyName+"InnerPlate");

  outerSkin=Control.EvalVar<double>(keyName+"OuterSkin");
  outerPlate=Control.EvalVar<double>(keyName+"OuterPlate");
    
  baseThick=Control.EvalVar<double>(keyName+"BaseThick");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  plateMat=ModelSupport::EvalMat<int>(Control,keyName+"PlateMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  
  return;
}

void
PortChicane::createUnitVector(const attachSystem::FixedComp& FC,
				 const long int sideIndex)
  /*!
    Create the unit vectors: Note only to construct front/back surf
    \param FC :: Centre point
    \param sideIndex :: Side index
  */
{
  ELog::RegMethod RegA("PortChicane","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  
  return;
}

void
PortChicane::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("PortChicane","createSurface");

  ExternalCut::makeExpandedSurf
    (SMap,"innerWall",buildIndex+11,Origin,clearGap);
  ExternalCut::makeExpandedSurf
    (SMap,"innerWall",buildIndex+21,Origin,clearGap+innerSkin);
  ExternalCut::makeExpandedSurf
    (SMap,"innerWall",buildIndex+31,Origin,clearGap+innerSkin+innerPlate);
  ExternalCut::makeExpandedSurf
    (SMap,"innerWall",buildIndex+41,Origin,clearGap+2*innerSkin+innerPlate);

  ExternalCut::makeExpandedSurf
    (SMap,"outerWall",buildIndex+12,Origin,clearGap);
  ExternalCut::makeExpandedSurf
    (SMap,"outerWall",buildIndex+22,Origin,clearGap+outerSkin);
  ExternalCut::makeExpandedSurf
    (SMap,"outerWall",buildIndex+32,Origin,clearGap+outerSkin+outerPlate);
  ExternalCut::makeExpandedSurf
    (SMap,"outerWall",buildIndex+42,Origin,clearGap+2*outerSkin+outerPlate);

  
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);


  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(baseThick+height/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+23,
			   Origin-X*(wallThick+overHang+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,
			   Origin+X*(wallThick+overHang+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+25,
			   Origin-Z*(baseThick+overHang+height/2.0),Z);

  return;
}

void
PortChicane::createObjects(Simulation& System) 
  /*!
    Creates the colllimator block
    \param System :: Simuation for object
  */
{
  ELog::RegMethod RegA("PortChicane","createObjects");

  std::string Out;
  const std::string innerStr=getRuleStr("innerWall");
  const std::string outerStr=getRuleStr("outerWall");
	
  // inner clearance gap
  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -12 3 -4 5 -6 ");
  makeCell("Void",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-11 21 23 -24 25 -6 ");
  makeCell("InnerSkinA",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-21 31 23 -24 25 -6 ");
  makeCell("InnerPlate",System,cellIndex++,plateMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-31 41 23 -24 25 -6 ");
  makeCell("InnerSkinB",System,cellIndex++,plateMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"12 -22 23 -24 25 -6 ");
  makeCell("OuterSkinA",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"22 -32 23 -24 25 -6 ");
  makeCell("OuterPlate",System,cellIndex++,plateMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"32 -42 23 -24 25 -6 ");
  makeCell("OuterSkinB",System,cellIndex++,plateMat,0.0,Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -12 13 -3 5 -6 ");
  makeCell("LeftSide",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -12 -14 4 5 -6 ");
  makeCell("RightSide",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -12 13 -14 -5 15 ");
  makeCell("Base",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -12 23 -13 5 -6 ");
  makeCell("InnerLeftOver",System,cellIndex++,0,0.0,Out+innerStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-12 -24 14 5 -6 ");
  makeCell("InnerRightOver",System,cellIndex++,0,0.0,Out+innerStr);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"11 23 -24 25 -5 ");
  makeCell("OuterBaseOver",System,cellIndex++,wallMat,0.0,Out+outerStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,"11 23 -13 5 -6 ");
  makeCell("OuterLeftOver",System,cellIndex++,0,0.0,Out+outerStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -24 14 5 -6 ");
  makeCell("OuterRightOver",System,cellIndex++,0,0.0,Out+outerStr);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"11 23 -24 25 -5 ");
  makeCell("OuterBaseOver",System,cellIndex++,wallMat,0.0,Out+outerStr);

  // needs to be group
  Out=ModelSupport::getComposite(SMap,buildIndex,"41 -42 23 -24 25 -6 ");
  addOuterSurf("Main",Out);
  Out=ModelSupport::getComposite(SMap,buildIndex,"13 -14 15 -6 ");
  addOuterSurf("Inner",Out);
  return;
}

void
PortChicane::createLinks()
  /*!
    Construct the links for the system
   */
{
  ELog::RegMethod RegA("PortChicane","createLinks");

  // get front/back origin
  Geometry::Vec3D frontPt=
    SurInter::getLinePoint(Origin,Y,SMap.realSurfPtr(buildIndex+41),Origin);  
  Geometry::Vec3D backPt=
    SurInter::getLinePoint(Origin,Y,SMap.realSurfPtr(buildIndex+42),Origin);
  
  FixedComp::setConnect(0,frontPt,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+41));

  FixedComp::setConnect(0,backPt,-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+42));

  // outer cross
  FixedComp::setConnect(2,frontPt-X*(wallThick+overHang+width/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+23));

  FixedComp::setConnect(3,frontPt+X*(wallThick+overHang+width/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+24));

  FixedComp::setConnect(4,frontPt-Z*(baseThick+overHang+height/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+25));

  FixedComp::setConnect(5,frontPt+Z*(height/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  // inner corners
  FixedComp::setConnect(7,backPt-X*(wallThick+overHang+width/2.0),-X);
  FixedComp::setLinkSurf(7,-SMap.realSurf(buildIndex+23));

  FixedComp::setConnect(8,backPt+X*(wallThick+overHang+width/2.0),X);
  FixedComp::setLinkSurf(8,SMap.realSurf(buildIndex+24));
  
  FixedComp::setConnect(9,backPt-Z*(baseThick+overHang+height/2.0),-Z);
  FixedComp::setLinkSurf(9,-SMap.realSurf(buildIndex+25));

  FixedComp::setConnect(10,backPt+Z*(height/2.0),Z);
  FixedComp::setLinkSurf(10,SMap.realSurf(buildIndex+6));
  
  nameSideIndex(8,"innerLeft");
  nameSideIndex(9,"innerRight");
  
  return;
}

void
PortChicane::createAll(Simulation& System,
                     const attachSystem::FixedComp& FC,
                     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation 
    \param FC :: Fixed component to set axis etc
    \param sideIndex :: position of linkpoint
  */
{
  ELog::RegMethod RegA("PortChicane","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

  
}  // NAMESPACE xraySystem
