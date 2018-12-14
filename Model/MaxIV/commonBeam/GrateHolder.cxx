/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/GrateHolder.cxx
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
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
#include "support.h"
#include "inputParam.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "GrateHolder.h"


namespace xraySystem
{

GrateHolder::GrateHolder(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,8),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{}


GrateHolder::~GrateHolder()
  /*!
    Destructor
   */
{}

void
GrateHolder::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("GrateHolder","populate");

  FixedOffset::populate(Control);
  
  gWidth=Control.EvalVar<double>(keyName+"Width");
  gThick=Control.EvalVar<double>(keyName+"Thick");
  gLength=Control.EvalVar<double>(keyName+"Length");

  sideThick=Control.EvalVar<double>(keyName+"SideThick");
  sideHeight=Control.EvalVar<double>(keyName+"SideHeight");

  endThick=Control.EvalVar<double>(keyName+"EndThick");
  endWidth=Control.EvalVar<double>(keyName+"EndWidth");
  endHeight=Control.EvalVar<double>(keyName+"EndHeight");

  fullLength=2.0*endThick+gLength;
  
  xstalMat=ModelSupport::EvalMat<int>(Control,keyName+"XstalMat");
  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  sideMat=ModelSupport::EvalMat<int>(Control,keyName+"SideMat");

  return;
}

void
GrateHolder::createUnitVector(const attachSystem::FixedComp& FC,
				  const long int sideIndex)
  /*!
    Create the unit vectors.
    Note that it also set the view point that neutrons come from
    \param FC :: FixedComp for origin
    \param sideIndex :: direction for link
  */
{
  ELog::RegMethod RegA("GrateHolder","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();  
  return;
}

void
GrateHolder::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("GrateHolder","createSurfaces");

  // Outer
  ModelSupport::buildPlane(SMap,buildIndex+11,
			   Origin-Y*(gLength/2.0+endThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(gLength/2.0+endThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(gWidth/2.0+sideThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(gWidth/2.0+sideThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*sideHeight,Z);
  

  // end points
  ModelSupport::buildPlane(SMap,buildIndex+23,
			   Origin-X*(endWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,
			   Origin+X*(endWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+26,Origin+Z*endHeight,Z);
  
  
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(gLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(gLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(gWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(gWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*gThick,Z);

  SurfMap::addSurf("Front",SMap.realSurf(buildIndex+11));
  SurfMap::addSurf("Back",-SMap.realSurf(buildIndex+12));
  SurfMap::addSurf("Left",SMap.realSurf(buildIndex+13));
  SurfMap::addSurf("Right",-SMap.realSurf(buildIndex+14));

  return; 
}

void
GrateHolder::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("GrateHolder","createObjects");

  std::string Out;

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 3 -4 5 -6 ");
  makeCell("XStal",System,cellIndex++,xstalMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 13 -3 5 -16 ");
  makeCell("LeftSide",System,cellIndex++,sideMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 4 -14 5 -16 ");
  makeCell("RightSide",System,cellIndex++,sideMat,0.0,Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -1 3 -4 5 -16 ");
  makeCell("FBar",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -12 3 -4 5 -16 ");
  makeCell("BBar",System,cellIndex++,mainMat,0.0,Out);
  

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -1 23 -24 16 -26");
  makeCell("FSupport",System,cellIndex++,mainMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -12 23 -24 16 -26");
  makeCell("BSupport",System,cellIndex++,mainMat,0.0,Out);


  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 23 -24 16 -26");
  makeCell("SVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 13 -23 16 -26");
  makeCell("LSVoid",System,cellIndex++,0,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 24 -14  16 -26");
  makeCell("RSVoid",System,cellIndex++,0,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 3 -4 6 -16 ");
  makeCell("MidVoid",System,cellIndex++,0,0.0,Out);
  
  // Main outer void
  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -12 13 -14 5 -26");
  addOuterSurf(Out);
  
  return; 
}

void
GrateHolder::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("GrateHolder","createLinks");

  FixedComp::setConnect(0,Origin-Y*(gLength/2.0+endThick),-Y);
  FixedComp::setConnect(1,Origin+Y*(gLength/2.0+endThick),Y);
  FixedComp::setConnect(2,Origin-X*(gWidth/2.0+sideThick),-X);
  FixedComp::setConnect(3,Origin+X*(gWidth/2.0+sideThick),X);
  FixedComp::setConnect(4,Origin,-Z);
  FixedComp::setConnect(5,Origin+Z*endHeight,Z);

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+11));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+12));
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+13));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+14));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+26));

  return;
}

void
GrateHolder::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("GrateHolder","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE xraySystem
