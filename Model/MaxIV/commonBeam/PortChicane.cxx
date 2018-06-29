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
#include "ContainedSpace.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "PortChicane.h"


namespace xraySystem
{

PortChicane::PortChicane(const std::string& Key) :
  attachSystem::ContainedSpace(),
  attachSystem::FixedOffset(Key,2),
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

  ELog::EM<<"Surf "<<*SMap.realSurfPtr(buildIndex+11)<<ELog::endDiag;
  ELog::EM<<"Surf "<<*SMap.realSurfPtr(buildIndex+21)<<ELog::endDiag;
  ELog::EM<<"Surf "<<*SMap.realSurfPtr(buildIndex+31)<<ELog::endDiag;
  ELog::EM<<"Surf "<<*SMap.realSurfPtr(buildIndex+41)<<ELog::endDiag;

  ELog::EM<<"Surf "<<*SMap.realSurfPtr(buildIndex+12)<<ELog::endDiag;
  ELog::EM<<"Surf "<<*SMap.realSurfPtr(buildIndex+22)<<ELog::endDiag;
  ELog::EM<<"Surf "<<*SMap.realSurfPtr(buildIndex+32)<<ELog::endDiag;
  ELog::EM<<"Surf "<<*SMap.realSurfPtr(buildIndex+42)<<ELog::endDiag;
	
  // inner clearance gap
  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -12 3 -4 5 -6 ");
  makeCell("Void",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-11 21 13 -14 15 -6 ");
  makeCell("InnerSkinA",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-21 31 13 -14 15 -6 ");
  makeCell("InnerPlate",System,cellIndex++,plateMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-31 41 13 -14 15 -6 ");
  makeCell("InnerSkinB",System,cellIndex++,plateMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"12 -22 13 -14 15 -6 ");
  makeCell("OuterSkinA",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"22 -32 13 -14 15 -6 ");
  makeCell("OuterPlate",System,cellIndex++,plateMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"32 -42 13 -14 15 -6 ");
  makeCell("OuterSkinB",System,cellIndex++,plateMat,0.0,Out);

  
  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -12 13 -3 5 -6 ");
  makeCell("LeftSide",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -12 -14 4 5 -6 ");
  makeCell("RightSide",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -12 13 -14 -5 15 ");
  makeCell("Base",System,cellIndex++,wallMat,0.0,Out);

  // needs to be group
  Out=ModelSupport::getComposite(SMap,buildIndex,"41 -42 13 -14 15 -6 ");
  addOuterSurf(Out);
  return;
}

void
PortChicane::createLinks()
  /*!
    Construct the links for the system
   */
{
  ELog::RegMethod RegA("PortChicane","createLinks");

  // FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  // FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  // FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
  // FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));      
  
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
