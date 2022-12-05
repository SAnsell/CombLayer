/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/EdgeWater.cxx 
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include <array>
#include <algorithm>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "LayerComp.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "EdgeWater.h"

namespace essSystem
{

EdgeWater::EdgeWater(const std::string& key) :
  attachSystem::FixedComp(key,6),
  attachSystem::ContainedComp(),
  attachSystem::LayerComp(0,0),
  attachSystem::CellMap(),
  attachSystem::ExternalCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param key :: Name for item in search
  */
{}

EdgeWater::EdgeWater(const EdgeWater& A) : 
  attachSystem::FixedComp(A),
  attachSystem::ContainedComp(A),
  attachSystem::LayerComp(A),
  attachSystem::CellMap(A),
  attachSystem::ExternalCut(A),
  width(A.width),wallThick(A.wallThick),modMat(A.modMat),
  wallMat(A.wallMat),modTemp(A.modTemp)
  /*!
    Copy constructor
    \param A :: EdgeWater to copy
  */
{} 

EdgeWater&
EdgeWater::operator=(const EdgeWater& A)
  /*!
    Assignment operator
    \param A :: EdgeWater to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::LayerComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      cellIndex=A.cellIndex;
      width=A.width;
      wallThick=A.wallThick;
      modMat=A.modMat;
      wallMat=A.wallMat;
      modTemp=A.modTemp;
    }
  return *this;
}

EdgeWater*
EdgeWater::clone() const
  /*!
    Virtual copy constructor
    \return new (this)
   */
{
  return new EdgeWater(*this);
}
  
EdgeWater::~EdgeWater() 
  /*!
    Destructor
  */
{}

void
EdgeWater::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("EdgeWater","populate");

  width=Control.EvalVar<double>(keyName+"Width");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  cutAngle=Control.EvalVar<double>(keyName+"CutAngle");
  cutWidth=Control.EvalVar<double>(keyName+"CutWidth");

  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  modTemp=Control.EvalVar<double>(keyName+"ModTemp");
  
  return;
}
  
void
EdgeWater::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("EdgeWater","createSurface");

  // Only x surfaces:

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+X*(width/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+11,
			   Origin-X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+X*(wallThick+width/2.0),X);


    
  // front dividers:
  const Geometry::Vec3D EdPtA=Origin-X*(cutWidth/2.0);
  const Geometry::Vec3D EdPtB=Origin+X*(cutWidth/2.0);
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+103,EdPtA,X,Z,cutAngle);
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+104,EdPtB,X,Z,-cutAngle);
  
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+203,EdPtA-X*wallThick,
				  X,Z,cutAngle);
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+204,EdPtB+X*wallThick,
				  X,Z,-cutAngle);
  
  return;
}
 
void
EdgeWater::createObjects(Simulation& System)
  /*!
    Adds the main components
    \param System :: Simulation to create objects in
    \param container string :: wing surface edge of reflector
  */
{
  ELog::RegMethod RegA("EdgeWater","createObjects");

  HeadRule HR;
  
  const HeadRule containerHR=ExternalCut::getRule("Container");
  const HeadRule divHR=ExternalCut::getRule("Divider");
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 103 -104");
  makeCell("Water",System,cellIndex++,modMat,modTemp,
	   HR*containerHR+divHR);
  
  // Two walls : otherwise divider container
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -1 103");
  makeCell("Wall",System,cellIndex++,wallMat,modTemp,HR*containterHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 -12 -104");
  makeCell("Wall",System,cellIndex++,wallMat,modTemp,HR*containterHR);

  // front walls
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -103 203");
  makeCell("Wall",System,cellIndex++,wallMat,modTemp,HR*containerHR*divHR);
  CellMap::setCell("InnerAlSupply",cellIndex-1);
    
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-12 104 -204");
  makeCell("Wall",System,cellIndex++,wallMat,modTemp,
				   HR*containerHR*divHR));
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 203 -204");
  addOuterSurf(HR*divHR);
  return;
}

void
EdgeWater::createLinks()
  /*!
    Construct links for edge water unit
    First 6 are the normal x,y,z 
  */
{
  ELog::RegMethod RegA("EdgeWater","createLinks");

  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 203");
  FixedComp::setLinkSurf(2,HR.complement());
  FixedComp::setConnect(2,Origin-X*(wallThick+cutWidth/2.0),-X);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-12 -204");
  FixedComp::setLinkSurf(3,HR.complement());
  FixedComp::setConnect(3,Origin+X*(wallThick+cutWidth/2.0),X);
  
  return;
}
  
Geometry::Vec3D
EdgeWater::getSurfacePoint(const size_t,
			const long int) const
  /*!
    Given a side and a layer calculate the link point
    \param  :: layer, 0 is inner moderator [0-6]
    \param  :: Side [0-3] // mid sides   
    \return Surface point
  */
{
  ELog::RegMethod RegA("EdgeWater","getSurfacePoint");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

HeadRule
EdgeWater::getLayerHR(const size_t,
		      const long int) const
  /*!
    Given a side and a layer calculate the link point
    \param  :: layer, 0 is inner moderator [0-6]
    \param  :: Side [0-3] // mid sides   
    \return Surface Rule
  */
{
  ELog::RegMethod RegA("EdgeWater","getLayerString");

  throw ColErr::AbsObjMethod("Not implemented yet");

}


void
EdgeWater::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed object just for origin/axis
    \param sideIndex :: link point to create on
  */
{
  ELog::RegMethod RegA("EdgeWater","createAll");

  if (!isActive("Divider"))
    setCutSurf("Divider",FC,sideIndex);
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();

  createObjects(System,divider);

  createLinks();
  insertObjects(System);       
  return;
}
  
}  // NAMESPACE essSystem
