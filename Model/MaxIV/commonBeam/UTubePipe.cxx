/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/UTubePipe.cxx
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
#include <array>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
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
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SurfMap.h"

#include "UTubePipe.h"

namespace xraySystem
{

UTubePipe::UTubePipe(const std::string& Key) : 
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedGroup("Pipe","FFlange","BFlange"),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),attachSystem::FrontBackCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


UTubePipe::~UTubePipe() 
  /*!
    Destructor
  */
{}

void
UTubePipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("UTubePipe","populate");
  
  FixedRotate::populate(Control);

  // Void + Fe special:
  width=Control.EvalVar<double>(keyName+"Width");  
  height=Control.EvalVar<double>(keyName+"Height");
  length=Control.EvalVar<double>(keyName+"Length");
  
  feThick=Control.EvalVar<double>(keyName+"FeThick");

  flangeARadius=Control.EvalPair<double>(keyName+"FlangeFrontRadius",
					 keyName+"FlangeRadius");
  flangeBRadius=Control.EvalPair<double>(keyName+"FlangeBackRadius",
					 keyName+"FlangeRadius");

  flangeALength=Control.EvalPair<double>(keyName+"FlangeFrontLength",
					 keyName+"FlangeLength");
  flangeBLength=Control.EvalPair<double>(keyName+"FlangeBackLength",
					 keyName+"FlangeLength");


  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");

  return;
}
  
void
UTubePipe::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("UTubePipe","createSurfaces");
  
  // Inner void
  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,
			       Origin-Y*(length/2.0),Y); 
      FrontBackCut::setFront(SMap.realSurf(buildIndex+1));
    }
  getShiftedFront(SMap,buildIndex+11,Y,flangeALength);
    
  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,
			       Origin+Y*(length/2.0),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+2));
    }
  getShiftedBack(SMap,buildIndex+12,Y,-flangeBLength);
  
  // main pipe
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z); 

  
  // two inner
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin-X*(width/2.0),Y,height/2.0);
  ModelSupport::buildCylinder(SMap,buildIndex+8,Origin+X*(width/2.0),Y,height/2.0);

  ModelSupport::buildCylinder(SMap,buildIndex+17,
			      Origin-X*(width/2.0),Y,height/2.0+feThick);
  ModelSupport::buildCylinder(SMap,buildIndex+18,
			      Origin+X*(width/2.0),Y,height/2.0+feThick);


  // main pipe walls
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(feThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(feThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(feThick+height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(feThick+height/2.0),Z); 

  
  // FLANGE SURFACES FRONT/BACK:
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeARadius);
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,flangeBRadius);

  return;
}

void
UTubePipe::createObjects(Simulation& System)
  /*!
    Adds the vacuum system
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("UTubePipe","createObjects");

  HeadRule HR;

  const HeadRule& frontHR=getFrontRule();
  const HeadRule& backHR=getBackRule();
  
  // Void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 5 -6 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-3 -7 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"4 -8 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 3 -4 6 -16");
  makeCell("TopPipe",System,cellIndex++,feMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 3 -4 -5 15");
  makeCell("BasePipe",System,cellIndex++,feMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 -3 7 -17 ");
  makeCell("LeftPipe",System,cellIndex++,feMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 4 8 -18 ");
  makeCell("RightPipe",System,cellIndex++,feMat,0.0,HR);

  // FLANGE Front: 
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				 " -11 -107 ((7 -3) : (8 4) : -5 : 6) ");
  makeCell("FrontFlange",System,cellIndex++,feMat,0.0,HR*frontHR);

  // FLANGE Back:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				 " 12 -207 ((7 -3) : (8 4) : -5 : 6) ");
  makeCell("BackFlange",System,cellIndex++,feMat,0.0,HR*backHR);

  // outer boundary [flange front/back]
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex," -11 -107 ");
  addOuterSurf("FFlange",HR*frontHR);
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex," 12 -207 ");
  addOuterUnionSurf("BFlange",HR*backHR);
  // outer boundary mid tube
  //  (3:-7) (-4:8) 5 -6 ");
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,
				    " 11 -12 15 -16 (3:-17) (-4:-18)");
  addOuterUnionSurf("Pipe",HR);
  return;
}
  
void
UTubePipe::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("UTubePipe","createLinks");

  //stuff for intersection
  FrontBackCut::createLinks(*this,Origin,Y);  //front and back

  FixedComp::setConnect(2,Origin-X*(feThick+height/2.0+width/2.0),-X);
  FixedComp::setConnect(3,Origin-X*(feThick+height/2.0+width/2.0),X);
  FixedComp::setConnect(4,Origin-Z*(feThick+height/2.0),-Z);
  FixedComp::setConnect(5,Origin+Z*(feThick+height/2.0),Z);
  
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+17));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+18));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+15));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+16));

  return;
}
  
  
void
UTubePipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("UTubePipe","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
