/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   saxs/Capillary.cxx
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
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "SurfMap.h"

#include "Capillary.h"

namespace saxsSystem
{

Capillary::Capillary(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

Capillary::~Capillary() 
  /*!
    Destructor
  */
{}

void
Capillary::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("Capillary","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  if (radius<0.0 || length<0.0)
    throw ColErr::RangeError<double>
	     (radius,length,0.0,"["+keyName+"] has neither Radius or Length");
  

  innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat");
  wallMat=ModelSupport::EvalDefMat<int>(Control,keyName+"WallMat",0);

  return;
}

void
Capillary::createUnitVector(const attachSystem::FixedComp& FC,
                             const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("Capillary","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
Capillary::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("Capillary","createSurfaces");
    
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-X*(length/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+X*(length/2.0),X);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,X,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,X,radius+wallThick);
  addSurf("InnerRadius",buildIndex+7);
  addSurf("OuterRadius",buildIndex+17);
  
  
  return;
}

void
Capillary::createObjects(Simulation& System)
  /*!
    Builds the simple capillary tube
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("Capillary","createObjects");

  std::string Out;
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 -7 ");
  makeCell("Sample",System,cellIndex++,innerMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 7 -17");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 -17");
  addOuterSurf(Out);
  
  return;
}

  
void
Capillary::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("Capillary","createLinks");

  //stuff for intersection
    
  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
  FixedComp::setConnect(2,Origin-X*(radius+wallThick),-X);
  FixedComp::setConnect(3,Origin+X*(radius+wallThick),X);
  FixedComp::setConnect(4,Origin-Z*(radius+wallThick),-Z);
  FixedComp::setConnect(5,Origin+Z*(radius+wallThick),Z);

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+7));      
 
  return;
}
  
void
Capillary::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: FixedComp Index
  */
{
  ELog::RegMethod RegA("Capillary","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
