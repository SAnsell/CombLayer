/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxpeem/LCollimator.cxx
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "LCollimator.h"


namespace xraySystem
{

LCollimator::LCollimator(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),attachSystem::SurfMap()
  /*!
    Default constructor
    \param Key :: Key name for variables
  */
{}
  

void
LCollimator::populate(const FuncDataBase& Control)
  /*!
    Sets the size of the object
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("LCollimator","populate");

  FixedRotate::populate(Control);
  
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");
  length=Control.EvalVar<double>(keyName+"Length");

  innerAWidth=Control.EvalVar<double>(keyName+"InnerAWidth");
  innerAHeight=Control.EvalVar<double>(keyName+"InnerAHeight");

  innerBWidth=Control.EvalVar<double>(keyName+"InnerBWidth");
  innerBHeight=Control.EvalVar<double>(keyName+"InnerBHeight");
  
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  
  return;
}

void
LCollimator::createUnitVector(const attachSystem::FixedComp& FC,
				 const long int sideIndex)
  /*!
    Create the unit vectors: Note only to construct front/back surf
    \param FC :: Centre point
    \param sideIndex :: Side index
  */
{
  ELog::RegMethod RegA("LCollimator","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}


void
LCollimator::createSurfaces()
  /*!
    Create All the surfaces
    Built with only the front inner 
   */
{
  ELog::RegMethod RegA("LCollimator","createSurface");
  
  // full volume
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  // cut volume
  const Geometry::Vec3D frontPt(Origin-X*innerAWidth-Y*(length/2.0)
				-Z*innerAHeight);
  const Geometry::Vec3D backPt(Origin-X*innerBWidth+Y*(length/2.0)
			       -Z*innerBHeight);

  const Geometry::Vec3D innerX(frontPt+Z);
  const Geometry::Vec3D innerZ(frontPt+X);

  ModelSupport::buildPlane(SMap,buildIndex+13,frontPt,backPt,innerX,X);
  ModelSupport::buildPlane(SMap,buildIndex+15,frontPt,backPt,innerZ,Z);
  
  return;
}

void
LCollimator::createObjects(Simulation& System) 
  /*!
    Creates the colllimator block
    \param System :: Simuation for object
  */
{
  ELog::RegMethod RegA("LCollimator","createObjects");
  std::string Out;

  // L metal
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 3 -4 5 -6 (-13:-15)");
  CellMap::makeCell("LShape",System,cellIndex++,mat,0.0,Out);

  // Void  metal
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 13 -4 15 -6");
  CellMap::makeCell("Void",System,cellIndex++,voidMat,0.0,Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 3 -4 5 -6");
  addOuterSurf(Out);
  return;
}

void
LCollimator::createLinks()
  /*!
    Construct the links for the system
  */
{
  ELog::RegMethod RegA("LCollimator","createLinks");

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  return;
}
   
void
LCollimator::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation 
    \param FC :: Fixed component to set axis etc
    \param sideIndex :: position of linkpoint
  */
{
  ELog::RegMethod RegA("LCollimator","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks(); 
  insertObjects(System);
  return;
}

 
}  // NAMESPACE xraySystem
