/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/HPJaws.cxx.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include "BaseVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
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
#include "FrontBackCut.h" 
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h" 

#include "HPJaws.h"

namespace xraySystem
{

HPJaws::HPJaws(const std::string& Key) :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::FrontBackCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  flangeJoin(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


HPJaws::~HPJaws() 
  /*!
    Destructor
  */
{}

void
HPJaws::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("HPJaws","populate");

  FixedRotate::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");
  sideThick=Control.EvalVar<double>(keyName+"SideThick");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  flangeInnerRadius=Control.EvalVar<double>(keyName+"FlangeInnerRadius");
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");

  dividerThick=Control.EvalVar<double>(keyName+"DividerThick");
  dividerGap=Control.EvalVar<double>(keyName+"DividerGap");

  jawFarLen=Control.EvalVar<double>(keyName+"JawFarLen");
  jawEdgeLen=Control.EvalVar<double>(keyName+"JawEdgeLen");
  jawThick=Control.EvalVar<double>(keyName+"JawThick");
  jawCornerEdge=Control.EvalVar<double>(keyName+"JawCornerEdge");
  jawCornerFar=Control.EvalVar<double>(keyName+"JawCornerFar");
  jawXGap=Control.EvalVar<double>(keyName+"JawXGap");
  jawZGap=Control.EvalVar<double>(keyName+"JawZGap");

  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  jawMat=ModelSupport::EvalMat<int>(Control,keyName+"JawMat");
  
  return;
}


void
HPJaws::createSurfaces()
  /*!
    Create All the surfaces
    Note: If BOTH front and Flange surfaces are given in 
    ExternalCut. Then the frount is built AROUND the
  */
{
  ELog::RegMethod RegA("HPJaws","createSurfaces");

  // Option A : Front defined
  if (!isActive("front"))
    {
      if (flangeJoin)
	{
	  ModelSupport::buildPlane
	    (SMap,buildIndex+1,Origin-Y*(flangeLength+length/2.0),Y);
	  ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
	}
      else
	{
	  ModelSupport::buildPlane
	    (SMap,buildIndex+21,Origin-Y*(length/2.0-wallThick),Y);
	  ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+21));
	}
	
    }
  
  if (!isActive("back"))
    {
      ModelSupport::buildPlane
	(SMap,buildIndex+2,Origin+Y*(flangeLength+length/2.0),Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }
  
  // flange
  ModelSupport::buildCylinder
    (SMap,buildIndex+107,Origin,Y,flangeInnerRadius);
  
  // flange both sides:
  ModelSupport::buildCylinder
    (SMap,buildIndex+207,Origin,Y,flangeRadius);

  // main volume
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(length/2.0),Y);

  if (!flangeJoin)
    ModelSupport::buildPlane
      (SMap,buildIndex+21,Origin-Y*(length/2.0-wallThick),Y);

  ModelSupport::buildPlane
    (SMap,buildIndex+22,Origin+Y*(length/2.0-wallThick),Y);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+sideThick);

  // Jaw/MidWall 
  ModelSupport::buildPlane(SMap,buildIndex+301,Origin-Y*(dividerThick/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+302,Origin+Y*(dividerThick/2.0),Y);
  
  ModelSupport::buildPlane(SMap,buildIndex+303,Origin-X*(dividerGap/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+304,Origin+X*(dividerGap/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+305,Origin-Z*(dividerGap/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+306,Origin+Z*(dividerGap/2.0),Z);

  // Front Jaw [vertical]
  ModelSupport::buildPlane
    (SMap,buildIndex+401,Origin-Y*(jawThick+dividerThick/2.0),Y);

  ModelSupport::buildPlane(SMap,buildIndex+403,Origin-X*(jawEdgeLen/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+404,Origin+X*(jawEdgeLen/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+413,Origin-X*(jawCornerEdge/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+414,Origin+X*(jawCornerEdge/2.0),X);

  const Geometry::Vec3D HOrg(Origin+Z*jawZGap);
  const Geometry::Vec3D LOrg(Origin-Z*jawZGap);
  ModelSupport::buildPlane(SMap,buildIndex+405,LOrg,Z);
  ModelSupport::buildPlane(SMap,buildIndex+415,LOrg-Z*jawCornerFar,Z);
  ModelSupport::buildPlane(SMap,buildIndex+425,LOrg-Z*jawFarLen,Z);

  ModelSupport::buildPlane(SMap,buildIndex+406,HOrg,Z);
  ModelSupport::buildPlane(SMap,buildIndex+416,HOrg+Z*jawCornerFar,Z);
  ModelSupport::buildPlane(SMap,buildIndex+426,HOrg+Z*jawFarLen,Z);

  // Back Jaw [horrizontal]
  ModelSupport::buildPlane
    (SMap,buildIndex+402,Origin+Y*(jawThick+dividerThick/2.0),Y);

  ModelSupport::buildPlane(SMap,buildIndex+505,Origin-Z*(jawEdgeLen/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+506,Origin+Z*(jawEdgeLen/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+515,Origin-Z*(jawCornerEdge/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+516,Origin+Z*(jawCornerEdge/2.0),Z);

  const Geometry::Vec3D POrg(Origin+X*jawZGap);
  const Geometry::Vec3D MOrg(Origin-X*jawZGap);
  ModelSupport::buildPlane(SMap,buildIndex+503,MOrg,X);
  ModelSupport::buildPlane(SMap,buildIndex+513,MOrg-X*jawCornerFar,X);
  ModelSupport::buildPlane(SMap,buildIndex+523,MOrg-X*jawFarLen,X);

  ModelSupport::buildPlane(SMap,buildIndex+504,POrg,X);
  ModelSupport::buildPlane(SMap,buildIndex+514,POrg+X*jawCornerFar,X);
  ModelSupport::buildPlane(SMap,buildIndex+524,POrg+X*jawFarLen,X);

  return;
}

void
HPJaws::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("HPJaws","createObjects");

  HeadRule HR;

  const HeadRule& frontHR=getRule("front");
  const HeadRule& backHR=getRule("back");

  // Front flange
  if (!flangeJoin)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex," -107 -21");
      makeCell("FrontVoid",System,cellIndex++,voidMat,0.0,HR*frontHR);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-207 107 -11");
      makeCell("FrontFlange",System,cellIndex++,wallMat,0.0,HR*frontHR);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 207 -11");
      makeCell("FrontOuter",System,cellIndex++,0,0.0,HR*frontHR);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 107 11 -21");
      makeCell("FrontWall",System,cellIndex++,wallMat,0.0,HR);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 21 -401");
      makeCell("Void",System,cellIndex++,voidMat,0.0,HR);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 -22 402");
      makeCell("Void",System,cellIndex++,voidMat,0.0,HR);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 7 21 -22");
      makeCell("Main",System,cellIndex++,wallMat,0.0,HR);
    }
  else
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 -401");
      makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 -22 402");
      makeCell("Void",System,cellIndex++,voidMat,0.0,HR);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 7 -22");
      makeCell("Main",System,cellIndex++,wallMat,0.0,HR*frontHR);
    }

  
  // back wall
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 207 22 -12");
  makeCell("BackWall",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -107 22 ");
  makeCell("BackVoid",System,cellIndex++,voidMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-207 107 22");
  makeCell("BackFlange",System,cellIndex++,wallMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 207 12");
  makeCell("BackOuter",System,cellIndex++,0,0.0,HR*backHR);

  // Mid wall
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"301 -302 303 -304 305 -306");
  makeCell("MidVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-7 301 -302 (-303:304:-305:306)");
  makeCell("MidWall",System,cellIndex++,wallMat,0.0,HR);

  // Z JAWS:
  // top jaw

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"401 -301 405 -406 403 -404");
  makeCell("JawZGap",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"401 -301 -405 425 403 -404 (413:415) (-414:415)");
  makeCell("JawZLow",System,cellIndex++,jawMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"401 -301 406 -426 403 -404 (413:-416) (-414:-416)");
  makeCell("JawZHigh",System,cellIndex++,jawMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"401 -301 403 -413 425 -415");
  makeCell("JawZLowVA",System,cellIndex++,voidMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"401 -301 -404 414 425 -415");
  makeCell("JawZLowVB",System,cellIndex++,voidMat,0.0,HR);

  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"401 -301 403 -413 -426 416");
  makeCell("JawZHighVA",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"401 -301 -404 414 -426 416");
  makeCell("JawZHighVB",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,
			       "-7 401 -301 (-403:404:-425:426)");
  makeCell("JawZVoid",System,cellIndex++,voidMat,0.0,HR);



  // X JAWS:
  // top jaw

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"302 -402 503 -504 505 -506");
  makeCell("JawXGap",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"302 -402 -503 523 505 -506 (515:513) (-516:513)");
  makeCell("JawXLow",System,cellIndex++,jawMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"302 -402 504 -524 505 -506 (515:-514) (-516:-514)");
  makeCell("JawXHigh",System,cellIndex++,jawMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"302 -402 505 -515 523 -513");
  makeCell("JawXLowVA",System,cellIndex++,voidMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"302 -402 -506 516 523 -513");
  makeCell("JawXLowVB",System,cellIndex++,voidMat,0.0,HR);

  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"302 -402 505 -515 -524 514");
  makeCell("JawXHighVA",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"302 -402 -506 516 -524 514");
  makeCell("JawXHighVB",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,
			       "-7 302 -402 (-505:506:-523:524)");
  makeCell("JawXVoid",System,cellIndex++,voidMat,0.0,HR);


  
  // outer void box:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 ");
  addOuterSurf(HR*frontHR*backHR);

  return;
}

void 
HPJaws::createLinks()
  /*!
    Create the linked units
  */
{
  ELog::RegMethod RegA("HPJaws","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);  //front and back
  ExternalCut::createLink("back",*this,1,Origin,Y);  //front and back

  return;
}

void
HPJaws::createAll(Simulation& System,
	       const attachSystem::FixedComp& FC,
	       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("HPJaws","createAll");
  
  populate(System.getDataBase());

  if (flangeJoin)
    // makes surface 21 at the FC(sindeIndex) point
    createCentredUnitVector(FC,sideIndex,length/2.0-wallThick);
  else
    createCentredUnitVector(FC,sideIndex,length/2.0+flangeLength);

  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
