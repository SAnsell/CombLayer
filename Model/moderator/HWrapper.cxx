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
#include "FixedUnit.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "HWrapper.h"

namespace moderatorSystem
{

HWrapper::HWrapper(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedUnit(Key,16),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  divideSurf(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{
  nameSideIndex(7,"FLhydroNeg");
  nameSideIndex(8,"FLhydroPlus");
  nameSideIndex(9,"FLhydroDown");
  nameSideIndex(10,"FLhydroUp");
}

HWrapper::HWrapper(const HWrapper& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedUnit(A),
  attachSystem::ExternalCut(A),
  attachSystem::CellMap(A),
  divideSurf(A.divideSurf),sideExt(A.sideExt),
  heightExt(A.heightExt),backExt(A.backExt),forwardExt(A.forwardExt),
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
      divideSurf=A.divideSurf;
      sideExt=A.sideExt;
      heightExt=A.heightExt;
      backExt=A.backExt;
      forwardExt=A.forwardExt;
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

  for(const std::string name : {"FLhydroNeg","FLhydroPlus",
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
  
  sideExt=Control.EvalVar<double>(keyName+"SideExt");
  heightExt=Control.EvalVar<double>(keyName+"HeightExt");
  backExt=Control.EvalVar<double>(keyName+"BackExt");
  forwardExt=Control.EvalVar<double>(keyName+"ForwardExt");

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
  const int NSurf(10);
  //  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  //  const masterRotate& MR=masterRotate::Instance();

  // Number of surfaces are required :: 
  //  Base/Top/Sides/Divider/CylinderFront.
  //   5  / 6 / 3,4 /   1   /   2 

  // Note can use the divide surface as a real surface.
  SMap.addMatch(buildIndex+1,divideSurf);

  
  const double sideWater=sideExt-(alInner+vacOuter+vacInner);
  const double topWater=heightExt-(alOuter+alInner+vacOuter+vacInner);
  //  const double frontWater=forwardExt-(alOuter+alInner+vacOuter+vacInner);
  
  ExternalCut::makeExpandedSurf
    (SMap,"FLhydroNeg",buildIndex+13,Origin,sideExt);
  ExternalCut::makeExpandedSurf
    (SMap,"FLhydroPlus",buildIndex+14,Origin,sideExt);
  ExternalCut::makeExpandedSurf
    (SMap,"FLhydroDown",buildIndex+15,Origin,sideExt);
  ExternalCut::makeExpandedSurf
    (SMap,"FLhydroUp",buildIndex+16,Origin,sideExt);

  ExternalCut::makeExpandedSurf
    (SMap,"FLhydroNeg",buildIndex+23,Origin,sideExt+vacOuter);
  ELog::EM<<"Real Surf == "<<
    *(SMap.realSurfPtr(buildIndex+13))<<ELog::endDiag;
  ELog::EM<<"Real Surf == "<<
    *(SMap.realSurfPtr(buildIndex+23))<<ELog::endDiag;
  /*  
  const int baseSurf[NSurf]={1,2,3,4,5,6,103,104,105,106};
  double mainOffset[NSurf]={backExt,forwardExt,-sideExt,sideExt,
			    -heightExt,0.0,0,0,0,0};
  const int signDir[NSurf]={-1,1,1,1,1,1,1,1,1,1};
  const int addDir[NSurf]={1,1,-1,1,-1,1,1,-1,1,-1};
  // CREATE GLOBAL:
  createSurfMesh(NSurf,10,baseSurf,mainOffset,signDir);  

  // CREATE OUTER VAC  
  for(int i=0;i<NSurf-4;i++)
    mainOffset[i]-=addDir[i]*vacOuter;
  createSurfMesh(NSurf,20,baseSurf,mainOffset,signDir);  

  // CREATE OUTER AL
  for(int i=0;i<NSurf;i++)
    mainOffset[i]-=addDir[i]*alOuter;
  createSurfMesh(NSurf,30,baseSurf,mainOffset,signDir);  

  // CREATE Water LAYER: Complex:
  for(int i=0;i<NSurf-4;i++)
    mainOffset[i]=addDir[i]*(alInner+vacInner);
  int idx=NSurf-4;
  mainOffset[idx]= -addDir[idx]*(sideWater+alInner); idx++;
  mainOffset[idx]= -addDir[idx]*(sideWater+alInner); idx++;
  mainOffset[idx]= -addDir[idx]*(topWater+alInner); idx++;
  mainOffset[idx]= -addDir[idx]*(topWater+alInner); 
  createSurfMesh(NSurf,40,baseSurf,mainOffset,signDir);  

  // CREATE AL inner LAYER: 
  for(int i=0;i<NSurf-4;i++)
    mainOffset[i]=addDir[i]*vacInner;
  for(int i=NSurf-4;i<NSurf;i++)
    mainOffset[i]-=addDir[i]*alOuter;
  createSurfMesh(NSurf,50,baseSurf,mainOffset,signDir);  

  // Just the outer values:
  for(int i=NSurf-4;i<NSurf;i++)
    mainOffset[i]-=addDir[i]*alOuter;
  createSurfMesh(4,60,baseSurf+NSurf-4,
		 mainOffset+NSurf-4,signDir+NSurf-4);  

  // Wing extension:
  
  // Full length
  Geometry::Surface* SX;
  SX=ModelSupport::surfaceCreateExpand
    (SMap.realSurfPtr(buildIndex+2),wingLen);
  SX->setName(buildIndex+202);
  SMap.registerSurf(buildIndex+202,SX);

  SX=ModelSupport::surfaceCreateExpand
    (SMap.realSurfPtr(buildIndex+2),wingLen-vacOuter);
  SX->setName(buildIndex+212);
  SMap.registerSurf(buildIndex+212,SX);

  SX=ModelSupport::surfaceCreateExpand
    (SMap.realSurfPtr(buildIndex+2),wingLen-(alOuter+vacOuter));
  SX->setName(buildIndex+222);
  SMap.registerSurf(buildIndex+222,SX);
  */  
  return;
}

void
HWrapper::createObjects(Simulation& System,
			const attachSystem::ContainedComp& CM)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
    \param CM :: Basic Pre-moderator [to avoid clipping]
  */
{
  ELog::RegMethod RegA("HWrapper","createObjects");

  const HeadRule vacHR=getComplementRule("VacCan");
  const HeadRule FLhydroHR=getComplementRule("FLhydro");
  const HeadRule baseCutHR=getComplementRule("BaseCut");
  
  HeadRule HR;
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,
     "(11 -12 13 -14 15 -6) :"
     "1 12 163 -164 165 -166 -202");
  addOuterSurf(HR);

  // Out space
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
			       "11 -12 13 -14 15 -6 "
				 "(-23 : 24 : 22 : -25 : -21 : 26)"
				 "( -31 : -153 : 154 : -155 : 156) ");
  System.addCell(cellIndex++,0,0.0,HR*vacHR.complement());

  // Al skin
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"21 -22 23 -24 25 -26 "
			       " (52 : -53 : 54 : -55 ) "
			       "(-33 : 34 : 32 : -35 : -31 : 36)"
			       "( -1 : -143 : 144 : -145 : 146) ");
  makeCell("AlSkin",System,cellIndex++,alMat,0.0,HR);

  // Al EXTRA [26 > 156]
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 2 -22 153 -154 26 -156");
  makeCell("AlSkin",System,cellIndex++,alMat,modTemp,HR);
  
  // Al EXTRA [36 > 146]
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 52 -42 143 -144 36 -146");
  makeCell("AlSkin",System,cellIndex++,alMat,modTemp,HR);

  // Water
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"31 -32 33 -34 35 -36 "
				 "(-43 : 44 : 42 : -45 : -41 )"
				 "( -1 : -133 : 134 : -135 : 136) ");
  makeCell("Water",System,cellIndex++,modMat,modTemp,HR*vacHR);

  // Water EXTRA [36 > 146]
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 42 -32 143 -144 36 -146 ");
  System.addCell(MonteCarlo::Object(cellIndex++,modMat,modTemp,HR));

  // AL Inner
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"31 -42 43 -44 45 -36 "
			       "(-53 : 54 : 52 : -55 : -51 : 56) ");
  //  				 "( -1 : -103 : 104 : -105 : 106) ");
  makeCell("AlInner",System,cellIndex++,alMat,modTemp,HR*vacHR*FLhydroHR);


  // Vac inner
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"21 -52 53 -54 55 -26 "
			       "( -1 : -103 : 104 : -105 : 106) ");
  makeCell("VacInner",System,cellIndex++,0,0.0,HR*vacHR);

  // WINGS:

  // Al inner
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 42 133 -134 135 -136 -212 ");
  makeCell("AlWing",System,cellIndex++,alMat,modTemp,HR*FLhydroHR);

  // Water inner
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 32 143 -144 145 -146 -222 "
				" (-133 : 134 : -135 : 136) ");
  makeCell("WaterWing",System,cellIndex++,modMat,modTemp,HR);

  // outer Al
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 22 153 -154 155 -156 -212 "
				 " (-143 : 144 : 222 : -145 : 146) "
				 " (-133 : 134 : -135 : 136) ");
  makeCell("OuterAl",System,cellIndex++,alMat,modTemp,HR);

  // outer Vac
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
			       "1 12 163 -164 165 -166 -202 "
			       "(-153 : 154 : 212 : -155 : 156) ");
  makeCell("OuterVac",System,cellIndex++,0,0.0,HR*baseCutHR*FLhydroHR);

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
  //  createSurfaces(vacFC,FLine);
  //  createObjects(System,CM);
  insertObjects(System);       
  
  return;
}
  
}  // NAMESPACE moderatorSystem
