/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   pipeBuild/CylTarget.cxx
 *
 * Copyright (c) 2004-2026 by Stuart Ansell
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
#include "Vec3D.h"
#include "surfDIter.h"
#include "surfRegister.h"
#include "surfDivide.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "surfDBase.h"
#include "mergeTemplate.h"
#include "CylTarget.h"

namespace pipeSystem
{

CylTarget::CylTarget(const std::string& Key) :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::CellMap()
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

CylTarget::CylTarget(const CylTarget& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  length(A.length),radius(A.radius),wallThick(A.wallThick),
  mainMat(A.mainMat),wallMat(A.wallMat),
  nWallLayers(A.nWallLayers),
  wallFracList(A.wallFracList),wallMatList(A.wallMatList)
  /*!
    Copy constructor
    \param A :: CylTarget to copy
  */
{}

CylTarget&
CylTarget::operator=(const CylTarget& A)
  /*!
    Assignment operator
    \param A :: CylTarget to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      length=A.length;
      radius=A.radius;
      wallThick=A.wallThick;
      mainMat=A.mainMat;
      wallMat=A.wallMat;
      nWallLayers=A.nWallLayers;
      wallFracList=A.wallFracList;
      wallMatList=A.wallMatList;
    }
  return *this;
}

CylTarget::~CylTarget()
  /*!
    Destructor
  */
{}
  

void
CylTarget::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("CylTarget","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  radius=Control.EvalVar<double>(keyName+"Radius");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  wallMat=ModelSupport::EvalDefMat(Control,keyName+"WallMat",mainMat);

  nWallLayers=0;
  if (wallThick>Geometry::zeroTol)
    {
      nWallLayers=Control.EvalDefVar<size_t>(keyName+"NWallLayers",0);
      ModelSupport::populateDivide(Control,nWallLayers,
				   keyName+"WLayerMat",
				   wallMat,wallMatList);
      ModelSupport::populateDivideLen(Control,nWallLayers,
				      keyName+"WLayerThick",
				      wallThick,wallFracList);
    }
  return;
}

void
CylTarget::createSurfaces()
  /*!
    Create planes for system
  */
{
  ELog::RegMethod RegA("CylTarget","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);  
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+wallThick);

  return; 
}

void
CylTarget::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("CylTarget","createObjects");

  HeadRule HR;

  // Inner 
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -7");
  makeCell("Inner",System,cellIndex++,mainMat,0.0,HR);

  if (wallThick>Geometry::zeroTol)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 7 -17");
      makeCell("Outer",System,cellIndex++,wallMat,0.0,HR);
    }
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -17");  
  addOuterSurf(HR);
  return; 
}

void
CylTarget::createLinks()
  /*!
    Creates a full attachment set
    First two are in the -/+Y direction and have a divider
    Last two are in the -/+X direction and have a divider
    The mid two are -/+Z direction
  */
{  
  ELog::RegMethod RegA("CylTarget","createLinks");

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  
  FixedComp::setConnect(2,Origin-X*(wallThick+radius/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+17));
  
  FixedComp::setConnect(3,Origin+X*(wallThick+radius/2.0),X);
  FixedComp::setLinkSurf(3,-SMap.realSurf(buildIndex+17));
  
  FixedComp::setConnect(4,Origin-Z*(wallThick+radius/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+17));
  
  FixedComp::setConnect(5,Origin+Z*(wallThick+wallThick+radius/2.0),Z);
  FixedComp::setLinkSurf(5,-SMap.realSurf(buildIndex+17));

  return;
}

void 
CylTarget::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("CylTarget","layerProcess");
  // Steel layers
  //  layerSpecial(System);

  if (nWallLayers>1)
    {
      HeadRule HRa,HRb;
      ModelSupport::surfDivide DA;
            
      for(size_t i=1;i<nWallLayers;i++)
	{
	  DA.addFrac(wallFracList[i-1]);
	  DA.addMaterial(wallMatList[i-1]);
	}
      DA.addMaterial(wallMatList.back());
      // Cell Specific:
      DA.setCellN(CellMap::getCell("Outer"));
      DA.setOutNum(cellIndex,buildIndex+1001);

      ModelSupport::mergeTemplate<Geometry::Cylinder,
				  Geometry::Cylinder> surroundRule;

      surroundRule.setSurfPair(SMap.realSurf(buildIndex+7),
			       SMap.realSurf(buildIndex+17));
      HRa=HeadRule(SMap,buildIndex,7);
      HRb=HeadRule(SMap,buildIndex,-17);

      surroundRule.setInnerRule(HRa);
      surroundRule.setOuterRule(HRb);
      DA.addRule(&surroundRule);
      DA.activeDivideTemplate(System);

      cellIndex=DA.getCellNum();
    }
  return;
}


  
void
CylTarget::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: Attachment point
    \param sideIndex :: sideIndex for link point
   */
{
  ELog::RegMethod RegA("CylTarget","createAll");

  populate(System.getDataBase());
  createCentredUnitVector(FC,sideIndex,length/2.0);

  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  layerProcess(System);
  return;
}

}  // NAMESPACE pipeSystem
