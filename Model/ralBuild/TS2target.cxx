/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:  ralBuild/TS2target.cxx
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
#include "Quaternion.h"
#include "surfRegister.h"
#include "surfDivide.h"
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
#include "FixedRotate.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "ProtonVoid.h"
#include "TargetBase.h"
#include "TS2target.h"


namespace TMRSystem
{

TS2target::TS2target(const std::string& Key) :
  TMRSystem::TargetBase(Key,5)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

TS2target::TS2target(const TS2target& A) : 
  TMRSystem::TargetBase(A),
  mainLength(A.mainLength),coreRadius(A.coreRadius),
  surfThick(A.surfThick),wSphDisplace(A.wSphDisplace),
  wSphRadius(A.wSphRadius),wPlaneCut(A.wPlaneCut),
  wPlaneAngle(A.wPlaneAngle),cladThick(A.cladThick),
  taSphDisplace(A.taSphDisplace),taSphRadius(A.taSphRadius),
  taPlaneCut(A.taPlaneCut),waterThick(A.waterThick),
  pressureThick(A.pressureThick),xFlowCyl(A.xFlowCyl),
  xFlowOutRadius(A.xFlowOutRadius),xFlowOutDisplace(A.xFlowOutDisplace),
  xFlowOutMidRadius(A.xFlowOutMidRadius),xFlowOutCutAngle(A.xFlowOutCutAngle),
  xFlowOPlaneCut(A.xFlowOPlaneCut),xFlowInnerRadius(A.xFlowInnerRadius),
  xFlowInnerDisplace(A.xFlowInnerDisplace),xFlowIPlaneCut(A.xFlowIPlaneCut),
  tCapInnerRadius(A.tCapInnerRadius),tCapOuterRadius(A.tCapOuterRadius),
  tCapDisplace(A.tCapDisplace),voidRadius(A.voidRadius),
  flangeThick(A.flangeThick),flangeLen(A.flangeLen),
  flangeRadius(A.flangeRadius),flangeClear(A.flangeClear),
  flangeYStep(A.flangeYStep),flangeMat(A.flangeMat),
  wMat(A.wMat),taMat(A.taMat),waterMat(A.waterMat),
  targetTemp(A.targetTemp),waterTemp(A.waterTemp),
  externTemp(A.externTemp),
  nLayers(A.nLayers),mainFrac(A.mainFrac)
  /*!
    Copy constructor
    \param A :: TS2target to copy
  */
{}

TS2target&
TS2target::operator=(const TS2target& A)
  /*!
    Assignment operator
    \param A :: TS2target to copy
    \return *this
  */
{
  if (this!=&A)
    {
      TMRSystem::TargetBase::operator=(A);
      mainLength=A.mainLength;
      coreRadius=A.coreRadius;
      surfThick=A.surfThick;
      wSphDisplace=A.wSphDisplace;
      wSphRadius=A.wSphRadius;
      wPlaneCut=A.wPlaneCut;
      wPlaneAngle=A.wPlaneAngle;
      cladThick=A.cladThick;
      taSphDisplace=A.taSphDisplace;
      taSphRadius=A.taSphRadius;
      taPlaneCut=A.taPlaneCut;
      waterThick=A.waterThick;
      pressureThick=A.pressureThick;
      xFlowCyl=A.xFlowCyl;
      xFlowOutRadius=A.xFlowOutRadius;
      xFlowOutDisplace=A.xFlowOutDisplace;
      xFlowOutMidRadius=A.xFlowOutMidRadius;
      xFlowOutCutAngle=A.xFlowOutCutAngle;
      xFlowOPlaneCut=A.xFlowOPlaneCut;
      xFlowInnerRadius=A.xFlowInnerRadius;
      xFlowInnerDisplace=A.xFlowInnerDisplace;
      xFlowIPlaneCut=A.xFlowIPlaneCut;
      tCapInnerRadius=A.tCapInnerRadius;
      tCapOuterRadius=A.tCapOuterRadius;
      tCapDisplace=A.tCapDisplace;
      voidRadius=A.voidRadius;
      flangeThick=A.flangeThick;
      flangeLen=A.flangeLen;
      flangeRadius=A.flangeRadius;
      flangeClear=A.flangeClear;
      flangeYStep=A.flangeYStep;
      flangeMat=A.flangeMat;
      wMat=A.wMat;
      taMat=A.taMat;
      waterMat=A.waterMat;
      targetTemp=A.targetTemp;
      waterTemp=A.waterTemp;
      externTemp=A.externTemp;
      nLayers=A.nLayers;
      mainFrac=A.mainFrac;
    }
  return *this;
}

TS2target*
TS2target::clone() const
  /*!
    Clone funciton
    \return new(this)
  */
{
  return new TS2target(*this);
}

TS2target::~TS2target() 
  /*!
    Destructor
  */
{}

void
TS2target::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("TS2target","populate");

  attachSystem::FixedRotate::populate(Control);
  
  mainLength=Control.EvalVar<double>(keyName+"MainLength");
  coreRadius=Control.EvalVar<double>(keyName+"CoreRadius");
  surfThick=Control.EvalVar<double>(keyName+"SurfThick");
  wSphDisplace=Control.EvalVar<double>(keyName+"wSphDisplace");
  wSphRadius=Control.EvalVar<double>(keyName+"wSphRadius");
  wPlaneCut=Control.EvalVar<double>(keyName+"wPlaneCut");
  wPlaneAngle=Control.EvalVar<double>(keyName+"wPlaneAngle");

  cladThick=Control.EvalVar<double>(keyName+"CladThick");
  taSphDisplace=Control.EvalVar<double>(keyName+"TaSphDisplace");
  taSphRadius=Control.EvalVar<double>(keyName+"TaSphRadius");
  taPlaneCut=Control.EvalVar<double>(keyName+"TaPlaneCut");
  
  waterThick=Control.EvalVar<double>(keyName+"WaterThick");
  pressureThick=Control.EvalVar<double>(keyName+"PressureThick");
  voidRadius=Control.EvalVar<double>(keyName+"BoreRadius");

  xFlowCyl=Control.EvalVar<double>(keyName+"XFlowCyl");
  xFlowOutDisplace=Control.EvalVar<double>(keyName+"XFlowOutDisplace");
  xFlowOutRadius=Control.EvalVar<double>(keyName+"XFlowOutRadius");

  xFlowInnerRadius=Control.EvalVar<double>(keyName+"XFlowInnerRadius");
  xFlowInnerDisplace=Control.EvalVar<double>(keyName+"XFlowInnerDisplace");
  xFlowIPlaneCut=Control.EvalVar<double>(keyName+"XFlowIPlaneCut");
  
  xFlowOutMidRadius=Control.EvalVar<double>(keyName+"XFlowOutMidRadius");
  xFlowOutCutAngle=Control.EvalVar<double>(keyName+"XFlowOutCutAngle");
  xFlowOPlaneCut=Control.EvalVar<double>(keyName+"XFlowOPlaneCut");

  tCapInnerRadius=Control.EvalVar<double>(keyName+"TCapInnerRadius");
  tCapOuterRadius=Control.EvalVar<double>(keyName+"TCapOuterRadius");
  tCapDisplace=Control.EvalVar<double>(keyName+"TCapDisplace");

  flangeThick=Control.EvalVar<double>(keyName+"FlangeThick");
  flangeLen=Control.EvalVar<double>(keyName+"FlangeLen");
  flangeClear=Control.EvalVar<double>(keyName+"FlangeClear");
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeYStep=Control.EvalVar<double>(keyName+"FlangeYStep");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");

  wMat=ModelSupport::EvalMat<int>(Control,keyName+"WMat");
  taMat=ModelSupport::EvalMat<int>(Control,keyName+"TaMat");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");

  targetTemp=Control.EvalVar<double>(keyName+"TargetTemp");
  waterTemp=Control.EvalVar<double>(keyName+"WaterTemp");
  externTemp=Control.EvalVar<double>(keyName+"ExternTemp");
  
  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");

  return;
}

void
TS2target::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("TS2target","createSurface");
  

  // OUTER VOID [Should be a copy of 11 ?]
  ModelSupport::buildCylinder(SMap,buildIndex+101,Origin,Y,voidRadius);

  // front face [plane]
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  // back face [plane]
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*mainLength,Y);
  // Basis cylinder
  
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,coreRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+8,
			      Origin-Y*wSphDisplace,Y,wSphRadius);
  if (surfThick>Geometry::zeroTol && nLayers>1)
    {
      ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,coreRadius-
				  surfThick);
      ModelSupport::buildCylinder(SMap,buildIndex+108,
				  Origin-Y*wSphDisplace,Y,
				  wSphRadius-surfThick);
    }

  // Top Edge Cylinder [used sphere radius!!]
  
  // FLANGE [Not moved with target]:
  ModelSupport::buildPlane(SMap,buildIndex+201,
			   Origin+Y*(mainLength-flangeLen/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+202,
			   Origin+Y*(mainLength+flangeLen/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+211,
			   Origin+Y*(mainLength-flangeYStep),Y);
  ModelSupport::buildPlane(SMap,buildIndex+212,
			   Origin+Y*(mainLength+
				     flangeThick+flangeYStep),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+207,
			      Origin,Y,flangeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+217,
			      Origin,Y,flangeRadius-flangeClear);

  // NOSE CONE:
  // Top Sphere
  ModelSupport::buildSphere(SMap,buildIndex+9,
			    Origin-Y*wSphDisplace,wSphRadius);
  // Top Sphere [with layer]
  ModelSupport::buildSphere(SMap,buildIndex+109,
			    Origin-Y*wSphDisplace,wSphRadius-surfThick);
  // Sphere Mid plane
  ModelSupport::buildPlane(SMap,buildIndex+19,
			   Origin-Y*wSphDisplace,Y);

  // Cut Planes:
  const Geometry::Quaternion QR=
    Geometry::Quaternion::calcQRotDeg(wPlaneAngle,Y);
  const Geometry::Quaternion antiQR=
    Geometry::Quaternion::calcQRotDeg(-wPlaneAngle+90.0,Y);
  Geometry::Vec3D PX(Z);
  Geometry::Vec3D PZ(Z);
  antiQR.rotate(PX);
  QR.rotate(PZ);

  // OuterSphere Cut Plane
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-PZ*wPlaneCut,PZ);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+PZ*wPlaneCut,PZ);

  if (surfThick>Geometry::zeroTol && nLayers>1)
    {
      ModelSupport::buildPlane(SMap,buildIndex+103,
			       Origin-PZ*(wPlaneCut-surfThick),PZ);
      ModelSupport::buildPlane(SMap,buildIndex+104,
			       Origin+PZ*(wPlaneCut-surfThick),PZ);
    }
  // Ta cladding [Inner]
  // Basic Cylinder
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,
			      coreRadius+cladThick);
  
  // Top Sphere : 
  ModelSupport::buildSphere(SMap,buildIndex+29,
			    Origin-Y*taSphDisplace,taSphRadius);

  // Sphere Mid plane
  ModelSupport::buildPlane(SMap,buildIndex+39,Origin-Y*taSphDisplace,Y);
  
  // Cut Planes:
  ModelSupport::buildPlane(SMap,buildIndex+23,Origin-PZ*taPlaneCut,PZ);
  ModelSupport::buildPlane(SMap,buildIndex+24,Origin+PZ*taPlaneCut,PZ);

  // --  WATER LAYER:  [40-60] -- 
  // cylinder water
  ModelSupport::buildCylinder(SMap,buildIndex+47,Origin,Y,
			      coreRadius+cladThick+waterThick);

  // pressure cylinder
  ModelSupport::buildCylinder(SMap,buildIndex+57,
			      Origin,Y,
			      coreRadius+cladThick+waterThick+pressureThick);
  
  // CrossFlow Outer:

  // Base cylinder 
  ModelSupport::buildCylinder(SMap,buildIndex+48,Origin,Y,xFlowCyl);
 
  // OuterSphere Cut Plane
  ModelSupport::buildPlane(SMap,buildIndex+59,Origin-Y*xFlowOutDisplace,Y);

  // OuterCut Sphere [60-70]
  ModelSupport::buildSphere(SMap,buildIndex+61,
			    Origin-Y*xFlowOutDisplace,xFlowOutMidRadius);


  // Base cut cylinder
  ModelSupport::buildCylinder(SMap,buildIndex+62,
			      Origin,Y,xFlowOutMidRadius);    

  ModelSupport::buildPlane(SMap,buildIndex+63,
			   Origin-PX*xFlowOPlaneCut,PX);
  ModelSupport::buildPlane(SMap,buildIndex+64,
			   Origin+PX*xFlowIPlaneCut,PX);
  
  // Inner water cut:
  ModelSupport::buildSphere(SMap,buildIndex+71,
			    Origin-Y*xFlowInnerDisplace,xFlowInnerRadius);

  ModelSupport::buildPlane(SMap,buildIndex+73,
			   Origin-PZ*xFlowIPlaneCut,PZ);
  ModelSupport::buildPlane(SMap,buildIndex+74,
			   Origin+PZ*xFlowIPlaneCut,PZ);


  // TopCap [80-90]
  ModelSupport::buildSphere(SMap,buildIndex+81,
			    Origin-Y*tCapDisplace,tCapInnerRadius);

  // TopCap Cut Plane
  ModelSupport::buildPlane(SMap,buildIndex+85,Origin-Y*tCapDisplace,Y);
  
  // Base cut cyclinder
  ModelSupport::buildCylinder(SMap,buildIndex+82,Origin,Y,tCapInnerRadius);
  ModelSupport::buildSphere(SMap,buildIndex+91,
			    Origin-Y*tCapDisplace,tCapOuterRadius);

  return;
}

void
TS2target::createNoseConeObjects(Simulation& System)
  /*!
    Builds just the nose cone objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("TS2target","createObjects");

  HeadRule HR;
  // Front piece [Cylinder/Sphere]
  if (surfThick>Geometry::zeroTol && nLayers>1)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,
				     "-9 -19 3 -4 -8 (109:-103:104:108)");
      makeCell("InnerWater",System,cellIndex++,waterMat,0.0,HR);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-109 -19 103 -104 -108");
      makeCell("InnerWater",System,cellIndex++,waterMat,0.0,HR);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,
				     "-1 -8  19 3 -4 (108:-103:104)");
      makeCell("InnerWater",System,cellIndex++,waterMat,0.0,HR);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1 -108  19 103 -104");
      makeCell("InnerWater",System,cellIndex++,waterMat,0.0,HR);

    }
  else
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1 -8 (-9 : 19) 3 -4");
      makeCell("InnerWater",System,cellIndex++,waterMat,0.0,HR);
    }

  // ----------------- WATER -------------------------------
  // Water Cut Top: 
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-81 61 63 -64 -59 (29:24)");
  makeCell("InnerWater",System,cellIndex++,waterMat,0.0,HR);

  //  ModelSupport::getExclude(cellIndex-1);
  const HeadRule watOCapHR=HR.complement();

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-48 62 63 -64 -1 59");
  makeCell("InnerWater",System,cellIndex++,waterMat,0.0,HR);
  const HeadRule watICylHR=HR.complement();

  // Inner Water Cut:
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 71 -29 73 -74 -59");
  makeCell("InnerWater",System,cellIndex++,waterMat,0.0,HR);
  const HeadRule watICapHR=HR.complement();

  // Tungsen Cap [Part Sphere]
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-1 23 -24 (-27:-59) (-29: 59) "
     "(-3 : 4 : 8 : ( 9 -19 ) ) ");

  makeCell("Cap",System,cellIndex++,taMat,0.0,HR*watICapHR*watICylHR);
  // Cut: [Top section]
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1 59 -48 (27 : -23 : 24)");

  makeCell("CutSection",System,cellIndex++,taMat,0.0,HR*watICylHR);
  // Cut: [Top section/Part 2]
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
			       "-59 -81 (-23 : 24 : ( 29 -39 ) ) ");
                                 
  makeCell("CutSection",System,cellIndex++,taMat,0.0,HR*watICapHR*watOCapHR);

  // Cap Base:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1 -57 59 48");
  makeCell("CapBase",System,cellIndex++,taMat,0.0,HR);
  // Cap Top:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-59 -91 81");
  makeCell("TopCap",System,cellIndex++,taMat,0.0,HR);


  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1 59 57 -101");
  makeCell("TopCapVoid",System,cellIndex++,0,0.0,HR);
  

  return;
}

void
TS2target::createObjects(Simulation& System)
  /*!
    Builds the target
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("TS2target","createObjects");

  // INNER PLANES    
  //  const HeadRule& frontPlateHR(ExternalCut::getRule("FrontPlate"));
  const HeadRule& backPlateHR(ExternalCut::getRule("BackPlate"));

  HeadRule HR;
  // Main cylinder:
  if (surfThick>Geometry::zeroTol && nLayers>1)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -17");
      makeCell("MainCell",System,cellIndex++,wMat,0.0,HR);
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -7 17");
      makeCell("MainCellSkin",System,cellIndex++,wMat,0.0,HR);
    }
  else
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -7");
      makeCell("MainCell",System,cellIndex++,wMat,0.0,HR);
    }


  // ----------------- FLANGE ----------------
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"201 -202 -207 101");
  makeCell("Flange",System,cellIndex++,0,0.0,HR);


    
  // -- WATER -- [Main Cylinder]
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 27 -47");
  makeCell("Water",System,cellIndex++,waterMat,0.0,HR);

  // -----------------------------------------------------------
  // Main Cylinder
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 7 -27");
  makeCell("SkinCell",System,cellIndex++,taMat,0.0,HR);

  // Ta Press: [Cylinder]
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 47 -57");
  makeCell("TaPressure",System,cellIndex++,taMat,0.0,HR);

  // Spacer Void around target:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 57 -101");
  makeCell("Spacer",System,cellIndex++,0,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 -101");
  makeCell("Spacer",System,cellIndex++,0,0.0,HR*backPlateHR);
    
  // Set EXCLUDE:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
			       "(59 :-91) -207 (-101 : (201 -202))");
  addOuterSurf(HR);

  return;
}

void
TS2target::createLinks()
  /*!
    Create all the links
    \todo swap link 0 to be link 2 
  */
{
  // all point out
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+101));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  // If not nose cone then change 2
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+91));
  FixedComp::addLinkSurf(2,-SMap.realSurf(buildIndex+59));

  FixedComp::setLinkSurf(3,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(0,Origin+Z*voidRadius,Z);
  FixedComp::setConnect(1,Origin+Y*mainLength,Y);
  FixedComp::setConnect(2,Origin-Y*(tCapDisplace+tCapOuterRadius),-Y);
  FixedComp::setConnect(3,Origin,-Y);

  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+27));
  FixedComp::setConnect(4,Origin+Z*(coreRadius+cladThick),Z);

  FixedComp::nameSideIndex(4,"CoreRadius");
  FixedComp::nameSideIndex(2,"TargetFront");
  return;
}

void 
TS2target::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("TS2target","LayerProcess");
  

  // Steel layers
  if (nLayers>1)
    {
      ModelSupport::surfDivide DA;
      
      // Generic [uniform fractions]
      const double frac(1.0/static_cast<double>(nLayers));
      for(size_t i=0;i<static_cast<size_t>(nLayers-1);i++)
	{
	  DA.addFrac(frac*(static_cast<double>(i)+1.0));
	  DA.addMaterial(wMat);
	}
      DA.addMaterial(wMat);
      
      // Cell Specific:
      DA.setCellN(getCell("MainCell",0));
      DA.setOutNum(cellIndex,buildIndex+801);
      DA.makeTemplate<Geometry::Plane>(SMap.realSurf(buildIndex+1),
				   SMap.realSurf(buildIndex+2));
      DA.activeDivide(System);
      cellIndex=DA.getCellNum();
    }
  return;
}

void
TS2target::addProtonLine(Simulation& System)
  /*!
    Add a proton void cell
    \param System :: Simualation
   */
{
  ELog::RegMethod RegA("TS2target","addProtonLine");

  ELog::EM<<"Target centre [TS2] "<<Origin<<ELog::endDebug;

  PLine->setCutSurf("TargetSurf",*this,"TargetFront");
  PLine->setCutSurf("RefBoundary",getRule("FrontPlate"));
  PLine->createAll(System,*this,3);
  createBeamWindow(System,-4);

  return;
}

  
void
TS2target::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed Component for origin
    \param sideIndex :: line point
  */
{
  ELog::RegMethod RegA("TS2target","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createNoseConeObjects(System);
  layerProcess(System);
  createLinks();
  insertObjects(System);

  return;
}


  
}  // NAMESPACE TMRsystem
