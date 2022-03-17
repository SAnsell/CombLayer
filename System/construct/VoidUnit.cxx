/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/VoidUnit.cxx
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
#include "Exception.h"
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
#include "generateSurf.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "VoidUnit.h"


namespace constructSystem
{

VoidUnit::VoidUnit(const std::string& Key)  :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
    \param ID :: Index number
  */
{}


  
VoidUnit::~VoidUnit() 
  /*!
    Destructor
  */
{}
  
void
VoidUnit::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase to copy
  */
{
  ELog::RegMethod RegA("VoidUnit","populate");

  FixedRotate::populate(keyName,Control);

  height=Control.EvalDefVar<double>(keyName+"Height",-1.0);
  width=Control.EvalDefVar<double>(keyName+"Width",-1.0);
  length=Control.EvalDefVar<double>(keyName+"Length",-1.0);
  
  return;
}
  

void
VoidUnit::createSurfaces()
  /*!
    Create surfaces
  */
{
  ELog::RegMethod RegA("VoidUnit","createSurfaces");
    
  if (!isActive("front"))
    {
      SurfMap::makePlane("front",SMap,buildIndex+1,Origin,Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      if (length<Geometry::zeroTol)
	throw ColErr::InContainerError<std::string>
	  (keyName+"Length","Value not defined/zero");
      
      SurfMap::makePlane("back",SMap,buildIndex+2,Origin+Y*length,Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }
  if (width>Geometry::zeroTol)
    {
      SurfMap::makePlane("left",SMap,buildIndex+3,Origin-X*(width/2.0),X);
      SurfMap::makePlane("right",SMap,buildIndex+4,Origin+X*(width/2.0),X);
      ExternalCut::setCutSurf("left",SMap.realSurf(buildIndex+3));
      ExternalCut::setCutSurf("right",-SMap.realSurf(buildIndex+4));
    }
  if (height>Geometry::zeroTol)
    {
      SurfMap::makePlane("base",SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
      SurfMap::makePlane("top",SMap,buildIndex+6,Origin+Z*(height/2.0),Z);
      ExternalCut::setCutSurf("base",SMap.realSurf(buildIndex+5));
      ExternalCut::setCutSurf("top",-SMap.realSurf(buildIndex+6));
    }
 return;
}

void
VoidUnit::createLinks()
  /*!
    Create linkes
  */
{
  ELog::RegMethod RegA("VoidUnit","createLinks");


  ExternalCut::createLink("front",*this,0,Origin,-Y);
  ExternalCut::createLink("back",*this,1,Origin,Y);

  // Note length may not properly exist:
  length=getLinkDistance(1,2);
  
  if (width>Geometry::zeroTol)
    {
      ExternalCut::createLink("left",*this,2,Origin,-X);
      ExternalCut::createLink("right",*this,3,Origin,X);
    }
  if (width>Geometry::zeroTol)
    {
      ExternalCut::createLink("left",*this,2,Origin+Y*(length/2.0),-X);
      ExternalCut::createLink("right",*this,3,Origin+Y*(length/2.0),X);
    }
  if (height>Geometry::zeroTol)
    {
      ExternalCut::createLink("base",*this,4,Origin+Y*(length/2.0),-Z);
      ExternalCut::createLink("top",*this,5,Origin+Y*(length/2.0),Z);
    }
  
  
  return;
}

   
void
VoidUnit::createAll(Simulation& System,
		   const attachSystem::FixedComp& FC,
		   const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FC :: Object for origin
    \param sideIndex :: linkPoint for axis
    \param wallBoundary :: External boundary rule 
  */
{
  ELog::RegMethod RegA("VoidUnit","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createLinks();
  
  return;
}
  
}  // NAMESPACE constructSystem
