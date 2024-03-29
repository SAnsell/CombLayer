/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t2Build/OrthoInsert.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include "FixedUnit.h"
#include "FixedRotate.h"
#include "ExternalCut.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "Groove.h"
#include "Hydrogen.h"
#include "OrthoInsert.h"

namespace moderatorSystem
{

OrthoInsert::OrthoInsert(const std::string& Key)  :
  attachSystem::ContainedGroup("GSide","HSide"),
  attachSystem::FixedUnit(Key,0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

OrthoInsert::OrthoInsert(const OrthoInsert& A) : 
  attachSystem::ContainedGroup(A),
  attachSystem::FixedUnit(A),
  attachSystem::ExternalCut(A),
  attachSystem::CellMap(A),
  GCent(A.GCent),grooveThick(A.grooveThick),grooveWidth(A.grooveWidth),
  grooveHeight(A.grooveHeight),HCent(A.HCent),
  HRadius(A.HRadius),hydroThick(A.hydroThick),
  orthoTemp(A.orthoTemp),orthoMat(A.orthoMat)
  /*!
    Copy constructor
    \param A :: OrthoInsert to copy
  */
{}

OrthoInsert&
OrthoInsert::operator=(const OrthoInsert& A)
  /*!
    Assignment operator
    \param A :: OrthoInsert to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::FixedComp::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      attachSystem::CellMap::operator=(A);
      GCent=A.GCent;
      grooveThick=A.grooveThick;
      grooveWidth=A.grooveWidth;
      grooveHeight=A.grooveHeight;
      HCent=A.HCent;
      HRadius=A.HRadius;
      hydroThick=A.hydroThick;
      orthoTemp=A.orthoTemp;
      orthoMat=A.orthoMat;
    }
  return *this;
}


OrthoInsert::~OrthoInsert() 
  /*!
    Destructor
  */
{}

void
OrthoInsert::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("OrthoInsert","populate");
  
  grooveThick=Control.EvalVar<double>(keyName+"GrooveThick");
  grooveWidth=Control.EvalVar<double>(keyName+"GrooveWidth");
  grooveHeight=Control.EvalVar<double>(keyName+"GrooveHeight");
  hydroThick=Control.EvalVar<double>(keyName+"HydroThick");

  orthoTemp=Control.EvalDefVar<double>(keyName+"OrthoTemp",-1.0);
  orthoMat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}
  

void
OrthoInsert::setModeratorCentres(const Hydrogen& HUnit,
				 const Groove& GUnit)
/*!
    Create the unit vectors
    - Y Points down the OrthoInsert direction
    - X Across the OrthoInsert
    - Z up (towards the target)
    \param HUnit :: Fixed unit that it is connected to 
    \param GUnit :: Fixed unit that it is connected to 
  */
{
  ELog::RegMethod RegA("OrthoInsert","createUnitVector");

  FixedComp::createUnitVector(HUnit);
  GCent=GUnit.getBackGroove()+Y*HUnit.getAlDivide();
  HCent=HUnit.getHCentre();
  HRadius=HUnit.getRadius();

  return;
}
  
void
OrthoInsert::createSurfaces(const Hydrogen& HC)
  /*!
    Create All the surfaces
    \param HC :: Fixed Component [size 6] 
  */
{
  ELog::RegMethod RegA("OrthoInsert","createSurface");

  // NOTE: Surfaces can be used in the noraml +/- manor
  int signVal(-1);
  for(int i=1;i<7;i++)
    {
      SMap.addMatch(buildIndex+i,signVal*HC.getLinkSurf(i));
      signVal*=-1;
    }

  ModelSupport::buildPlane(SMap,buildIndex+11,GCent+Y*grooveThick,Y);
  
  ModelSupport::buildCylinder(SMap,buildIndex+12,
			      HCent-Y*hydroThick,Z,HRadius);

  ModelSupport::buildPlane(SMap,buildIndex+13,GCent-X*grooveWidth/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+14,GCent+X*grooveWidth/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+15,GCent-Z*grooveHeight/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,GCent+Z*grooveHeight/2.0,Z);

  return;
}

void
OrthoInsert::createObjects(Simulation& System)
  /*!
    Adds the orthorgonal
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("OrthoInsert","createObjects");

  const HeadRule& mainCellHR=getRule("HCell");

  HeadRule HR;
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"12 13 -14 15 -16");
  addOuterSurf("HSide",HR);
  
  makeCell("HydroH2",System,cellIndex++,orthoMat,orthoTemp,
	   HR*mainCellHR);
  // Groove side
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-11 (-13:14:-15:16) ");
  addOuterSurf("GSide",HR); 

  makeCell("GrooveH2",System,cellIndex++,orthoMat,orthoTemp,
	   HR*mainCellHR);
  
  return;
}
  
void
OrthoInsert::build(Simulation& System,
		       const Hydrogen& HUnit,
		       const Groove& GUnit)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param HUnit :: Fixed Base unit [Hydrogen]
    \param GUnit :: Fixed Base unit [Groove]
  */
{
  ELog::RegMethod RegA("OrthoInsert","createAll");

  setCutSurf("HCell",HUnit.getCellHR(System,"HCell"));
  populate(System.getDataBase());

  setModeratorCentres(HUnit,GUnit);
  createSurfaces(HUnit);
  createObjects(System);
  addAllInsertCell(HUnit.getCell("HCell"));
  insertObjects(System);       
  
  return;
}
  
}  // NAMESPACE moderatorSystem
