/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Build/SideCoolTarget.cxx
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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "EllipticCyl.h"
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
#include "SideCoolTarget.h"


namespace ts1System
{

SideCoolTarget::SideCoolTarget(const std::string& Key) :
  constructSystem::TargetBase(Key,6),
  tarIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(tarIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

SideCoolTarget::SideCoolTarget(const SideCoolTarget& A) : 
  constructSystem::TargetBase(A),
  tarIndex(A.tarIndex),cellIndex(A.cellIndex),
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
      constructSystem::TargetBase::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
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
SideCoolTarget::createUnitVector(const attachSystem::FixedComp& FC,
				 const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed unit for origin + xyz
    \param sideIndex :: offset side
  */
{
  ELog::RegMethod RegA("SideCoolTarget","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

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
  ModelSupport::buildPlane(SMap,tarIndex+101,Origin,X);

  // Inner W
  ModelSupport::buildEllipticCyl(SMap,tarIndex+7,Origin,Y,X,
				 xRadius,zRadius);
  ModelSupport::buildPlane(SMap,tarIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,tarIndex+2,Origin+Y*mainLength,Y);

  double TThick(cladThick);  // main thickness
  double LThick(0.0);   // layer thick
  ModelSupport::buildEllipticCyl(SMap,tarIndex+17,Origin,Y,X,
				 xRadius+TThick,zRadius+TThick);
  ModelSupport::buildPlane(SMap,tarIndex+11,Origin-Y*(TThick),Y);
  ModelSupport::buildPlane(SMap,tarIndex+12,
			   Origin+Y*(mainLength+TThick),Y);

  // Water
  TThick+=waterThick;
  ModelSupport::buildEllipticCyl(SMap,tarIndex+27,Origin,Y,X,
				 xRadius+TThick,zRadius+TThick);
  ModelSupport::buildPlane(SMap,tarIndex+21,Origin-Y*(TThick),Y);
  ModelSupport::buildPlane(SMap,tarIndex+22,
			   Origin+Y*(mainLength+TThick),Y);

  ModelSupport::buildPlane(SMap,tarIndex+23,Origin,ZRotA);
  ModelSupport::buildPlane(SMap,tarIndex+24,Origin,ZRotB);

  // Pressure
  TThick+=pressThick;
  LThick+=pressThick;
  ModelSupport::buildEllipticCyl(SMap,tarIndex+37,Origin,Y,X,
				 xRadius+TThick,zRadius+TThick);
  ModelSupport::buildPlane(SMap,tarIndex+31,Origin-Y*(TThick),Y);
  ModelSupport::buildPlane(SMap,tarIndex+32,
		   Origin+Y*(mainLength+TThick),Y);

  ModelSupport::buildPlane(SMap,tarIndex+33,Origin-ZRotA*LThick,ZRotA);
  ModelSupport::buildPlane(SMap,tarIndex+34,Origin+ZRotB*LThick,ZRotB);
  ModelSupport::buildPlane(SMap,tarIndex+35,Origin+ZRotA*LThick,ZRotA);
  ModelSupport::buildPlane(SMap,tarIndex+36,Origin-ZRotB*LThick,ZRotB);

  // Clearance
  TThick+=voidThick;
  LThick+=voidThick;
  ModelSupport::buildEllipticCyl(SMap,tarIndex+47,Origin,Y,X,
				 xRadius+TThick,zRadius+TThick);
  ModelSupport::buildPlane(SMap,tarIndex+41,Origin-Y*(TThick),Y);
  ModelSupport::buildPlane(SMap,tarIndex+42,
		   Origin+Y*(mainLength+TThick),Y);

  ModelSupport::buildPlane(SMap,tarIndex+43,Origin-ZRotA*LThick,ZRotA);
  ModelSupport::buildPlane(SMap,tarIndex+44,Origin+ZRotB*LThick,ZRotB);
  ModelSupport::buildPlane(SMap,tarIndex+45,Origin+ZRotA*LThick,ZRotA);
  ModelSupport::buildPlane(SMap,tarIndex+46,Origin-ZRotB*LThick,ZRotB);



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
  Out=ModelSupport::getComposite(SMap,tarIndex,"1 -2 -7");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wMat,targetTemp,Out));
  addBoundarySurf(-SMap.realSurf(tarIndex+17));

  // Cladding
  Out=ModelSupport::getComposite(SMap,tarIndex,"11 -12 -17 (-1:2:7)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,waterTemp,Out));

  // Water : Stops at the Tungsten edge
  Out=ModelSupport::getComposite(SMap,tarIndex,"11 -12 -27 23 -24 101 17");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,waterTemp,Out));
  Out=ModelSupport::getComposite(SMap,tarIndex,"11 -12 -27 -23 24 -101 17");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,waterTemp,Out));
  // front face
  Out=ModelSupport::getComposite(SMap,tarIndex,"21 -11 -27 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,waterTemp,Out));
  
  // Pressure
  Out=ModelSupport::getComposite(SMap,tarIndex,
				 "11 -12 -37 33 -34 101 17 (27:-23:24)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,externTemp,Out));
  Out=ModelSupport::getComposite(SMap,tarIndex,
				 "11 -12 -37 -35 36 -101 17 (27:23:-24)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,externTemp,Out));
  Out=ModelSupport::getComposite(SMap,tarIndex,"31 -11 -37 (-21 : 27) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,externTemp,Out));

  // clearance void
  Out=ModelSupport::getComposite(SMap,tarIndex,
				 "11 -12 -47 101 17 (37:-33:34)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  Out=ModelSupport::getComposite(SMap,tarIndex,
				 "11 -12 -47 -101 17 (37:35:-36)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  Out=ModelSupport::getComposite(SMap,tarIndex,"41 -11 -47 (-31 : 37) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  // Tail to be replaced with something
  Out=ModelSupport::getComposite(SMap,tarIndex,"12 -42 -47 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // Set EXCLUDE:
  Out=ModelSupport::getComposite(SMap,tarIndex,"41 -42 -47");
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
  FixedComp::setLinkSurf(0,-SMap.realSurf(tarIndex+41));
  FixedComp::setLinkSurf(1,SMap.realSurf(tarIndex+42));
  for(size_t i=2;i<6;i++)
    FixedComp::setLinkSurf(i,SMap.realSurf(tarIndex+47));

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
  PLine->createAll(System,*this,0,refFC,index);
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
  Out.push_back(tarIndex+2);
  return Out;
}

void
SideCoolTarget::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed Component for origin
  */
{
  ELog::RegMethod RegA("SideCoolTarget","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,0);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);


  return;
}


  
}  // NAMESPACE TMRsystem
