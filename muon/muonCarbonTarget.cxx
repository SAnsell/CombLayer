/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   muon/muonCarbonTarget.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell/Goron Skoro
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
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

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
#include "SimProcess.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "World.h"
#include "muonCarbonTarget.h"

namespace muSystem
{

muonCarbonTarget::muonCarbonTarget(const std::string& Key)  : 
  attachSystem::FixedComp(Key,6),attachSystem::ContainedComp(),
  muCtIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(muCtIndex+1)
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


  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYAngle");

  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  width=Control.EvalVar<double>(keyName+"Width");
  
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
       
  return;
}

void
muonCarbonTarget::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
  */
{
  ELog::RegMethod RegA("muonCarbonTarget","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,0.0);  
  
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
  ModelSupport::buildPlane(SMap,muCtIndex+1,Origin-Y*depth/2.0,Y);
  ModelSupport::buildPlane(SMap,muCtIndex+2,Origin+Y*depth/2.0,Y);
  ModelSupport::buildPlane(SMap,muCtIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,muCtIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,muCtIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,muCtIndex+6,Origin+Z*height/2.0,Z);

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

  Out=ModelSupport::getComposite(SMap,muCtIndex,"1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);
  addBoundarySurf(Out);
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
  
  return;
}


void
muonCarbonTarget::createLinks()
  /*!
    Create links
   */
{
  ELog::RegMethod RegA("muonCarbonTarget","createLinks");

  FixedComp::setLinkSurf(0,-SMap.realSurf(muCtIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(muCtIndex+2));
  FixedComp::setLinkSurf(2,-SMap.realSurf(muCtIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(muCtIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(muCtIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(muCtIndex+6));


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
			    const attachSystem::FixedComp& FC)
  /*!
    Create the shutter
    \param System :: Simulation to process
    \param FC :: Fixed unit to use
  */
{
  ELog::RegMethod RegA("muonCarbonTarget","createAll");
  populate(System.getDataBase());

  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  return;
}
  
}  // NAMESPACE shutterSystem
