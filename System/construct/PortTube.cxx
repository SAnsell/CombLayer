/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/PortTube.cxx
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
#include <array>

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
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "FrontBackCut.h"

#include "portItem.h"
#include "PortTube.h"

namespace constructSystem
{

PortTube::PortTube(const std::string& Key) :
  attachSystem::FixedOffset(Key,2),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::FrontBackCut(),
  vacIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(vacIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

  
PortTube::~PortTube() 
  /*!
    Destructor
  */
{}

void
PortTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("PortTube","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  radius=Control.EvalVar<double>(keyName+"Radius");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");
  
  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  
  return;
}

void
PortTube::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("PortTube","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  Origin+=Y*(inPortLen+wallThick+length/2.0);
  applyOffset();

  return;
}


void
PortTube::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("PortTube","createSurfaces");

  // Do outer surfaces (vacuum ports)
  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,vacIndex+1,Origin-Y*(length/2.0),Y);
      setFront(SMap.realSurf(vacIndex+1));
    }
  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,vacIndex+2,Origin+Y*(length/2.0),Y);
      setBack(-SMap.realSurf(vacIndex+2));
    }
  

  return;
}

void
PortTube::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("PortTube","createObjects");

  std::string Out;
  return;
}

void
PortTube::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane.
    Port position are used for first two link points
    Note that 0/1 are the flange surfaces
  */
{
  ELog::RegMethod RegA("PortTube","createLinks");

  
  //  FrontBackCut::createFrontLinks(*this,ACentre,Y); 
  //  FrontBackCut::createBackLinks(*this,BCentre,Y);  
  
  return;
}

void
PortTube::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("PortTube","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
