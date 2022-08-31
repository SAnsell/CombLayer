/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Build/SideCoolTarget.cxx
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
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ProtonVoid.h"
#include "TargetBase.h"
#include "SideCoolTarget.h"


namespace ts1System
{

SideCoolTarget::SideCoolTarget(const std::string& Key) :
  TMRSystem::TargetBase(Key,6)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

SideCoolTarget::SideCoolTarget(const SideCoolTarget& A) : 
  TMRSystem::TargetBase(A),
  mainLength(A.mainLength),
  xRadius(A.xRadius),zRadius(A.zRadius),
  cladThick(A.cladThick),waterThick(A.waterThick),
  pressThick(A.pressThick),voidThick(A.voidThick),
  cutAngle(A.cutAngle),wMat(A.wMat),taMat(A.taMat),
  waterMat(A.waterMat),targetTemp(A.targetTemp),
  waterTemp(A.waterTemp),externTemp(A.externTemp),
  mainCell(A.mainCell),nLayers(A.nLayers),
  mainFrac(A.mainFrac)
  /*!
    Copy constructor
    \param A :: SideCoolTarget to copy
  */
{}

SideCoolTarget&
SideCoolTarget::operator=(const SideCoolTarget& A)
  /*!
    Assignment operator
    \param A :: SideCoolTarget to copy
    \return *this
  */
{
  if (this!=&A)
    {
      TMRSystem::TargetBase::operator=(A);
      mainLength=A.mainLength;
      xRadius=A.xRadius;
      zRadius=A.zRadius;
      cladThick=A.cladThick;
      waterThick=A.waterThick;
      pressThick=A.pressThick;
      voidThick=A.voidThick;
      cutAngle=A.cutAngle;
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


SideCoolTarget*
SideCoolTarget::clone() const
  /*!
    Clone funciton
    \return new(this)
  */
{
  return new SideCoolTarget(*this);
}

SideCoolTarget::~SideCoolTarget() 
  /*!
    Destructor
  */
{}

void
SideCoolTarget::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Database to use
  */
{
  ELog::RegMethod RegA("SideCoolTarget","populate");
  
  attachSystem::FixedRotate::populate(Control);
  
  mainLength=Control.EvalVar<double>(keyName+"MainLength");
  xRadius=Control.EvalVar<double>(keyName+"XRadius");
  zRadius=Control.EvalVar<double>(keyName+"ZRadius");
  cladThick=Control.EvalVar<double>(keyName+"CladThick");
  waterThick=Control.EvalVar<double>(keyName+"WaterThick");
  pressThick=Control.EvalVar<double>(keyName+"PressThick");
  voidThick=Control.EvalVar<double>(keyName+"VoidThick");
  cutAngle=Control.EvalVar<double>(keyName+"CutAngle");

  wMat=ModelSupport::EvalMat<int>(Control,keyName+"WMat");
  taMat=ModelSupport::EvalMat<int>(Control,keyName+"TaMat");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");

  targetTemp=Control.EvalVar<double>(keyName+"TargetTemp");
  waterTemp=Control.EvalVar<double>(keyName+"WaterTemp");
  externTemp=Control.EvalVar<double>(keyName+"ExternTemp");
  
  ///  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");

  return;
}

void
SideCoolTarget::createSurfaces()
  /*!
    Create All the surfaces
    The Ellipses get progressively more circular as it progresses
    because the ellipse only moves relative open side (for the void)
   */
{
  ELog::RegMethod RegA("SideCoolTarget","createSurface");

  // Side angles
  const Geometry::Quaternion QX=
    Geometry::Quaternion::calcQRotDeg(cutAngle,Y);
  Geometry::Vec3D ZRotA(Z);
  Geometry::Vec3D ZRotB(Z);
  QX.rotate(ZRotA);
  QX.invRotate(ZRotB);
  // divider plane
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin,X);

  // Inner W
  ModelSupport::buildEllipticCyl(SMap,buildIndex+7,Origin,Y,X,
				 xRadius,zRadius);
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*mainLength,Y);

  double TThick(cladThick);  // main thickness
  double LThick(0.0);   // layer thick
  ModelSupport::buildEllipticCyl(SMap,buildIndex+17,Origin,Y,X,
				 xRadius+TThick,zRadius+TThick);
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(TThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(mainLength+TThick),Y);

  // Water
  TThick+=waterThick;
  ModelSupport::buildEllipticCyl(SMap,buildIndex+27,Origin,Y,X,
				 xRadius+TThick,zRadius+TThick);
  ModelSupport::buildPlane(SMap,buildIndex+21,Origin-Y*(TThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,
			   Origin+Y*(mainLength+TThick),Y);

  ModelSupport::buildPlane(SMap,buildIndex+23,Origin,ZRotA);
  ModelSupport::buildPlane(SMap,buildIndex+24,Origin,ZRotB);

  // Pressure
  TThick+=pressThick;
  LThick+=pressThick;
  ModelSupport::buildEllipticCyl(SMap,buildIndex+37,Origin,Y,X,
				 xRadius+TThick,zRadius+TThick);
  ModelSupport::buildPlane(SMap,buildIndex+31,Origin-Y*(TThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+32,
		   Origin+Y*(mainLength+TThick),Y);

  ModelSupport::buildPlane(SMap,buildIndex+33,Origin-ZRotA*LThick,ZRotA);
  ModelSupport::buildPlane(SMap,buildIndex+34,Origin+ZRotB*LThick,ZRotB);
  ModelSupport::buildPlane(SMap,buildIndex+35,Origin+ZRotA*LThick,ZRotA);
  ModelSupport::buildPlane(SMap,buildIndex+36,Origin-ZRotB*LThick,ZRotB);

  // Clearance
  TThick+=voidThick;
  LThick+=voidThick;
  ModelSupport::buildEllipticCyl(SMap,buildIndex+47,Origin,Y,X,
				 xRadius+TThick,zRadius+TThick);
  ModelSupport::buildPlane(SMap,buildIndex+41,Origin-Y*(TThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+42,
		   Origin+Y*(mainLength+TThick),Y);

  ModelSupport::buildPlane(SMap,buildIndex+43,Origin-ZRotA*LThick,ZRotA);
  ModelSupport::buildPlane(SMap,buildIndex+44,Origin+ZRotB*LThick,ZRotB);
  ModelSupport::buildPlane(SMap,buildIndex+45,Origin+ZRotA*LThick,ZRotA);
  ModelSupport::buildPlane(SMap,buildIndex+46,Origin-ZRotB*LThick,ZRotB);



  return;
}

void
SideCoolTarget::createObjects(Simulation& System)
  /*!
    Creates the objects for a side cooled target
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("SideCoolTarget","createObjects");
  

  HeadRule HR;

  // Tungsten inner core
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -7");
  makeCell("Core",System,cellIndex++,wMat,targetTemp,HR);
  
  // Cladding
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 -17 (-1:2:7)");
  makeCell("Cladding",System,cellIndex++,taMat,waterTemp,HR);
  
  // Water : Stops at the Tungsten edge
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 -27 23 -24 101 17");
  makeCell("Water",System,cellIndex++,waterMat,waterTemp,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 -27 -23 24 -101 17");
  makeCell("Water",System,cellIndex++,waterMat,waterTemp,HR);

  // front face
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"21 -11 -27");
  makeCell("Water",System,cellIndex++,waterMat,waterTemp,HR);
  
  // Pressure
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				 "11 -12 -37 33 -34 101 17 (27:-23:24)");
  makeCell("Pressure",System,cellIndex++,taMat,externTemp,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				 "11 -12 -37 -35 36 -101 17 (27:23:-24)");
  makeCell("Pressure",System,cellIndex++,taMat,externTemp,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"31 -11 -37 (-21 : 27)");
  makeCell("Pressure",System,cellIndex++,taMat,externTemp,HR);
  
  // clearance void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				 "11 -12 -47 101 17 (37:-33:34)");
  makeCell("Void",System,cellIndex++,0,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
				 "11 -12 -47 -101 17 (37:35:-36)");
  makeCell("Void",System,cellIndex++,0,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"41 -11 -47 (-31 : 37)");
  makeCell("Void",System,cellIndex++,0,0.0,HR);
  // Tail to be replaced with something
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"12 -42 -47 ");
  makeCell("Tail",System,cellIndex++,0,0.0,HR);

  // Set EXCLUDE:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"41 -42 -47");
  addOuterSurf(HR);
  return;
}

void
SideCoolTarget::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("SideCoolTarget","createLinks");

  // all point out
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+41));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+42));
  for(size_t i=2;i<6;i++)
    FixedComp::setLinkSurf(i,SMap.realSurf(buildIndex+47));

  const double TThick(cladThick+waterThick+pressThick+
		      voidThick);
		      
  FixedComp::setConnect(0,Origin-Y*TThick,-Y);
  FixedComp::setConnect(1,Origin+Y*(mainLength+TThick),Y);
  FixedComp::setConnect(2,Origin-Z*(zRadius+TThick),-Z);
  FixedComp::setConnect(3,Origin+Z*(zRadius+TThick),Z);
  FixedComp::setConnect(4,Origin-X*(xRadius+TThick),-X);
  FixedComp::setConnect(5,Origin+X*(xRadius+TThick),X);

  return;
}



void
SideCoolTarget::addProtonLine(Simulation& System)
  /*!
    Add a proton void cell
    \param System :: Simualation
   */
{
  ELog::RegMethod RegA("SideCoolTarget","addProtonLine");

  PLine->setCutSurf("TargetSurf",*this,"front");
  PLine->setCutSurf("RefBoundary",getRule("FrontPlate"));
  PLine->createAll(System,*this,1);
  createBeamWindow(System,-1);
  return;
}

void
SideCoolTarget::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed Component for origin
    \param sideIndex :: Link poitn
  */
{
  ELog::RegMethod RegA("SideCoolTarget","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);


  return;
}


  
}  // NAMESPACE TMRsystem
