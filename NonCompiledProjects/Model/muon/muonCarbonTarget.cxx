/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   muon/muonCarbonTarget.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell/Goron Skoro
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
#include <iterator>
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
#include "generateSurf.h"
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "muonCarbonTarget.h"

namespace muSystem
{

muonCarbonTarget::muonCarbonTarget(const std::string& Key)  : 
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key to use
  */
{}


muonCarbonTarget::~muonCarbonTarget() 
  /*!
    Destructor
  */
{}

void
muonCarbonTarget::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Data base unit
  */
{
  ELog::RegMethod RegA("muonCarbonTarget","populate");

  FixedRotate::populate(Control);

  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  width=Control.EvalVar<double>(keyName+"Width");
  
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
       
  return;
}

void
muonCarbonTarget::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("muonCarbonTarget","createSurface");

  // CH4 layer
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*depth/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*depth/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height/2.0,Z);

  return;
}

void
muonCarbonTarget::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("muonCarbonTarget","createObjects");
  
  std::string Out;

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);
  addBoundarySurf(Out);
  System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));
  
  return;
}


void
muonCarbonTarget::createLinks()
  /*!
    Create links
   */
{
  ELog::RegMethod RegA("muonCarbonTarget","createLinks");

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  FixedComp::setConnect(0,Origin-Y*(depth/2.0),-Y);
  FixedComp::setConnect(1,Origin+Y*(depth/2.0),Y);
  FixedComp::setConnect(2,Origin-X*(width/2.0),-X);
  FixedComp::setConnect(3,Origin+X*(width/2.0),X);
  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);

  return;
}

void
muonCarbonTarget::createAll(Simulation& System,
			    const attachSystem::FixedComp& FC,
			    const long int sideIndex)
  /*!
    Create the shutter
    \param System :: Simulation to process
    \param FC :: Fixed unit to use
  */
{
  ELog::RegMethod RegA("muonCarbonTarget","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  return;
}
  
}  // NAMESPACE shutterSystem
