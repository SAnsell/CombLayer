/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/SimpleChicane.cxx
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
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
#include "ExternalCut.h"
#include "SpaceCut.h"
#include "ContainedSpace.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "SurInter.h"
#include "SimpleChicane.h"


namespace xraySystem
{

SimpleChicane::SimpleChicane(const std::string& Key) :
  attachSystem::ContainedGroup("Outer","Inner","Middle"),
  attachSystem::FixedOffset(Key,12),
  attachSystem::CellMap(),attachSystem::SurfMap(),
  attachSystem::ExternalCut()
  /*!
    Default constructor
    \param Key :: Key name for variables
  */
{
  nameSideIndex(2,"outerLeft");
  nameSideIndex(3,"outerRight");
  nameSideIndex(7,"innerLeft");
  nameSideIndex(8,"innerRight");
}

  
void
SimpleChicane::populate(const FuncDataBase& Control)
  /*!
    Sets the size of the object
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("SimpleChicane","populate");

  FixedOffset::populate(Control);
  
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");
  clearGap=Control.EvalVar<double>(keyName+"ClearGap");
  upStep=Control.EvalVar<double>(keyName+"UpStep");

  plateThick=Control.EvalVar<double>(keyName+"PlateThick");

  innerXWidth=Control.EvalVar<double>(keyName+"InnerXWidth");
  innerXDepth=Control.EvalVar<double>(keyName+"InnerXDepth");

  plateMat=ModelSupport::EvalMat<int>(Control,keyName+"PlateMat");
  
  return;
}

void
SimpleChicane::createUnitVector(const attachSystem::FixedComp& FC,
				 const long int sideIndex)
  /*!
    Create the unit vectors: Note only to construct front/back surf
    \param FC :: Centre point
    \param sideIndex :: Side index
  */
{
  ELog::RegMethod RegA("SimpleChicane","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  
  return;
}

void
SimpleChicane::createSurfaces()
  /*!
    Create All the surfaces
    Requires SurfMap::innerWall and SurfMap::outerWall to be set
  */
{
  ELog::RegMethod RegA("SimpleChicane","createSurface");

  
  ExternalCut::makeExpandedSurf
    (SMap,"innerWall",buildIndex+101,Origin,innerXDepth);
  ExternalCut::makeExpandedSurf
    (SMap,"outerWall",buildIndex+12,Origin,clearGap);
  ExternalCut::makeExpandedSurf
    (SMap,"outerWall",buildIndex+22,Origin,clearGap+plateThick);
  
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(upStep+height/2.0),Z);


  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(plateThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(plateThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(plateThick+height/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+103,
			   Origin-X*(innerXWidth+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,
			   Origin+X*(innerXWidth+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+105,
			   Origin-Z*(innerXWidth+height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,
			   Origin+Z*(innerXWidth+height/2.0),Z);

  return;
}

void
SimpleChicane::createObjects(Simulation& System) 
  /*!
    Creates the colllimator block
    \param System :: Simuation for object
  */
{
  ELog::RegMethod RegA("SimpleChicane","createObjects");

  std::string Out;

  const std::string innerWall=getComplementStr("innerWall");
  const std::string outerWall=getComplementStr("outerWall");

  const std::string innerOut=getRuleStr("innerWall");
  const std::string outerOut=getRuleStr("outerWall");

  ELog::EM<<"ORG == "<<Origin<<ELog::endDiag;
  
  // inner clearance gap
  Out=ModelSupport::getComposite(SMap,buildIndex,"-101 12 3 -4 5 -6 ");
  makeCell("Void",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 12 3 -4 6 -16 ");
  makeCell("TopVoid",System,cellIndex++,0,0.0,Out+outerOut);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"-12 22 3 -4 5 -16 ");
  makeCell("Plate",System,cellIndex++,plateMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 22 3 -4 -5 15 ");
  makeCell("BasePlate",System,cellIndex++,plateMat,0.0,Out+outerOut);

  Out=ModelSupport::getComposite(SMap,buildIndex," 22 13 -3 15 -16 ");
  makeCell("LeftPlate",System,cellIndex++,plateMat,0.0,Out+outerOut);

  Out=ModelSupport::getComposite(SMap,buildIndex," 22 -14 4 15 -16 ");
  makeCell("RightPlate",System,cellIndex++,plateMat,0.0,Out+outerOut);

  Out=ModelSupport::getComposite(SMap,buildIndex," 13 -14 15 -6 (-3:4:-5) ");
  makeCell("innerPlate",System,cellIndex++,
	   plateMat,0.0,Out+outerWall+innerWall);

  Out=ModelSupport::getComposite(SMap,buildIndex," -101 -5 105 103 -104 ");
  makeCell("LowXBeam",System,cellIndex++,plateMat,0.0,Out+innerOut);

  Out=ModelSupport::getComposite(SMap,buildIndex," -101 6 -106 103 -104 ");
  makeCell("TopXBeam",System,cellIndex++,plateMat,0.0,Out+innerOut);

  Out=ModelSupport::getComposite(SMap,buildIndex," -101 5 -6 103 -3 ");
  makeCell("LeftXBeam",System,cellIndex++,plateMat,0.0,Out+innerOut);

  Out=ModelSupport::getComposite(SMap,buildIndex," -101 5 -6 4 -104 ");
  makeCell("RightXBeam",System,cellIndex++,plateMat,0.0,Out+innerOut);

  // needs to be group
  Out=ModelSupport::getComposite(SMap,buildIndex," -101 103 -104 105 -106 ");
  addOuterSurf("Inner",Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 13 -14 15 -6 ");
  addOuterSurf("Middle",Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 13 -14 15 -16 22 ");
  addOuterSurf("Outer",Out);
  return;
}

void
SimpleChicane::createLinks()
  /*!
    Construct the links for the system
  */
{
  ELog::RegMethod RegA("SimpleChicane","createLinks");

  // get front/back origin
  Geometry::Vec3D frontPt=
    SurInter::getLinePoint(Origin,Y,SMap.realSurfPtr(buildIndex+101),Origin);  
  Geometry::Vec3D backPt=
    SurInter::getLinePoint(Origin,Y,SMap.realSurfPtr(buildIndex+22),Origin);
  
  FixedComp::setConnect(0,frontPt,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+11));

  FixedComp::setConnect(1,backPt,-Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+22));

  // outer points
  FixedComp::setConnect(2,backPt-X*(plateThick+width/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+13));

  FixedComp::setConnect(3,backPt+X*(plateThick+width/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+14));

  FixedComp::setConnect(4,backPt-Z*(plateThick+height/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+15));
  
  FixedComp::setConnect(5,backPt+Z*(plateThick+height/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+16));

  
  // inner cross
  FixedComp::setConnect(7,frontPt-X*(innerXWidth+width/2.0),-X);
  FixedComp::setLinkSurf(7,-SMap.realSurf(buildIndex+103));

  FixedComp::setConnect(8,frontPt+X*(innerXWidth+width/2.0),X);
  FixedComp::setLinkSurf(8,SMap.realSurf(buildIndex+104));

  FixedComp::setConnect(9,frontPt-Z*(innerXWidth+width/2.0),-Z);
  FixedComp::setLinkSurf(9,-SMap.realSurf(buildIndex+105));

  FixedComp::setConnect(10,frontPt+Z*(innerXWidth+width/2.0),Z);
  FixedComp::setLinkSurf(10,SMap.realSurf(buildIndex+106));
  
  return;
}

void
SimpleChicane::createAll(Simulation& System,
			 const attachSystem::FixedComp& FC,
			 const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation 
    \param FC :: Fixed component to set axis etc
    \param sideIndex :: position of linkpoint
  */
{
  ELog::RegMethod RegA("SimpleChicane","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

  
}  // NAMESPACE xraySystem
