/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   softimax/M1BackPlate.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include "Exception.h"
#include "surfRegister.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "M1BackPlate.h"

namespace xraySystem
{

M1BackPlate::M1BackPlate(const std::string& Key) :
  attachSystem::FixedRotate(Key,8),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{}

M1BackPlate::~M1BackPlate()
  /*!
    Destructor
   */
{}

void
M1BackPlate::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("M1BackPlate","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  clearGap=Control.EvalVar<double>(keyName+"ClearGap");
  backThick=Control.EvalVar<double>(keyName+"BackThick");
  mainThick=Control.EvalVar<double>(keyName+"MainThick");
  cupHeight=Control.EvalVar<double>(keyName+"CupHeight");
  topExtent=Control.EvalVar<double>(keyName+"TopExtent");
  extentThick=Control.EvalVar<double>(keyName+"ExtentThick");
  baseExtent=Control.EvalVar<double>(keyName+"BaseExtent");

  supVOffset=Control.EvalVar<double>(keyName+"SupVOffset");
  supLength=Control.EvalVar<double>(keyName+"SupLength");
  supXOut=Control.EvalVar<double>(keyName+"SupXOut");
  supThick=Control.EvalVar<double>(keyName+"SupThick");
  supEdge=Control.EvalVar<double>(keyName+"SupEdge");
  supHoleRadius=Control.EvalVar<double>(keyName+"SupHoleRadius");

  baseMat=ModelSupport::EvalMat<int>(Control,keyName+"BaseMat");
  supportMat=ModelSupport::EvalMat<int>(Control,keyName+"SupportMat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");

  return;
}

void
M1BackPlate::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("M1BackPlate","createSurfaces");

  if (!isActive("Top") || !isActive("Base") || !isActive("Back"))
    throw ColErr::InContainerError<std::string>
      ("Top/Base/Back","Surface not defined");

  // mid divider
  ModelSupport::buildPlane(SMap,buildIndex+100,Origin,Z);
  
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*clearGap,X);
  makeShiftedSurf(SMap,"Base",buildIndex+5,Z,-clearGap);
  makeShiftedSurf(SMap,"Top",buildIndex+6,Z,clearGap);

  ModelSupport::buildPlane
    (SMap,buildIndex+104,Origin+X*(topExtent-clearGap),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+204,Origin+X*(baseExtent-clearGap),X);

  // plate layer
  ModelSupport::buildPlane
    (SMap,buildIndex+13,Origin-X*(clearGap+backThick),X);
  makeShiftedSurf(SMap,"Base",buildIndex+15,Z,-(clearGap+mainThick));
  makeShiftedSurf(SMap,"Top",buildIndex+16,Z,clearGap+mainThick);

  // Extent
  makeShiftedSurf(SMap,"Base",buildIndex+25,Z,-(clearGap+cupHeight));
  makeShiftedSurf(SMap,"Top",buildIndex+26,Z,clearGap+cupHeight);
  ModelSupport::buildPlane
    (SMap,buildIndex+124,Origin+X*(topExtent-clearGap-extentThick),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+224,Origin+X*(baseExtent-clearGap-extentThick),X);


  // STOP SURFACE:
  // top
  ModelSupport::buildPlane(SMap,buildIndex+501,Origin+Y*(supLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+502,Origin+Y*(supLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+504,Origin+X*supXOut,X);
  makeShiftedSurf(SMap,"Top",buildIndex+505,Z,supVOffset);
  makeShiftedSurf(SMap,"Top",buildIndex+506,Z,supVOffset+supEdge);

  ModelSupport::buildPlane
    (SMap,buildIndex+513,Origin+X*(supThick+topExtent-clearGap),X);
  ModelSupport::buildPlane(SMap,buildIndex+514,Origin+X*(supXOut-supThick),X);
  makeShiftedSurf(SMap,"Top",buildIndex+515,Z,supVOffset+supThick);
  
  return;
}

void
M1BackPlate::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("M1BackPlate","createObjects");

  const HeadRule topHR=getRule("Top");
  const HeadRule baseHR=getRule("Base");
  const HeadRule backHR=getRule("Back");
  const HeadRule tbUnionHR=topHR+backHR;
  const HeadRule bbUnionHR=baseHR+backHR;
  
  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"100 1 -2 3 -104 -6");
  makeCell("Plate",System,cellIndex++,voidMat,0.0,HR*tbUnionHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"100 1 -2 13 -104 -16 (6:-3)");
  makeCell("Plate",System,cellIndex++,baseMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 16 -26 124 -104");
  makeCell("Plate",System,cellIndex++,baseMat,0.0,HR);
  

  // lower section:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-100 1 -2 3 -204 5");
  makeCell("Plate",System,cellIndex++,voidMat,0.0,HR*bbUnionHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-100 1 -2 13 -204 15 (-5:-3)");
  makeCell("Plate",System,cellIndex++,baseMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -15 25 224 -204");
  makeCell("Plate",System,cellIndex++,baseMat,0.0,HR);
  

  // support:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"501 -502 104 -504 505 -506");
  makeCell("TopSupport",System,cellIndex++,supportMat,0.0,HR);
  addOuterUnionSurf(HR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"100 1 -2 13 -104 -16");
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 16 -26 124 -104");
  addOuterUnionSurf(HR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-100 1 -2 13 -204 15");
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -15 25 224 -204");
  addOuterUnionSurf(HR);

  return;
}

void
M1BackPlate::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("M1BackPlate","createLinks");

  // link points are defined in the end of createSurfaces

  return;
}

void
M1BackPlate::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("M1BackPlate","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
