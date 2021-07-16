/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   Linac/BeamWing.cxx
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
#include "BaseModVisit.h"
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
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BeamWing.h"

namespace tdcSystem
{

BeamWing::BeamWing(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}


BeamWing::~BeamWing() 
  /*!
    Destructor
  */
{}

void
BeamWing::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Data Base
  */
{
  ELog::RegMethod RegA("BeamWing","populate");

  FixedRotate::populate(Control);
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");

  return;
}


void
BeamWing::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("BeamWing","createSurface");

  if (!isActive("front"))
    {
      SurfMap::makePlane("Front",SMap,buildIndex+1,Origin-Y*(depth/2.0),Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  else
    setSurf("Front",getRule("front").getPrimarySurface());
  
  if (!isActive("back"))
    {
      SurfMap::makePlane("Back",SMap,buildIndex+2,Origin+Y*(depth/2.0),Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }
  else
    setSurf("Back",getRule("back").getPrimarySurface());

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);  

  setSurf("Left",SMap.realSurf(buildIndex+3));
  setSurf("Right",SMap.realSurf(buildIndex+4));
  setSurf("Base",SMap.realSurf(buildIndex+5));
  setSurf("Top",SMap.realSurf(buildIndex+6));
  return;
}

void
BeamWing::createObjects(Simulation& System)
  /*!
    Create the main volume
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("BeamWing","createObjects");
  
  HeadRule HR=
    ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 5 -6");
  HR*=getRule("front");
  HR*=getRule("back");
  makeCell("Main",System,cellIndex++,mainMat,0.0,HR);

  addOuterSurf(HR);
  return;
}

void
BeamWing::createLinks()
  /*!
    Create link points
  */
{
  ELog::RegMethod RegA("BeamWing","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);  // Front and back
  ExternalCut::createLink("back",*this,1,Origin,Y);   // Front and back
  
  FixedComp::setConnect(2,Origin-X*(width/2.0),-X);
  FixedComp::setConnect(3,Origin+X*(width/2.0),X);
  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);

  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));


  return;
}

void
BeamWing::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Linear component to set axis etc
    \param sideIndex :: link Index
  */
{
  ELog::RegMethod RegA("BeamWing","createAll(FC,index)");

  populate(System.getDataBase());  
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  
  return;
}

   
}  // NAMESPACE tdcSystem
