/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R1Common/DipoleExtract.cxx
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
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"

#include "DipoleExtract.h"

namespace xraySystem
{

DipoleExtract::DipoleExtract(const std::string& Key) : 
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


DipoleExtract::~DipoleExtract() 
  /*!
    Destructor
  */
{}

void
DipoleExtract::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("DipoleExtract","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  
  wideHeight=Control.EvalVar<double>(keyName+"WideHeight");
  wideWidth=Control.EvalVar<double>(keyName+"WideWidth");
  exitLength=Control.EvalVar<double>(keyName+"ExitLength");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  edgeThick=Control.EvalVar<double>(keyName+"EdgeThick");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  outerMat=ModelSupport::EvalMat<int>(Control,keyName+"OuterMat");

  return;
}

void
DipoleExtract::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("DipoleExtract","createSurface");

  if (!ExternalCut::isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!ExternalCut::isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
      setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(wideWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(wideWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(wideHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(wideHeight/2.0),Z);

  const Geometry::Vec3D mainX(X*(width/2.0));
  const Geometry::Vec3D mainZ(Z*(height/2.0));
  const Geometry::Vec3D wideX(X*(wideWidth/2.0));
  const Geometry::Vec3D wideZ(Z*(wideHeight/2.0));

  ModelSupport::buildPlane(SMap,buildIndex+35,
			   Origin-mainX-mainZ,
			   Origin-mainX-mainZ+Y,
			   Origin-wideX-wideZ,
			   Z);
  ModelSupport::buildPlane(SMap,buildIndex+36,
			   Origin-mainX+mainZ,
			   Origin-mainX+mainZ+Y,
			   Origin-wideX+wideZ,
			   Z);
  ModelSupport::buildPlane(SMap,buildIndex+45,
			   Origin+mainX-mainZ,
			   Origin+mainX-mainZ+Y,
			   Origin+wideX-wideZ,
			   Z);
  ModelSupport::buildPlane(SMap,buildIndex+46,
			   Origin+mainX+mainZ,
			   Origin+mainX+mainZ+Y,
			   Origin+wideX+wideZ,
			   Z);

  ModelSupport::buildPlane
    (SMap,buildIndex+63,Origin-X*(exitLength+wideWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+64,Origin+X*(exitLength+wideWidth/2.0),X);

  // WALLS:

  ModelSupport::buildPlane
    (SMap,buildIndex+105,Origin-Z*(wallThick+height/2.0),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+106,Origin+Z*(wallThick+height/2.0),Z);

  // 10% overflow to allow definiative angle join and no microgaps
  ModelSupport::buildPlane
    (SMap,buildIndex+115,Origin-Z*(wallThick+wideHeight/1.9),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+116,Origin+Z*(wallThick+wideHeight/1.9),Z);

  const Geometry::Vec3D wallZ(Z*(wallThick+height/2.0));
  const Geometry::Vec3D wWallZ(Z*(wallThick+wideHeight/2.0));

  ModelSupport::buildPlane(SMap,buildIndex+135,
			   Origin-mainX-wallZ,
			   Origin-wideX-wWallZ,
			   Origin-mainX-wallZ+Y,
			   Z);
  ModelSupport::buildPlane(SMap,buildIndex+136,
			   Origin-mainX+wallZ,
			   Origin-mainX+wallZ+Y,
			   Origin-wideX+wWallZ,			   
			   Z);
  ModelSupport::buildPlane(SMap,buildIndex+145,
			   Origin+mainX-wallZ,
			   Origin+mainX-wallZ+Y,
			   Origin+wideX-wWallZ,
			   Z);
  ModelSupport::buildPlane(SMap,buildIndex+146,
			   Origin+mainX+wallZ,
			   Origin+mainX+wallZ+Y,
			   Origin+wideX+wWallZ,
			   Z);

  ModelSupport::buildPlane
    (SMap,buildIndex+163,Origin-X*(edgeThick+exitLength+wideWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+164,Origin+X*(edgeThick+exitLength+wideWidth/2.0),X);

  
			   
  return;
}

void
DipoleExtract::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("DipoleExtract","createObjects");

  std::string Out;
  const std::string fbStr=getRuleStr("front")+
    getRuleStr("back");

  //

  Out=ModelSupport::getComposite(SMap,buildIndex,"13 -14 5 35 45 -36 -46 -6  ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+fbStr);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"13 -14 -136 -146 -106 (36:6:46)");
  makeCell("TopWall",System,cellIndex++,wallMat,0.0,Out+fbStr);

  Out=ModelSupport::getComposite
    (SMap,buildIndex,"13 -14 135 145 105 (-35:-5:-45)");
  makeCell("LowWall",System,cellIndex++,wallMat,0.0,Out+fbStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 63 -135 -115 105 ");
  makeCell("LowGap",System,cellIndex++,outerMat,0.0,Out+fbStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-64 -145 -115 105");
  makeCell("LowGap",System,cellIndex++,outerMat,0.0,Out+fbStr);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"63 136 116 -106 ");
  makeCell("TopGap",System,cellIndex++,outerMat,0.0,Out+fbStr);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"-64 146 116 -106 ");
  makeCell("TopGap",System,cellIndex++,outerMat,0.0,Out+fbStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,"63 115 -116 (-13:136:-135) ");
  makeCell("End",System,cellIndex++,wallMat,0.0,Out+fbStr);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"(14:146:-145) -64 115 -116 ");
  makeCell("End",System,cellIndex++,wallMat,0.0,Out+fbStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,"63 -64 105 -106");
  addOuterUnionSurf(Out+fbStr);


  return;
}

void 
DipoleExtract::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("DipoleExtract","createLinks");
  ExternalCut::createLink("front",*this,0,Origin,Y);
  ExternalCut::createLink("back",*this,1,Origin,Y);
  return;
}


void
DipoleExtract::createAll(Simulation& System,
			 const attachSystem::FixedComp& FC,
			 const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("DipoleExtract","createAll");
  
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE epbSystem
