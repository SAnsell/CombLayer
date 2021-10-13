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
  fullCut=Control.EvalDefVar<double>(keyName+"FullCut",-10.0);
  innerCut=Control.EvalDefVar<double>(keyName+"InnerCut",-10.0);

  backThick=Control.EvalVar<double>(keyName+"BackThick");
  b4cThick=Control.EvalVar<double>(keyName+"B4CThick");
  backExtension=Control.EvalDefVar<double>(keyName+"BackExtension",-10.0);
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  
  innerMat=ModelSupport::EvalDefMat(Control,keyName+"InnerMat",0);
  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  backMat=ModelSupport::EvalDefMat(Control,keyName+"BackMat",mainMat);
  b4cMat=ModelSupport::EvalDefMat(Control,keyName+"B4CMat",backMat);

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
    (SMap,buildIndex+1012,Origin+Y*(length+backThick-b4cThick),Y);  
  ModelSupport::buildPlane
    (SMap,buildIndex+13,Origin-X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+14,Origin+X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+15,Origin-Z*(wallThick+height/2.0),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+16,Origin+Z*(wallThick+height/2.0),Z);

  if (fullCut>Geometry::zeroTol)
    ModelSupport::buildPlane(SMap,buildIndex+202,Origin+Y*fullCut,Y);
  if (innerCut>Geometry::zeroTol)
    ModelSupport::buildPlane(SMap,buildIndex+302,Origin+Y*innerCut,Y);


  if (backExtension>Geometry::zeroTol)
    {
      ModelSupport::buildPlane
	(SMap,buildIndex+103,Origin-X*(backExtension+wallThick+width/2.0),X);
      ModelSupport::buildPlane
	(SMap,buildIndex+104,Origin+X*(backExtension+wallThick+width/2.0),X);
    }
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
  
  if (innerCut>Geometry::zeroTol)
    {
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"302 -2 3 -4 5 -6");
      makeCell("Void",System,cellIndex++,innerMat,0.0,HR);
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-302 3 -4 5 -6");
      makeCell("Void",System,cellIndex++,0,0.0,HR*frontHR);
    }
  else
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 3 -4 5 -6");
      makeCell("Void",System,cellIndex++,innerMat,0.0,HR*frontHR);
    }

  if (fullCut>Geometry::zeroTol)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,
				   "202 -2 13 -14 15 -16 (-3:4:-5:6)");
      makeCell("Wall",System,cellIndex++,mainMat,0.0,HR);
      HR=ModelSupport::getHeadRule(SMap,buildIndex,
				   "-202 13 -14 15 -16 (-3:4:-5:6)");
      makeCell("WallCut",System,cellIndex++,0,0.0,HR*frontHR);
    }
  else
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,
				   "-2 13 -14 15 -16 (-3:4:-5:6)");
      makeCell("Wall",System,cellIndex++,mainMat,0.0,HR*frontHR);
    }

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1012 2 13 -14 15 -16");
  makeCell("BackWall",System,cellIndex++,backMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-12 1012 13 -14 15 -16");
  makeCell("BackB4C",System,cellIndex++,b4cMat,0.0,HR);

  if (backExtension>Geometry::zeroTol)
    {
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"-1012 2 103 -104 15 -16 (-13:14)");
      makeCell("BackExtension",System,cellIndex++,backMat,0.0,HR);
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"1012 -12 103 -104 15 -16 (-13:14)");
      makeCell("BackExtension",System,cellIndex++,b4cMat,0.0,HR);
      
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"-12 -2 103 -104 15 -16 (-13:14)");
      makeCell("BackExtVoid",System,cellIndex++,0,0.0,HR*frontHR);
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-12 103 -104 15 -16");
    }
  else
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-12 13 -14 15 -16");
    }
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

  FixedComp::setNamedLinkSurf(1,"back",SMap.realSurf(buildIndex+12));
  FixedComp::setNamedLinkSurf(2,"left",-SMap.realSurf(buildIndex+13));
  FixedComp::setNamedLinkSurf(3,"right",SMap.realSurf(buildIndex+14));
  FixedComp::setNamedLinkSurf(4,"base",-SMap.realSurf(buildIndex+15));
  FixedComp::setNamedLinkSurf(5,"top",SMap.realSurf(buildIndex+16));

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
