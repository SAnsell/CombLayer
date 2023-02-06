/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderator/HWrapper.cxx
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
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Surface.h"
#include "surfRegister.h"
#include "Quadratic.h"
#include "Plane.h"
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
#include "surfExpand.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "HWrapper.h"

namespace moderatorSystem
{

HWrapper::HWrapper(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(Key,20),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{
  nameSideIndex(6,"FLhydro");
  nameSideIndex(7,"FLhydroNeg");
  nameSideIndex(8,"FLhydroPlus");
  nameSideIndex(9,"FLhydroDown");
  nameSideIndex(10,"FLhydroUp");

  nameSideIndex(11,"VacFront");
  nameSideIndex(12,"VacBack");
  nameSideIndex(13,"VacNeg");
  nameSideIndex(14,"VacPlus");
  nameSideIndex(15,"VacDown");
  nameSideIndex(16,"VacUp");
  nameSideIndex(18,"PreModSurf");
}

HWrapper::HWrapper(const HWrapper& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedComp(A),
  attachSystem::ExternalCut(A),
  attachSystem::CellMap(A),
  wingLen(A.wingLen),vacInner(A.vacInner),alInner(A.alInner),
  alOuter(A.alOuter),vacOuter(A.vacOuter),modTemp(A.modTemp),
  modMat(A.modMat),alMat(A.alMat)
  /*!
    Copy constructor
    \param A :: HWrapper to copy
  */
{}

HWrapper&
HWrapper::operator=(const HWrapper& A)
  /*!
    Assignment operator
    \param A :: HWrapper to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      attachSystem::CellMap::operator=(A);
      wingLen=A.wingLen;
      vacInner=A.vacInner;
      alInner=A.alInner;
      alOuter=A.alOuter;
      vacOuter=A.vacOuter;
      modTemp=A.modTemp;
      modMat=A.modMat;
      alMat=A.alMat;
    }
  return *this;
}

HWrapper::~HWrapper() 
  /*!
    Destructor
  */
{}

void
HWrapper::checkExternalCut()
  /*!
    The class can be constructed either from FixedComp
    or from ExternalCut. If it is done from FixedComp
    the ExternalCut are constructed here.
   */
{
  ELog::RegMethod RegA("HWrapper","checkExternalCut");

  for(const std::string name :
	{ "VacFront", "VacBack", "VacNeg",
	  "VacPlus", "VacDown", "VacUp",
	  "FLhydroNeg","FLhydroPlus",
	  "FLhydroDown","FLhydroUp"})
    {
      if (!ExternalCut::isActive(name))
	ExternalCut::setCutSurf(name,*this,name);  // if not set?
    }
  return;
}
  
void
HWrapper::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase to use
  */
{
  ELog::RegMethod RegA("HWrapper","populate");
  
  modThick=Control.EvalVar<double>(keyName+"ModThick");
  vacInner=Control.EvalVar<double>(keyName+"VacInner");
  vacOuter=Control.EvalVar<double>(keyName+"VacOuter");
  alInner=Control.EvalVar<double>(keyName+"AlInner");
  alOuter=Control.EvalVar<double>(keyName+"VacOuter");

  wingLen=Control.EvalVar<double>(keyName+"WingLen");

  modTemp=Control.EvalVar<double>(keyName+"ModTemp");
  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");
  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");
  
  return;
}
  
void
HWrapper::createSurfaces()
  /*!
    Create All the surfaces
    \param VacFC :: Fixed unit that connects to this moderator
    \param FLine :: FLight line that co-joines
  */
{
  ELog::RegMethod RegA("HWrapper","createSurface");
  
  //  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  //  const masterRotate& MR=masterRotate::Instance();

  const Geometry::Vec3D& HOrg=getLinkPt("FLhydro");	 
  // Note can use the divide surface as a real surface.
  
  double subLen(0.0);
  int BI(buildIndex);
  for(const double layerThick : {alInner,modThick,alOuter,vacOuter})
    {
      subLen+=layerThick;
      ExternalCut::makeExpandedSurf
	(SMap,"VacBack",BI+12,HOrg,wingLen+subLen);
      ExternalCut::makeExpandedSurf
      (SMap,"FLhydroNeg",BI+13,HOrg,subLen);
      ExternalCut::makeExpandedSurf
      (SMap,"FLhydroPlus",BI+14,HOrg,subLen);
      ExternalCut::makeExpandedSurf
      (SMap,"FLhydroDown",BI+15,HOrg,subLen);
      ExternalCut::makeExpandedSurf
      (SMap,"FLhydroUp",BI+16,HOrg,subLen);

      ExternalCut::makeExpandedSurf
      (SMap,"VacBack",BI+112,HOrg,subLen);
      ExternalCut::makeExpandedSurf
      (SMap,"VacNeg",BI+113,HOrg,subLen);
      ExternalCut::makeExpandedSurf
      (SMap,"VacPlus",BI+114,HOrg,subLen);
      ExternalCut::makeExpandedSurf
      (SMap,"VacDown",BI+115,HOrg,subLen);
      BI+=10;
    }
 
  return;
}

void
HWrapper::createObjects(Simulation& System)
		
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("HWrapper","createObjects");

  const HeadRule vacHR=getComplementRule("VacCan");
  const HeadRule FLhydroHR=getFullRule("FLhydro").complement();
  const HeadRule baseCutHR=getRule("BaseCut");
  const HeadRule baseFullCutHR=getComplementRule("BaseFullCut");
  const HeadRule baseFrontCutHR=getRule("BaseFrontCut");
  const HeadRule divHR=getRule("DivideSurf");

  
  HeadRule HR;
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-142 143 -144 145 ");
  addOuterSurf(HR*FLhydroHR*vacHR*baseCutHR*divHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"122 -42 43 -44 45 -46");
  addOuterUnionSurf(HR*FLhydroHR*baseFullCutHR*divHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,
			       "142 -42 43 -44 45 -46 (32:-33:34:-35:36)");
  makeCell("OuterCell",System,cellIndex++,0,0.0,
	   HR*baseFullCutHR*FLhydroHR*divHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,
			       "132 -32 33 -34 35 -36 (22:-23:24:-25:26)");
  makeCell("AlWall",System,cellIndex++,alMat,0.0,
	   HR*FLhydroHR*baseFullCutHR*divHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,
			       "112 -22 23 -24 25 -26(-13:14:-15:16)");
  makeCell("Water",System,cellIndex++,modMat,0.0,HR*baseFullCutHR*divHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-22 13 -14 15 -16");
  makeCell("AlWall",System,cellIndex++,alMat,0.0,
	   HR*FLhydroHR*getRule("VacBack")*divHR);

  // Main moderator

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"(-33:34:-35) -142 143 -144 145 (132:-133:134:-135)");
  makeCell("OuterCell",System,cellIndex++,0,0.0,HR*baseCutHR*divHR);  

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"(-23:24:-25) -132 133 -134 135 (122:-123:124:-125)");
  makeCell("AlWall",System,cellIndex++,alMat,0.0,HR*baseCutHR*divHR);  

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"(22:-23:24:-25) -122 123 -124 125 (112:-113:114:-115)");
  makeCell("Water",System,cellIndex++,modMat,0.0,HR*baseCutHR*divHR);  

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"(12:-13:14:-15:16) -112 113 -114 115");
  makeCell("AlWall",System,cellIndex++,alMat,0.0,HR*baseCutHR*vacHR*divHR);  

  // Extras (because of mis-joining of pre-mod to HWmod):
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-142 122 -46 36");
  makeCell("OuterCell",System,cellIndex++,0,0.0,HR*baseFrontCutHR);  

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-132 122 26 -36");
  makeCell("AlWall",System,cellIndex++,alMat,0.0,HR*baseFrontCutHR);  

  return;
}

void
HWrapper::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed unit that connects to this moderator
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("HWrapper","creatAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  checkExternalCut();
  createSurfaces();
  createObjects(System);
  insertObjects(System);       
  
  return;
}
  
}  // NAMESPACE moderatorSystem
