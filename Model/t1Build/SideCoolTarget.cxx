/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Build/SideCoolTarget.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include "FixedOffset.h"
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
  
  attachSystem::FixedOffset::populate(Control);
  
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
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("SideCoolTarget","createObjects");
  
  // Tungsten inner core

  std::string Out;
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 -7");
  System.addCell(MonteCarlo::Object(cellIndex++,wMat,targetTemp,Out));
  addBoundarySurf(-SMap.realSurf(buildIndex+17));

  // Cladding
  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -12 -17 (-1:2:7)");
  System.addCell(MonteCarlo::Object(cellIndex++,taMat,waterTemp,Out));

  // Water : Stops at the Tungsten edge
  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -12 -27 23 -24 101 17");
  System.addCell(MonteCarlo::Object(cellIndex++,waterMat,waterTemp,Out));
  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -12 -27 -23 24 -101 17");
  System.addCell(MonteCarlo::Object(cellIndex++,waterMat,waterTemp,Out));
  // front face
  Out=ModelSupport::getComposite(SMap,buildIndex,"21 -11 -27 ");
  System.addCell(MonteCarlo::Object(cellIndex++,waterMat,waterTemp,Out));
  
  // Pressure
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "11 -12 -37 33 -34 101 17 (27:-23:24)");
  System.addCell(MonteCarlo::Object(cellIndex++,taMat,externTemp,Out));
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "11 -12 -37 -35 36 -101 17 (27:23:-24)");
  System.addCell(MonteCarlo::Object(cellIndex++,taMat,externTemp,Out));
  Out=ModelSupport::getComposite(SMap,buildIndex,"31 -11 -37 (-21 : 27) ");
  System.addCell(MonteCarlo::Object(cellIndex++,taMat,externTemp,Out));

  // clearance void
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "11 -12 -47 101 17 (37:-33:34)");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "11 -12 -47 -101 17 (37:35:-36)");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  Out=ModelSupport::getComposite(SMap,buildIndex,"41 -11 -47 (-31 : 37) ");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  // Tail to be replaced with something
  Out=ModelSupport::getComposite(SMap,buildIndex,"12 -42 -47 ");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));

  // Set EXCLUDE:
  Out=ModelSupport::getComposite(SMap,buildIndex,"41 -42 -47");
  addOuterSurf(Out);
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
SideCoolTarget::addProtonLine(Simulation& System,
			      const attachSystem::FixedComp& refFC,
			      const long int index)
  /*!
    Add a proton void cell
    \param System :: Simualation
    \param refFC :: reflector edge
    \param index :: Index of the proton cutting surface [6 typically (-7)]
   */
{
  ELog::RegMethod RegA("SideCoolTarget","addProtonLine");

  // 0 ::  front face of target
  PLine->createAll(System,*this,0);
  createBeamWindow(System,1);
  System.populateCells();
  System.createObjSurfMap();
  return;
}

std::vector<int>  
SideCoolTarget::getInnerCells() const
  /*!
    Get the main inner cells
    \return Inner W and Ta cladding vector
  */
{
  std::vector<int> Out;
  Out.push_back(getMainBody());
  Out.push_back(buildIndex+2);
  return Out;
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
