/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   build/TS2target.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "localRotate.h"
#include "masterRotate.h"
#include "surfDivide.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Sphere.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BeamWindow.h"
#include "ProtonVoid.h"
#include "TargetBase.h"
#include "TS2target.h"


namespace TMRSystem
{

TS2target::TS2target(const std::string& Key) :
  constructSystem::TargetBase(Key,3),
  protonIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(protonIndex+1),frontPlate(0),backPlate(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

TS2target::TS2target(const TS2target& A) : 
  constructSystem::TargetBase(A),
  protonIndex(A.protonIndex),cellIndex(A.cellIndex),
  frontPlate(A.frontPlate),
  backPlate(A.backPlate),mainLength(A.mainLength),coreRadius(A.coreRadius),
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
  externTemp(A.externTemp),mainCell(A.mainCell),
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
      constructSystem::TargetBase::operator=(A);
      cellIndex=A.cellIndex;
      frontPlate=A.frontPlate;
      backPlate=A.backPlate;
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
      mainCell=A.mainCell;
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

  attachSystem::FixedOffset::populate(Control);
  
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
TS2target::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Fixed unit for origin + xyz
  */
{
  ELog::RegMethod RegA("TS2target","createUnitVector");

  FixedComp::createUnitVector(FC);
  applyOffset();
  
  return;
}

void
TS2target::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("TS2target","createSurface");
  
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();

  // INNER PLANES    
  Geometry::Plane* PX;
  SMap.addMatch(protonIndex+186,frontPlate);
  SMap.addMatch(protonIndex+190,backPlate);

  // OUTER VOID [Should be a copy of 11 ?]
  ModelSupport::buildCylinder(SMap,protonIndex+101,Origin,Y,voidRadius);

  // front face [plane]
  ModelSupport::buildPlane(SMap,protonIndex+1,Origin,Y);
  // back face [plane]
  ModelSupport::buildPlane(SMap,protonIndex+2,Origin+Y*mainLength,Y);
  // Basis cylinder
  
  ModelSupport::buildCylinder(SMap,protonIndex+7,Origin,Y,coreRadius);
  ModelSupport::buildCylinder(SMap,protonIndex+8,
			      Origin-Y*wSphDisplace,Y,wSphRadius);
  if (surfThick>Geometry::zeroTol && nLayers>1)
    {
      ModelSupport::buildCylinder(SMap,protonIndex+17,Origin,Y,coreRadius-
				  surfThick);
      ModelSupport::buildCylinder(SMap,protonIndex+108,
				  Origin-Y*wSphDisplace,Y,
				  wSphRadius-surfThick);
    }

  // Top Edge Cylinder [used sphere radius!!]
  
  // FLANGE [Not moved with target]:
  ModelSupport::buildPlane(SMap,protonIndex+201,
			   Origin+Y*(mainLength-flangeLen/2.0),Y);
  ModelSupport::buildPlane(SMap,protonIndex+202,
			   Origin+Y*(mainLength+flangeLen/2.0),Y);
  ModelSupport::buildPlane(SMap,protonIndex+211,
			   Origin+Y*(mainLength-flangeYStep),Y);
  ModelSupport::buildPlane(SMap,protonIndex+212,
			   Origin+Y*(mainLength+
				     flangeThick+flangeYStep),Y);
  ModelSupport::buildCylinder(SMap,protonIndex+207,
			      Origin,Y,flangeRadius);
  ModelSupport::buildCylinder(SMap,protonIndex+217,
			      Origin,Y,flangeRadius-flangeClear);

  // NOSE CONE:
  // Top Sphere
  ModelSupport::buildSphere(SMap,protonIndex+9,
			    Origin-Y*wSphDisplace,wSphRadius);
  // Top Sphere [with layer]
  ModelSupport::buildSphere(SMap,protonIndex+109,
			    Origin-Y*wSphDisplace,wSphRadius-surfThick);
  // Sphere Mid plane
  ModelSupport::buildPlane(SMap,protonIndex+19,
			   Origin-Y*wSphDisplace,Y);

  // Cut Planes:
  // Rotation:
  Geometry::Quaternion QR=
    Geometry::Quaternion::calcQRotDeg(wPlaneAngle,Y);
  Geometry::Quaternion antiQR=
    Geometry::Quaternion::calcQRotDeg(-wPlaneAngle,Y);

  PX=SurI.createUniqSurf<Geometry::Plane>(protonIndex+3);
  PX->setPlane(Origin-Z*wPlaneCut,Z);
  PX->rotate(QR);
  SMap.registerSurf(protonIndex+3,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(protonIndex+4);
  PX->setPlane(Origin+Z*wPlaneCut,Z);
  PX->rotate(QR);
  SMap.registerSurf(protonIndex+4,PX);

  if (surfThick>Geometry::zeroTol && nLayers>1)
    {
      PX=SurI.createUniqSurf<Geometry::Plane>(protonIndex+103);
      PX->setPlane(Origin-Z*(wPlaneCut-surfThick),Z);
      PX->rotate(QR);
      SMap.registerSurf(protonIndex+103,PX);
      
      PX=SurI.createUniqSurf<Geometry::Plane>(protonIndex+104);
      PX->setPlane(Origin+Z*(wPlaneCut-surfThick),Z);
      PX->rotate(QR);
      SMap.registerSurf(protonIndex+104,PX);

    }
  // Ta cladding [Inner]
  // Basic Cylinder
  ModelSupport::buildCylinder(SMap,protonIndex+27,Origin,Y,
			      coreRadius+cladThick);
  
  // Top Sphere : 
  ModelSupport::buildSphere(SMap,protonIndex+29,
			    Origin-Y*taSphDisplace,taSphRadius);

  // Sphere Mid plane
  ModelSupport::buildPlane(SMap,protonIndex+39,Origin-Y*taSphDisplace,Y);
  
  // Cut Planes:
  PX=SurI.createUniqSurf<Geometry::Plane>(protonIndex+23);
  PX->setPlane(Origin-Z*taPlaneCut,Z);
  PX->rotate(QR);
  SMap.registerSurf(protonIndex+23,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(protonIndex+24);
  PX->setPlane(Origin+Z*taPlaneCut,Z);
  PX->rotate(QR);
  SMap.registerSurf(protonIndex+24,PX);

  // --  WATER LAYER:  [40-60] -- 
  // cylinder water
  ModelSupport::buildCylinder(SMap,protonIndex+47,Origin,Y,
			      coreRadius+cladThick+waterThick);

  // pressure cylinder
  ModelSupport::buildCylinder(SMap,protonIndex+57,
			      Origin,Y,
			      coreRadius+cladThick+waterThick+pressureThick);
  
  // CrossFlow Outer:

  // Base cylinder 
  ModelSupport::buildCylinder(SMap,protonIndex+48,Origin,Y,xFlowCyl);
 
  // OuterSphere Cut Plane
  ModelSupport::buildPlane(SMap,protonIndex+59,Origin-Y*xFlowOutDisplace,Y);

  // OuterCut Sphere [60-70]
  ModelSupport::buildSphere(SMap,protonIndex+61,
			    Origin-Y*xFlowOutDisplace,xFlowOutMidRadius);


  // Base cut cylinder
  ModelSupport::buildCylinder(SMap,protonIndex+62,
			      Origin,Y,xFlowOutMidRadius);    
  // OuterCut left Edge
  Geometry::Quaternion QCyl=
    Geometry::Quaternion::calcQRotDeg(xFlowOutCutAngle,Y);

  PX=SurI.createUniqSurf<Geometry::Plane>(protonIndex+63);
  PX->setPlane(Origin-Z*xFlowOPlaneCut,Z);    
  PX->rotate(antiQR);
  SMap.registerSurf(protonIndex+63,PX);

  // OuterCut right Edge
  PX=SurI.createUniqSurf<Geometry::Plane>(protonIndex+64);
  PX->setPlane(Origin+Z*xFlowIPlaneCut,Z);    
  PX->rotate(antiQR);
  SMap.registerSurf(protonIndex+64,PX);

  // Inner water cut:
  ModelSupport::buildSphere(SMap,protonIndex+71,
			    Origin-Y*xFlowInnerDisplace,xFlowInnerRadius);

  PX=SurI.createUniqSurf<Geometry::Plane>(protonIndex+73);
  PX->setPlane(Origin-Z*xFlowIPlaneCut,Z);    
  PX->rotate(QR);
  SMap.registerSurf(protonIndex+73,PX);
  // OuterCut right Edge
  PX=SurI.createUniqSurf<Geometry::Plane>(protonIndex+74);
  PX->setPlane(Origin+Z*xFlowIPlaneCut,Z);    
  PX->rotate(QR);
  SMap.registerSurf(protonIndex+74,PX);

  // TopCap [80-90]
  ModelSupport::buildSphere(SMap,protonIndex+81,
			    Origin-Y*tCapDisplace,tCapInnerRadius);

  // TopCap Cut Plane
  ModelSupport::buildPlane(SMap,protonIndex+85,Origin-Y*tCapDisplace,Y);
  
  // Base cut cyclinder
  ModelSupport::buildCylinder(SMap,protonIndex+82,Origin,Y,tCapInnerRadius);
  ModelSupport::buildSphere(SMap,protonIndex+91,
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

  std::string Out;
  // Front piece [Cylinder/Sphere]
  if (surfThick>Geometry::zeroTol && nLayers>1)
    {
      Out=ModelSupport::getComposite(SMap,protonIndex,
				     "-9 -19 3 -4 -8 (109:-103:104:108)");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wMat,0.0,Out));
      Out=ModelSupport::getComposite(SMap,protonIndex,"-109 -19 103 -104 -108");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wMat,0.0,Out));
      Out=ModelSupport::getComposite(SMap,protonIndex,
				     "-1 -8  19 3 -4 (108:-103:104)");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wMat,0.0,Out));
      Out=ModelSupport::getComposite(SMap,protonIndex,"-1 -108  19 103 -104");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wMat,0.0,Out));
    }
  else
    {
      Out=ModelSupport::getComposite(SMap,protonIndex,"-1 -8 (-9 : 19) 3 -4");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wMat,0.0,Out));
    }

  // ----------------- WATER -------------------------------
  // Water Cut Top: 
  
  Out=ModelSupport::getComposite(SMap,protonIndex,
		 "-81 61 ((63 -64):(-63 64)) -59 (29:24)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));
  const std::string watOCap=ModelSupport::getExclude(cellIndex-1);

  Out=ModelSupport::getComposite(SMap,protonIndex,
				 "-48 62 ((63 -64):(-63 64)) -1 59");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));
  const std::string watICyl=ModelSupport::getExclude(cellIndex-1);



  // Inner Water Cut:
  Out=ModelSupport::getComposite(SMap,protonIndex,
				 "71 -29 ((73 -74) : (-73 74)) -59");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));
  const std::string watICap=ModelSupport::getExclude(cellIndex-1);

  // Tungsen Cap [Part Sphere]
  Out=ModelSupport::getComposite(SMap,protonIndex,"-1 23 -24 (-27:-59) "
                                   "(-29: 59) "
   				 "(-3 : 4 : 8 : ( 9 -19 ) ) "
                                 +watICap+watICyl);
  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));

  // Cut: [Top section]
  Out=ModelSupport::getComposite(SMap,protonIndex,"-1 59 -48 (27 : -23 : 24)"
                                 +watICyl); 
  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));

  // Cut: [Top section/Part 2]
  Out=ModelSupport::getComposite(SMap,protonIndex,"-59 -81  "
   				 "(-23 : 24 : ( 29 -39 ) ) "
                                 +watICap+watOCap);
  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));


  // Cap Base:
  Out=ModelSupport::getComposite(SMap,protonIndex,"-1 -57 59 48");
  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));
  // Cap Top:
  Out=ModelSupport::getComposite(SMap,protonIndex,"-59 -91 81");
  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));


  Out=ModelSupport::getComposite(SMap,protonIndex,"-1 59 57 -101");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

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

  std::string Out;
  mainCell=cellIndex;
  // Main cylinder:
  if (surfThick>Geometry::zeroTol && nLayers>1)
    {
      Out=ModelSupport::getComposite(SMap,protonIndex,"1 -2 -17");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wMat,0.0,Out));
      Out=ModelSupport::getComposite(SMap,protonIndex,"1 -2 -7 17");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wMat,0.0,Out));
    }
  else
    {
      Out=ModelSupport::getComposite(SMap,protonIndex,"1 -2 -7");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wMat,0.0,Out));
    }


  // ----------------- FLANGE ----------------
  Out=ModelSupport::getComposite(SMap,protonIndex,"201 -202 -207 101");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

    
  // -- WATER -- [Main Cylinder]
  Out=ModelSupport::getComposite(SMap,protonIndex,"1 -2 27 -47");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));

  // -----------------------------------------------------------
  // Main Cylinder
  Out=ModelSupport::getComposite(SMap,protonIndex,"1 -2 7 -27");
  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));
  skinCell=cellIndex-1;

  // Ta Press: [Cylinder]
  Out=ModelSupport::getComposite(SMap,protonIndex,"1 -2 47 -57");
  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));
  

  // Spacer Void around target:
  Out=ModelSupport::getComposite(SMap,protonIndex,"1 -2 57 -101");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,protonIndex,"2 -101 186");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  

  // Front spacer [Proton flight line]:
  // if (backPlate)
  //   Out=ModelSupport::getComposite(SMap,protonIndex,"-59 91 -101 -190");
  // else
  //   Out=ModelSupport::getComposite(SMap,protonIndex,"-59 91 -101 -202 186");
  // System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  //  protonVoidCell=cellIndex-1;
  
  // Set EXCLUDE:
  Out=ModelSupport::getComposite(SMap,protonIndex,
				 "(59 :-91) -207 (-101 : (201 -202))");
  addOuterSurf(Out);
  // Inner boundary
  addBoundarySurf(-SMap.realSurf(protonIndex+27));    
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
  FixedComp::setLinkSurf(0,SMap.realSurf(protonIndex+101));
  FixedComp::setLinkSurf(1,SMap.realSurf(protonIndex+2));
  // If not nose cone then change 2
  FixedComp::setLinkSurf(2,SMap.realSurf(protonIndex+91));
  FixedComp::addLinkSurf(2,-SMap.realSurf(protonIndex+59));

  FixedComp::setConnect(0,Origin+Z*voidRadius,Z);
  FixedComp::setConnect(1,Origin+Y*mainLength,Y);
  FixedComp::setConnect(2,Origin-Y*(tCapDisplace+tCapOuterRadius),-Y);

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
      DA.setCellN(mainCell);
      DA.setOutNum(cellIndex,protonIndex+801);
      DA.makePair<Geometry::Plane>(SMap.realSurf(protonIndex+1),
				   SMap.realSurf(protonIndex+2));
      DA.activeDivide(System);
      cellIndex=DA.getCellNum();
    }
  return;
}


void
TS2target::addInnerBoundary(attachSystem::ContainedComp& CC) const
  /*!
    Adds the inner W boundary to an object
    \param CC :: Object to boundary to 
   */
{
  ELog::RegMethod RegA("TS2target","addInnerBoundary");
  CC.addBoundarySurf(-SMap.realSurf(protonIndex+27));
  return;
}



void
TS2target::addProtonLine(Simulation& System,
			 const attachSystem::FixedComp& refFC,
			 const long int index)
  /*!
    Add a proton void cell
    \param System :: Simualation
    \param refFC :: reflector edge
    \param index :: Index of the proton cutting surface [6 typically (-7)]
   */
{
  ELog::RegMethod RegA("TS2target","addProtonLine");
  ELog::EM<<"Target centre [TS2] "<<Origin<<ELog::endDebug;
  PLine->createAll(System,*this,3,refFC,index);
  createBeamWindow(System,-3);
  System.populateCells();
  System.createObjSurfMap();
  return;
}

  
void
TS2target::createAll(Simulation& System,const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed Component for origin
  */
{
  ELog::RegMethod RegA("TS2target","createAll");

  populate(System.getDataBase());
  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createNoseConeObjects(System);
  layerProcess(System);
  createLinks();
  insertObjects(System);
  //  addInnerBoundary()

  return;
}


  
}  // NAMESPACE TMRsystem
