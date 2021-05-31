/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderator/OrthoInsert.cxx
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
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedUnit.h"
#include "FixedOffset.h"
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
  attachSystem::ContainedGroup(A),attachSystem::FixedUnit(A),
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
OrthoInsert::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("OrthoInsert","populate");
  
  const FuncDataBase& Control=System.getDataBase();

  grooveThick=Control.EvalVar<double>(keyName+"GrooveThick");
  grooveWidth=Control.EvalVar<double>(keyName+"GrooveWidth");
  grooveHeight=Control.EvalVar<double>(keyName+"GrooveHeight");
  hydroThick=Control.EvalVar<double>(keyName+"HydroThick");

  orthoTemp=Control.EvalDefVar<double>(keyName+"OrthoTemp",-1.0);
  orthoMat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}
  

void
OrthoInsert::createUnitVector(const Hydrogen& HUnit,
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
OrthoInsert::createObjects(Simulation& System,
			   const attachSystem::ContainedComp& CU)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
    \param CU :: Contained unit				
  */
{
  ELog::RegMethod RegA("OrthoInsert","createObjects");
  
  std::string Out;  
  Out=ModelSupport::getComposite(SMap,buildIndex," 12 13 -14 15 -16 ");
  addOuterSurf("HSide",Out); 
  Out+=CU.getContainer();
  System.addCell(MonteCarlo::Object(cellIndex++,orthoMat,orthoTemp,Out));
  // Groove side
  Out=ModelSupport::getComposite(SMap,buildIndex," -11 (-13:14:-15:16) ");
  addOuterSurf("GSide",Out); 
  Out+=CU.getContainer();
  System.addCell(MonteCarlo::Object(cellIndex++,orthoMat,orthoTemp,Out));
  
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

  populate(System);

  createUnitVector(HUnit,GUnit);
  createSurfaces(HUnit);
  createObjects(System,HUnit);
  addAllInsertCell(HUnit.getMainBody());
  insertObjects(System);       
  
  return;
}
  
}  // NAMESPACE moderatorSystem
