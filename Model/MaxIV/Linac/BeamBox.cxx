/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   Linac/BeamBox.cxx
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
#include "BeamBox.h"

namespace tdcSystem
{

BeamBox::BeamBox(const std::string& Key)  :
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


BeamBox::~BeamBox() 
  /*!
    Destructor
  */
{}

void
BeamBox::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Data Base
  */
{
  ELog::RegMethod RegA("BeamBox","populate");

  FixedRotate::populate(Control);
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  length=Control.EvalVar<double>(keyName+"Length");

  backThick=Control.EvalVar<double>(keyName+"BackThick");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  innerMat=ModelSupport::EvalDefMat<int>(Control,keyName+"InnerMat",0);
  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");

  return;
}


void
BeamBox::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("BeamBox","createSurface");

  if (!isActive("front"))
    {
      makePlane("Front",SMap,buildIndex+1,Origin,Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
      setSurf("Front",buildIndex+1);
    }
  else
    setSurf("Front",getRule("front").getPrimarySurface());
  
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);  
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  ModelSupport::buildPlane
    (SMap,buildIndex+12,Origin+Y*(length+backThick),Y);  
  ModelSupport::buildPlane
    (SMap,buildIndex+13,Origin-X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+14,Origin+X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+15,Origin-Z*(wallThick+height/2.0),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+16,Origin+Z*(wallThick+height/2.0),Z);

  return;
}

void
BeamBox::createObjects(Simulation& System)
  /*!
    Create the main volume
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("BeamBox","createObjects");

  const HeadRule frontHR=getRule("front");
  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 3 -4 5 -6");
  makeCell("Void",System,cellIndex++,innerMat,0.0,HR*frontHR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
			    "-12 13 -14 15 -16 (2:-3:4:-5:6)");
  makeCell("Wall",System,cellIndex++,mainMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-12 13 -14 15 -16");
  addOuterSurf(HR*frontHR);
  
  return;
}


void
BeamBox::createLinks()
  /*!
    Create link points
  */
{
  ELog::RegMethod RegA("BeamBox","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);  // Front and back

  FixedComp::setConnect(1,Origin+Y*(backThick+length),Y);
  FixedComp::setConnect(2,Origin-X*(wallThick+width/2.0),-X);
  FixedComp::setConnect(3,Origin+X*(wallThick+width/2.0),X);
  FixedComp::setConnect(4,Origin-Z*(wallThick+height/2.0),-Z);
  FixedComp::setConnect(5,Origin+Z*(wallThick+height/2.0),Z);

  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+12));
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+13));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+14));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+15));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+16));


  return;
}
  
void
BeamBox::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Linear component to set axis etc
    \param sideIndex :: link Index
  */
{
  ELog::RegMethod RegA("BeamBox","createAll(FC,index)");

  populate(System.getDataBase());  
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  
  return;
}

   
}  // NAMESPACE tdcSystem
