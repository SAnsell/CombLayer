/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   build/TS2FlatTarget.cxx
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
#include "TS2FlatTarget.h"


namespace TMRSystem
{

TS2FlatTarget::TS2FlatTarget(const std::string& Key) :
  constructSystem::TargetBase(Key,3),
  protonIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(protonIndex+1),frontPlate(0),backPlate(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

TS2FlatTarget::TS2FlatTarget(const TS2FlatTarget& A) : 
  constructSystem::TargetBase(A),
  protonIndex(A.protonIndex),cellIndex(A.cellIndex),
  frontPlate(A.frontPlate),backPlate(A.backPlate),
  mainLength(A.mainLength),coreRadius(A.coreRadius),
  surfThick(A.surfThick),cladThick(A.cladThick),
  waterThick(A.waterThick),waterFront(A.waterFront),
  pressureThick(A.pressureThick),
  pressureFront(A.pressureFront),voidRadius(A.voidRadius),
  voidFront(A.voidFront),flangeThick(A.flangeThick),
  flangeLen(A.flangeLen),flangeRadius(A.flangeRadius),
  flangeClear(A.flangeClear),flangeYStep(A.flangeYStep),
  flangeMat(A.flangeMat),wMat(A.wMat),taMat(A.taMat),
  waterMat(A.waterMat),targetTemp(A.targetTemp),
  waterTemp(A.waterTemp),externTemp(A.externTemp),
  mainCell(A.mainCell),skinCell(A.skinCell),
  nLayers(A.nLayers),mainFrac(A.mainFrac)
  /*!
    Copy constructor
    \param A :: TS2FlatTarget to copy
  */
{}

TS2FlatTarget&
TS2FlatTarget::operator=(const TS2FlatTarget& A)
  /*!
    Assignment operator
    \param A :: TS2FlatTarget to copy
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
      cladThick=A.cladThick;
      waterThick=A.waterThick;
      waterFront=A.waterFront;
      pressureThick=A.pressureThick;
      pressureFront=A.pressureFront;
      voidRadius=A.voidRadius;
      voidFront=A.voidFront;
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
      skinCell=A.skinCell;
      nLayers=A.nLayers;
      mainFrac=A.mainFrac;
    }
  return *this;
}


TS2FlatTarget*
TS2FlatTarget::clone() const
  /*!
    Clone funciton
    \return new(this)
  */
{
  return new TS2FlatTarget(*this);
}

TS2FlatTarget::~TS2FlatTarget() 
  /*!
    Destructor
  */
{}

void
TS2FlatTarget::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: database to use
  */
{
  ELog::RegMethod RegA("TS2FlatTarget","populate");

  FixedOffset::populate(Control);
  
  mainLength=Control.EvalVar<double>(keyName+"MainLength");
  coreRadius=Control.EvalVar<double>(keyName+"CoreRadius");
  surfThick=Control.EvalVar<double>(keyName+"SurfThick");

  cladThick=Control.EvalVar<double>(keyName+"CladThick");
  cladFront=Control.EvalVar<double>(keyName+"CladFront");

  waterThick=Control.EvalVar<double>(keyName+"WaterThick");
  waterFront=Control.EvalVar<double>(keyName+"WaterFront");

  pressureThick=Control.EvalVar<double>(keyName+"PressureThick");
  pressureFront=Control.EvalVar<double>(keyName+"PressureFront");
  voidRadius=Control.EvalVar<double>(keyName+"BoreRadius");
  voidFront=Control.EvalVar<double>(keyName+"VoidFront");


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
TS2FlatTarget::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Fixed unit for origin + xyz
  */
{
  ELog::RegMethod RegA("TS2FlatTarget","createUnitVector");

  FixedComp::createUnitVector(FC);
  applyOffset();
  
  return;
}

void
TS2FlatTarget::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("TS2FlatTarget","createSurface");
  
  // INNER PLANES

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
  if (surfThick>Geometry::zeroTol && nLayers>1)
    {
      ModelSupport::buildCylinder(SMap,protonIndex+17,Origin,Y,coreRadius-
				  surfThick);
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


  // Ta cladding [Inner]
  // Basic Cylinder
  ModelSupport::buildCylinder(SMap,protonIndex+27,Origin,Y,
			      coreRadius+cladThick);
  
  // --  WATER LAYER:  [40-60] -- 
  // cylinder water
  ModelSupport::buildCylinder(SMap,protonIndex+47,Origin,Y,
			      coreRadius+cladThick+waterThick);

  // pressure cylinder
  ModelSupport::buildCylinder(SMap,protonIndex+57,
			      Origin,Y,
			      coreRadius+cladThick+waterThick+pressureThick);

  // FRONT CAP
  ModelSupport::buildPlane(SMap,protonIndex+301,Origin-Y*cladFront,Y);
  ModelSupport::buildPlane(SMap,protonIndex+311,Origin-
			   Y*(cladFront+waterFront),Y);
  ModelSupport::buildPlane(SMap,protonIndex+321,Origin-
			   Y*(cladFront+waterFront+pressureFront),Y);
  ModelSupport::buildPlane(SMap,protonIndex+331,Origin-
			   Y*(cladFront+waterFront+pressureFront+voidFront),Y);

  return;
}

void
TS2FlatTarget::createObjects(Simulation& System)
  /*!
    Builds the target
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("TS2FlatTarget","createObjects");

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

  // Space for water manifold
  Out=ModelSupport::getComposite(SMap,protonIndex,"2 -101 190");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  ELog::EM<<"Cell == "<<Out<<" ::: "<<cellIndex-1<<ELog::endDiag;
  // FRONT Plate:
  Out=ModelSupport::getComposite(SMap,protonIndex,"-1 -27 301");
  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,protonIndex,"-1 -47 311 (27 : -301)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,protonIndex,"-1 -57 321 (47 : -311)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,protonIndex,"-1 -101 331 (57 : -321)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  

  // Set EXCLUDE:
  Out=ModelSupport::getComposite(SMap,protonIndex,
				 "331 -207 (-101 : (201 -202))");
  addOuterSurf(Out);
  addBoundarySurf(-SMap.realSurf(protonIndex+101));    
  return;
}

void
TS2FlatTarget::createLinks()
  /*!
    Create all the links
    \todo swap link 0 to be link 2 
  */
{
  // all point out
  FixedComp::setLinkSurf(0,SMap.realSurf(protonIndex+101));
  FixedComp::setLinkSurf(1,SMap.realSurf(protonIndex+2));
  // If not nose cone then change 2
  FixedComp::setBridgeSurf(2,-SMap.realSurf(protonIndex+331));

  FixedComp::setConnect(0,Origin+Z*voidRadius,Z);
  FixedComp::setConnect(1,Origin+Y*mainLength,Y);
  FixedComp::setConnect(2,Origin,-Y);

  return;
}

void 
TS2FlatTarget::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("TS2FlatTarget","LayerProcess");
  
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
TS2FlatTarget::addInnerBoundary(attachSystem::ContainedComp& CC) const
  /*!
    Adds the inner W boundary to an object
    \param CC :: Object to boundary to 
   */
{
  ELog::RegMethod RegA("TS2FlatTarget","addInnerBoundary");
  CC.addBoundarySurf(-SMap.realSurf(protonIndex+27));
  return;
}

void
TS2FlatTarget::addProtonLine(Simulation& System,
			 const attachSystem::FixedComp& refFC,
			 const long int index)
  /*!
    Add a proton void cell
    \param System :: Simualation
    \param refFC :: reflector edge
    \param index :: Index of the proton cutting surface [6 typically (-7)]
   */
{
  ELog::RegMethod RegA("TS2Target","addProtonLine");
  ELog::EM<<"Target centre [TS2] "<<Origin<<ELog::endDebug;

  PLine->createAll(System,*this,3,refFC,index);
  createBeamWindow(System,3);
  return;
}

  
void
TS2FlatTarget::createAll(Simulation& System,
			 const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed Component for origin
  */
{
  ELog::RegMethod RegA("TS2FlatTarget","createAll");

  populate(System.getDataBase());
  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  layerProcess(System);
  createLinks();
  addInnerBoundary(*this);
  insertObjects(System);

  return;
}


  
}  // NAMESPACE TMRsystem
