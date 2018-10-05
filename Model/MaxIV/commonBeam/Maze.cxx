/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/Maze.cxx
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
#include "Maze.h"


namespace xraySystem
{

Maze::Maze(const std::string& Key) :
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
Maze::populate(const FuncDataBase& Control)
  /*!
    Sets the size of the object
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("Maze","populate");

  FixedOffset::populate(Control);
  
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");

  mainOutLength=Control.EvalVar<double>(keyName+"MainOutLength");
  mainThick=Control.EvalVar<double>(keyName+"MainThick");
  mainXWidth=Control.EvalVar<double>(keyName+"MainXWidth");

  catchOutLength=Control.EvalVar<double>(keyName+"CatchOutLength");
  catchThick=Control.EvalVar<double>(keyName+"CatchThick");
  catchXGap=Control.EvalVar<double>(keyName+"CatchXGap");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  
  return;
}

void
Maze::createUnitVector(const attachSystem::FixedComp& FC,
				 const long int sideIndex)
  /*!
    Create the unit vectors: Note only to construct front/back surf
    \param FC :: Centre point
    \param sideIndex :: Side index
  */
{
  ELog::RegMethod RegA("Maze","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  
  return;
}

void
Maze::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Maze","createSurface");

  // InnerWall and OuterWall MUST be set
  if (!ExternalCut::isActive("innerWall") ||
      !ExternalCut::isActive("outerWall"))
    throw ColErr::InContainerError<std::string>
      ("InnerWall/OuterWall","Maze:"+keyName);

  // origin in on outer wall:
  
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);
  
  // main wall
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*mainOutLength,Y);

  ModelSupport::buildPlane(SMap,buildIndex+22,
			   Origin+Y*(mainOutLength+mainThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+23,
			   Origin-X*(mainThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,
			   Origin+X*(mainXWidth-width/2.0),X);

  // side wall
  ModelSupport::buildPlane(SMap,buildIndex+32,Origin+Y*catchOutLength,Y);
  ModelSupport::buildPlane(SMap,buildIndex+42,
			   Origin+Y*(catchThick+catchOutLength),Y);

  const double wallXPos(catchXGap-width/2.0);

  ModelSupport::buildPlane(SMap,buildIndex+33,Origin+X*wallXPos,X);
  ModelSupport::buildPlane(SMap,buildIndex+34,Origin+X*(wallXPos+catchThick),X);

  return;
}

void
Maze::createObjects(Simulation& System) 
  /*!
    Creates the colllimator block
    \param System :: Simuation for object
  */
{
  ELog::RegMethod RegA("Maze","createObjects");

  std::string Out;
  const std::string innerStr=ExternalCut::getRuleStr("innerWall");
  const std::string outerStr=ExternalCut::getRuleStr("outerWall");
  const std::string outerComp=ExternalCut::getComplementStr("outerWall");
  
  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 5 -6 ");
  makeCell("Void",System,cellIndex++,0,0.0,Out+innerStr+outerStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," -12 3 -24 5 -6 ");
  makeCell("MainVoid",System,cellIndex++,0,0.0,Out+outerComp);

  Out=ModelSupport::getComposite(SMap,buildIndex," -22 23 -3 -24 5 -6 ");
  makeCell("MainLeftWall",System,cellIndex++,wallMat,0.0,Out+outerComp);

  Out=ModelSupport::getComposite(SMap,buildIndex," 12 -22 3 -24 5 -6 ");
  makeCell("MainWall",System,cellIndex++,wallMat,0.0,Out+outerComp);

  
  Out=ModelSupport::getComposite(SMap,buildIndex," -42 33 -34 5 -6 ");
  makeCell("SideWall",System,cellIndex++,wallMat,0.0,Out+outerComp);

  Out=ModelSupport::getComposite(SMap,buildIndex," 32 -42 -33 23 5 -6 ");
  makeCell("BackWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 12 -22 3 -24 5 -6 ");
  //  makeCell("SideWall",System,cellIndex++,wallMat,0.0,Out+outerComp);

  Out=ModelSupport::getComposite(SMap,buildIndex," -22 24 -33 5 -6 ");
  makeCell("SideVoid",System,cellIndex++,0,0.0,Out+outerComp);

  Out=ModelSupport::getComposite(SMap,buildIndex," 22 -32 23 -33 5 -6 ");
  makeCell("BackVoid",System,cellIndex++,0,0.0,Out);

  
  // needs to be group
  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 5 -6 ");
  addOuterSurf("Inner",Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -42 23 -34 5 -6 ");
  addOuterSurf("Main",Out);
  //  Out=ModelSupport::getComposite(SMap,buildIndex,"13 -14 15 -106 ");
  //  addOuterSurf("Inner",Out);
  return;
}

void
Maze::createLinks()
  /*!
    Construct the links for the system
   */
{
  ELog::RegMethod RegA("Maze","createLinks");

  
  
  return;
}

void
Maze::createAll(Simulation& System,
                     const attachSystem::FixedComp& FC,
                     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation 
    \param FC :: Fixed component to set axis etc
    \param sideIndex :: position of linkpoint
  */
{
  ELog::RegMethod RegA("Maze","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

  
}  // NAMESPACE xraySystem
