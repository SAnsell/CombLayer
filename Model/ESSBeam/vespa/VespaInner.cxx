/********************************************************************** 
  CombLayer : MCNP(X) Input builder
 
 * File:   vespa/VespaInner.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedGroup.h"
#include "FixedRotateGroup.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"

#include "VespaInner.h"

namespace essSystem
{

VespaInner::VespaInner(const std::string& Key) : 
  attachSystem::FixedRotateGroup(Key,"Inner",6,"Mid",6,"Outer",6),
  attachSystem::ContainedComp(),attachSystem::FrontBackCut(),
  attachSystem::CellMap(),attachSystem::SurfMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

VespaInner::~VespaInner() 
  /*!
    Destructor
  */
{}

void
VespaInner::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("VespaInner","populate");
  
  FixedRotateGroup::populate(Control);

  backAngle=Control.EvalVar<double>(keyName+"BackAngle");
  backCutStep=Control.EvalVar<double>(keyName+"BackCutStep");
  
  // Void + Fe special:
  voidHeight=Control.EvalVar<double>(keyName+"VoidHeight");
  voidWidth=Control.EvalVar<double>(keyName+"VoidWidth");
  voidDepth=Control.EvalVar<double>(keyName+"VoidDepth");
  voidLength=Control.EvalVar<double>(keyName+"VoidLength");
  
  feFront=Control.EvalVar<double>(keyName+"FeFront");
  feLeftWall=Control.EvalVar<double>(keyName+"FeLeftWall");
  feRightWall=Control.EvalVar<double>(keyName+"FeRightWall");
  feRoof=Control.EvalVar<double>(keyName+"FeRoof");

  concFront=Control.EvalVar<double>(keyName+"ConcFront");
  concLeftWall=Control.EvalVar<double>(keyName+"ConcLeftWall");
  concRightWall=Control.EvalVar<double>(keyName+"ConcRightWall");
  concRoof=Control.EvalVar<double>(keyName+"ConcRoof");

  feMat=ModelSupport::EvalMat<int>(Control,keyName+"PipeMat");
  concMat=ModelSupport::EvalMat<int>(Control,keyName+"ConcMat");

  return;
}

void
VespaInner::createUnitVector(const attachSystem::FixedComp& FC,
			     const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("VespaInner","createUnitVector");

  // add nosecone + half centre
  yStep+=voidLength/2.0;
  attachSystem::FixedComp& Outer=getKey("Outer");
  attachSystem::FixedComp& Mid=getKey("Mid");
  attachSystem::FixedComp& Inner=getKey("Inner");
  Outer.createUnitVector(FC,sideIndex);
  Mid.createUnitVector(FC,sideIndex);
  Inner.createUnitVector(FC,sideIndex);
  applyOffset();
  setDefault("Inner");
  return;
}
 
void
VespaInner::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("VespaInner","createSurfaces");

  // Inner void
  if (!FrontBackCut::frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(voidLength/2.0),Y);
      setFront(SMap.realSurf(buildIndex+1));
    }

  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*voidDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*voidHeight,Z);  

  Geometry::Vec3D APt=
    Origin-X*(voidWidth/2.0)+Y*(voidLength/2.0-backCutStep);
  Geometry::Vec3D BPt=
    Origin+X*(voidWidth/2.0)+Y*(voidLength/2.0-backCutStep);

  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+7,APt,-X,-Z,backAngle);
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+8,BPt,X,Z,backAngle);

  // FE Layer
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(feFront+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(feLeftWall+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(feRightWall+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(voidDepth+feFloor),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(feRoof+voidHeight),Z);  

  APt-=X*feLeftWall;
  BPt+=X*feRightWall;
  
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+17,APt,-X,-Z,backAngle);
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+18,BPt,X,Z,backAngle);


  // Outer Layer
  ModelSupport::buildPlane(SMap,buildIndex+22,
			   Origin+Y*(concFront+feFront+voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+23,
		   Origin-X*(concLeftWall+feLeftWall+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,
		   Origin+X*(concRightWall+feRightWall+voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+25,
		   Origin-Z*(concFloor+feFloor+voidDepth),Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,
			   Origin+Z*(concRoof+feRoof+voidHeight),Z);  

  APt-=X*concLeftWall;
  BPt+=X*concRightWall;
  
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+27,APt,-X,-Z,backAngle);
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+28,BPt,X,Z,backAngle);

  
  return;
}

void
VespaInner::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("VespaInner","createObjects");

  const HeadRule frontHR=FrontBackCut::getFrontRule();
  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 3 -4 5 -6 -7 -8");
  makeCell("Void",System,cellIndex++,0,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				"-12 13 -14 15 -16 -17 -18"
				"( 2 : -3 : 4 : -5 : 6 : 7 : 8 )");
  makeCell("FeLayer",System,cellIndex++,feMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				"-22 23 -24 25 -26 -27 -28"
				"( 12 : -13 : 14 : -15 : 16 : 17 : 18 )");
  makeCell("ConcLayer",System,cellIndex++,concMat,0.0,HR*frontHR);

  // Exclude:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-22 23 -24 25 -26 -27 -28");
  addOuterSurf(HR);      

  return;
}

void
VespaInner::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("VespaInner","createLinks");

  attachSystem::FixedComp& innerFC=FixedGroup::getKey("Inner");
  attachSystem::FixedComp& midFC=FixedGroup::getKey("Mid");
  attachSystem::FixedComp& outerFC=FixedGroup::getKey("Outer");

  // INNER VOID
  createFrontLinks(innerFC,Origin,Y);
  innerFC.setConnect(1,Origin+Y*(voidLength/2.0),Y);
  innerFC.setConnect(2,Origin-X*(voidWidth/2.0),-X);
  innerFC.setConnect(3,Origin+X*(voidWidth/2.0),X);
  innerFC.setConnect(4,Origin-Z*voidDepth,-Z);
  innerFC.setConnect(5,Origin+Z*voidHeight,Z);  


  innerFC.setLinkSurf(1,SMap.realSurf(buildIndex+2));
  innerFC.setLinkSurf(2,-SMap.realSurf(buildIndex+3));
  innerFC.setLinkSurf(3,SMap.realSurf(buildIndex+4));
  innerFC.setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  innerFC.setLinkSurf(5,SMap.realSurf(buildIndex+6));
  
  
  // INNER VOID
  createFrontLinks(midFC,Origin,Y);
  midFC.setConnect(1,Origin+Y*(feFront+voidLength/2.0),Y);
  midFC.setConnect(2,Origin-X*(feLeftWall+voidWidth/2.0),-X);
  midFC.setConnect(3,Origin+X*(feRightWall+voidWidth/2.0),X);
  midFC.setConnect(4,Origin-Z*(feFloor+voidDepth),-Z);
  midFC.setConnect(5,Origin+Z*(feRoof+voidHeight),Z);  

  midFC.setLinkSurf(1,SMap.realSurf(buildIndex+12));
  midFC.setLinkSurf(2,-SMap.realSurf(buildIndex+13));
  midFC.setLinkSurf(3,SMap.realSurf(buildIndex+14));
  midFC.setLinkSurf(4,-SMap.realSurf(buildIndex+15));
  midFC.setLinkSurf(5,SMap.realSurf(buildIndex+16));

  
    // OUTER VOID
  createFrontLinks(outerFC,Origin,Y);
  outerFC.setConnect(1,Origin+Y*(feFront+concFront+voidLength/2.0),Y);
  outerFC.setConnect(2,Origin-X*(concLeftWall+feLeftWall+voidWidth/2.0),-X);
  outerFC.setConnect(3,Origin+X*(concRightWall+feRightWall+voidWidth/2.0),X);
  outerFC.setConnect(4,Origin-Z*(concFloor+feFloor+voidDepth),-Z);
  outerFC.setConnect(5,Origin+Z*(concRoof+feRoof+voidHeight),Z);  

  outerFC.setLinkSurf(1,SMap.realSurf(buildIndex+22));
  outerFC.setLinkSurf(2,-SMap.realSurf(buildIndex+23));
  outerFC.setLinkSurf(3,SMap.realSurf(buildIndex+24));
  outerFC.setLinkSurf(4,-SMap.realSurf(buildIndex+25));
  outerFC.setLinkSurf(5,SMap.realSurf(buildIndex+26));

  
  return;
}

void
VespaInner::createAll(Simulation& System,
		   const attachSystem::FixedComp& FC,
		   const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("VespaInner","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  
  createSurfaces();    
  createObjects(System);

  createLinks();
  insertObjects(System);   
  return;
}
  
}  // NAMESPACE essSystem
