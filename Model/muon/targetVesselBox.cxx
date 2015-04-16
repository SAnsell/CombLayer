/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   muon/targetVesselBox.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell/Goran Skoro
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
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "Convex.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "targetVesselBox.h"

namespace muSystem
{

targetVesselBox::targetVesselBox(const std::string& Key)  : 
  attachSystem::FixedComp(Key,6),attachSystem::ContainedComp(),
  tvBoxIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(tvBoxIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key to use
  */
{}


targetVesselBox::~targetVesselBox() 
  /*!
    Destructor
  */
{}

void
targetVesselBox::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Data Base
  */
{
  ELog::RegMethod RegA("targetVesselBox","populate");

  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYAngle");

  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  width=Control.EvalVar<double>(keyName+"Width");
  steelThick=Control.EvalVar<double>(keyName+"SteelThick");
  
  steelMat=ModelSupport::EvalMat<int>(Control,keyName+"SteelMat");    
       
  return;
}

void
targetVesselBox::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Master Origin
  */
{
  ELog::RegMethod RegA("targetVesselBox","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,0);    

  return;
}

void
targetVesselBox::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("targetVesselBox","createSurface");

  // outer layer
  ModelSupport::buildPlane(SMap,tvBoxIndex+1,Origin-Y*depth/2.0,Y);
  ModelSupport::buildPlane(SMap,tvBoxIndex+2,Origin+Y*depth/2.0,Y);
  ModelSupport::buildPlane(SMap,tvBoxIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,tvBoxIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,tvBoxIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,tvBoxIndex+6,Origin+Z*height/2.0,Z);
  
  // steel layer
  ModelSupport::buildPlane(SMap,tvBoxIndex+11,
			   Origin-Y*(depth/2.0-steelThick),Y);
  ModelSupport::buildPlane(SMap,tvBoxIndex+12,
			   Origin+Y*(depth/2.0-steelThick),Y);
  ModelSupport::buildPlane(SMap,tvBoxIndex+13,
			   Origin-X*(width/2.0-steelThick),X);
  ModelSupport::buildPlane(SMap,tvBoxIndex+14,
			   Origin+X*(width/2.0-steelThick),X);
  ModelSupport::buildPlane(SMap,tvBoxIndex+15,
			   Origin-Z*(height/2.0-steelThick),Z);
  ModelSupport::buildPlane(SMap,tvBoxIndex+16,
			   Origin+Z*(height/2.0-steelThick),Z);  

  return;
}

void
targetVesselBox::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("targetVesselBox","createObjects");
  
  std::string Out;
  std::string Out1;

    // Steel
  Out=ModelSupport::getComposite(SMap,tvBoxIndex,"1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);
  addBoundarySurf(Out);
  Out1=ModelSupport::getComposite(SMap,tvBoxIndex,"(-11:12:-13:14:-15:16) ");  
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out+Out1));

    // hole
  Out=ModelSupport::getComposite(SMap,tvBoxIndex,"11 -12 13 -14 15 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  
  return;
}


void
targetVesselBox::createLinks()
  /*!
    Create links
   */
{
  ELog::RegMethod RegA("targetVesselBox","createLinks");

  FixedComp::setLinkSurf(0,-SMap.realSurf(tvBoxIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(tvBoxIndex+2));
  FixedComp::setLinkSurf(2,-SMap.realSurf(tvBoxIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(tvBoxIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(tvBoxIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(tvBoxIndex+6));

  return;
}

void
targetVesselBox::createAll(Simulation& System,
			   const attachSystem::FixedComp& FC)

  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("targetVesselBox","createAll");
  populate(System.getDataBase());
  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  return;
}
  
}  // NAMESPACE shutterSystem
