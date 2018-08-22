/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxpeem/maxpeemOpticsHutch.cxx
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
#include <array>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
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
#include "FixedGroup.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedSpace.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "PortChicane.h"

#include "maxpeemOpticsHutch.h"

namespace xraySystem
{

maxpeemOpticsHutch::maxpeemOpticsHutch(const std::string& Key) : 
  attachSystem::FixedOffset(Key,18),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  nameSideIndex(15,"floorCut");
  nameSideIndex(16,"roofCut");
  nameSideIndex(17,"frontCut");
}


maxpeemOpticsHutch::~maxpeemOpticsHutch() 
  /*!
    Destructor
  */
{}

void
maxpeemOpticsHutch::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("maxpeemOpticsHutch","populate");
  
  FixedOffset::populate(Control);

  // depth set by external + sides.
  // Void + Fe special:
  height=Control.EvalVar<double>(keyName+"Height");
  length=Control.EvalVar<double>(keyName+"Length");
  outWidth=Control.EvalVar<double>(keyName+"OutWidth");
  ringShortWidth=Control.EvalVar<double>(keyName+"RingShortWidth");
  ringLongWidth=Control.EvalVar<double>(keyName+"RingLongWidth");

  shortLen=Control.EvalVar<double>(keyName+"ShortLen");
  fullLen=Control.EvalVar<double>(keyName+"FullLen");

  innerSkin=Control.EvalVar<double>(keyName+"InnerSkin");
  pbWallThick=Control.EvalVar<double>(keyName+"PbWallThick");
  pbFrontThick=Control.EvalVar<double>(keyName+"PbFrontThick");
  pbBackThick=Control.EvalVar<double>(keyName+"PbBackThick");
  pbRoofThick=Control.EvalVar<double>(keyName+"PbRoofThick");
  outerSkin=Control.EvalVar<double>(keyName+"OuterSkin");

  holeXStep=Control.EvalDefVar<double>(keyName+"HoleXStep",0.0);
  holeZStep=Control.EvalDefVar<double>(keyName+"HoleZStep",0.0);
  holeRadius=Control.EvalDefVar<double>(keyName+"HoleRadius",0.0);

  inletXStep=Control.EvalDefVar<double>(keyName+"InletXStep",0.0);
  inletZStep=Control.EvalDefVar<double>(keyName+"InletZStep",0.0);
  inletRadius=Control.EvalDefVar<double>(keyName+"InletRadius",0.0);
  
  innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat");
  pbMat=ModelSupport::EvalMat<int>(Control,keyName+"PbMat");
  outerMat=ModelSupport::EvalMat<int>(Control,keyName+"OuterMat");
  
  return;
}

void
maxpeemOpticsHutch::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("maxpeemOpticsHutch","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  // shift forward for back wall
  Origin+=Y*(outerSkin+innerSkin+pbFrontThick);
  return;
}
 
void
maxpeemOpticsHutch::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("maxpeemOpticsHutch","createSurfaces");

  // Inner void [large volme
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*outWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*ringLongWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);

  ModelSupport::buildPlane(SMap,buildIndex+23,Origin+X*ringShortWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin+X*ringShortWidth+Y*shortLen,
			   Origin+X*ringLongWidth+Y*fullLen,
			   Origin+X*ringLongWidth+Y*fullLen+Z,
			   X);

  const Geometry::Plane* SPtr=
    SMap.realPtr<const Geometry::Plane>(buildIndex+13);
  
    // Inner void [large volme
  double TF(innerSkin);
  double TW(innerSkin);
  double TB(innerSkin);
  double TR(innerSkin);
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin-Y*TF,Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*(length+TB),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+103,Origin-X*(outWidth+TW),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(ringLongWidth+TW),X);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(height+TR),Z);
  ModelSupport::buildPlane(SMap,buildIndex+123,Origin+X*(ringShortWidth+TW),X);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+113,SPtr,TW);

  TF+=pbFrontThick;
  TW+=pbWallThick;
  TR+=pbRoofThick;
  TB+=pbBackThick;

  ModelSupport::buildPlane(SMap,buildIndex+201,Origin-Y*TF,Y);
  ModelSupport::buildPlane(SMap,buildIndex+202,Origin+Y*(length+TB),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+103,Origin-X*(outWidth+TW),X);
  ModelSupport::buildPlane(SMap,buildIndex+204,Origin+X*(ringLongWidth+TW),X);
  ModelSupport::buildPlane(SMap,buildIndex+206,Origin+Z*(height+TR),Z);
  ModelSupport::buildPlane(SMap,buildIndex+223,Origin+X*(ringShortWidth+TW),X);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+213,SPtr,TW);

  TF+=outerSkin;
  TW+=outerSkin;
  TR+=outerSkin;
  TB+=outerSkin;

  ModelSupport::buildPlane(SMap,buildIndex+301,Origin-Y*TF,Y);
  ModelSupport::buildPlane(SMap,buildIndex+302,Origin+Y*(length+TB),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+103,Origin-X*(outWidth+TW),X);
  ModelSupport::buildPlane(SMap,buildIndex+304,Origin+X*(ringLongWidth+TW),X);
  ModelSupport::buildPlane(SMap,buildIndex+306,Origin+Z*(height+TR),Z);
  ModelSupport::buildPlane(SMap,buildIndex+323,Origin+X*(ringShortWidth+TW),X);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+313,SPtr,TW);


  
  return;
}

void
maxpeemOpticsHutch::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("maxpeemOpticsHutch","createObjects");

  std::string Out;


  return;
}

void
maxpeemOpticsHutch::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("maxpeemOpticsHutch","createLinks");


  return;
}

void
maxpeemOpticsHutch::createChicane(Simulation& System)
  /*!
    Generic function to create chicanes
    \param System :: Simulation 
  */
{
  ELog::RegMethod Rega("maxpeemOpticsHutch","createChicane");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const FuncDataBase& Control=System.getDataBase();

  const size_t NChicane=
    Control.EvalDefVar<size_t>(keyName+"NChicane",0);

  for(size_t i=0;i<NChicane;i++)
    {
      const std::string NStr(std::to_string(i));
      std::shared_ptr<PortChicane> PItem=
	std::make_shared<PortChicane>(keyName+"Chicane"+NStr);

      OR.addObject(PItem);
      PItem->addInsertCell("Main",getCell("WallVoid"));
      PItem->addInsertCell("Inner",getCell("InnerWall",0));
      PItem->addInsertCell("Inner",getCell("LeadWall",0));
      PItem->addInsertCell("Inner",getCell("OuterWall",0));
      // set surfaces:

      PItem->setCutSurf("innerWall",*this,"innerLeftWall");
      PItem->setCutSurf("outerWall",*this,"leftWall");

      PItem->setPrimaryCell("Main",getCell("WallVoid"));
  
      PItem->registerSpaceCut("Main",
			      PItem->getSideIndex("innerLeft"),
			      PItem->getSideIndex("innerRight"));

      
      PItem->createAll(System,*this,getSideIndex("leftWall"));

      PItem->clearSpace("Main");
      PItem->addInsertCell("Main",getCell("OuterVoid",0));

      PItem->setPrimaryCell("Main",getCell("OuterVoid"));
      PItem->registerSpaceCut("Main",
			      PItem->getSideIndex("outerLeft"),
			      PItem->getSideIndex("outerRight"));
      PItem->insertObjects(System);
      PChicane.push_back(PItem);
      //      PItem->splitObject(System,23,getCell("WallVoid"));
      //      PItem->splitObject(System,24,getCell("SplitVoid"));      
    }
  return;
}

void
maxpeemOpticsHutch::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("maxpeemOpticsHutch","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  createChicane(System);
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
