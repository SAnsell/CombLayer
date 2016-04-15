/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/TwisterModule.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
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
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "TwisterModule.h"

namespace essSystem
{

TwisterModule::TwisterModule(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,11),
  attachSystem::CellMap(),
  tIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(tIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
}

TwisterModule::TwisterModule(const TwisterModule& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  attachSystem::CellMap(A),  
  tIndex(A.tIndex),cellIndex(A.cellIndex),xStep(A.xStep),
  yStep(A.yStep),zStep(A.zStep),xyAngle(A.xyAngle),zAngle(A.zAngle),
  shaftRadius(A.shaftRadius),shaftHeight(A.shaftHeight),shaftWallThick(A.shaftWallThick),
  shaftBearingRadius(A.shaftBearingRadius),shaftBearingHeight(A.shaftBearingHeight),shaftBearingWallThick(A.shaftBearingWallThick),
  plugFrameRadius(A.plugFrameRadius),plugFrameHeight(A.plugFrameHeight),
  plugFrameDepth(A.plugFrameDepth),plugFrameAngle(A.plugFrameAngle),
  plugFrameWallThick(A.plugFrameWallThick),
  plugFrameMat(A.plugFrameMat),plugFrameWallMat(A.plugFrameWallMat),
  shaftMat(A.shaftMat),shaftWallMat(A.shaftWallMat)
  /*!
    Copy constructor
    \param A :: TwisterModule to copy
  */
{}

TwisterModule&
TwisterModule::operator=(const TwisterModule& A)
  /*!
    Assignment operator
    \param A :: TwisterModule to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      CellMap::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      shaftRadius=A.shaftRadius;
      shaftHeight=A.shaftHeight;
      shaftWallThick=A.shaftWallThick;
      shaftBearingRadius=A.shaftBearingRadius;
      shaftBearingHeight=A.shaftBearingHeight;
      shaftBearingWallThick=A.shaftBearingWallThick;

      plugFrameRadius=A.plugFrameRadius;
      plugFrameHeight=A.plugFrameHeight;
      plugFrameDepth=A.plugFrameDepth;
      plugFrameAngle=A.plugFrameAngle;
      plugFrameWallThick=A.plugFrameWallThick;

      plugFrameMat=A.plugFrameMat;
      plugFrameWallMat=A.plugFrameWallMat;
      shaftMat=A.shaftMat;
      shaftWallMat=A.shaftWallMat;
    }
  return *this;
}
  
  TwisterModule*
  TwisterModule::clone() const
  /*!
    Clone self
    \return new (this)
   */
  {
    return new TwisterModule(*this);
  }
  

TwisterModule::~TwisterModule()
  /*!
    Destructor
   */
{}

void
TwisterModule::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("TwisterModule","populate");

  engActive=Control.EvalPair<int>(keyName,"","EngineeringActive");

    // Master values
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");
  
  shaftRadius=Control.EvalVar<double>(keyName+"ShaftRadius");   
  shaftHeight=Control.EvalVar<double>(keyName+"ShaftHeight");   
  shaftWallThick=Control.EvalVar<double>(keyName+"ShaftWallThick");

  shaftBearingRadius=Control.EvalVar<double>(keyName+"ShaftBearingRadius");
  shaftBearingHeight=Control.EvalVar<double>(keyName+"ShaftBearingHeight");
  shaftBearingWallThick=Control.EvalVar<double>(keyName+"ShaftBearingWallThick");

  plugFrameRadius=Control.EvalVar<double>(keyName+"PlugFrameRadius");   
  plugFrameHeight=Control.EvalVar<double>(keyName+"PlugFrameHeight");   
  plugFrameDepth=Control.EvalVar<double>(keyName+"PlugFrameDepth");
  plugFrameAngle=Control.EvalVar<double>(keyName+"PlugFrameAngle");
  plugFrameWallThick=Control.EvalVar<double>(keyName+"PlugFrameWallThick");

  plugFrameMat=ModelSupport::EvalMat<int>(Control,keyName+"PlugFrameMat");
  plugFrameWallMat=ModelSupport::EvalMat<int>(Control,keyName+"PlugFrameWallMat");

  shaftMat=ModelSupport::EvalMat<int>(Control,keyName+"ShaftMat");
  shaftWallMat=ModelSupport::EvalMat<int>(Control,keyName+"ShaftWallMat");

  return;
}

void
TwisterModule::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
  */
{
  ELog::RegMethod RegA("TwisterModule","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);
  
  return;
}

void
TwisterModule::createSurfaces()
  /*!
    Create Surfaces for the Be
  */
{
  ELog::RegMethod RegA("TwisterModule","createSurfaces");

  ModelSupport::buildPlane(SMap,tIndex+5,Origin-Z*plugFrameDepth,Z);
  ModelSupport::buildPlane(SMap,tIndex+6,Origin+Z*plugFrameHeight,Z);
  ModelSupport::buildPlane(SMap,tIndex+16,Origin+Z*(plugFrameHeight+shaftHeight),Z);

  ModelSupport::buildPlane(SMap,tIndex+25,Origin-Z*(plugFrameDepth-plugFrameWallThick),Z);
  ModelSupport::buildPlane(SMap,tIndex+26,Origin+Z*(plugFrameHeight-plugFrameWallThick),Z);

  ModelSupport::buildCylinder(SMap,tIndex+7,Origin,Z,shaftRadius);
  ModelSupport::buildCylinder(SMap,tIndex+17,Origin,Z,shaftRadius+shaftWallThick);

  ModelSupport::buildCylinder(SMap,tIndex+27,Origin,Z,plugFrameRadius);
  ModelSupport::buildCylinder(SMap,tIndex+37,Origin,Z,plugFrameRadius-plugFrameWallThick);

  // sector walls
  const double angle = plugFrameAngle/2.0*M_PI/180;
  double R = shaftRadius;

  ModelSupport::buildPlaneRotAxis(SMap,tIndex+1,
				  Origin-Y*R*sin(angle)+X*R*cos(angle),
				  X,-Z,plugFrameAngle/2.0);
  ModelSupport::buildPlaneRotAxis(SMap,tIndex+11,
				  Origin-Y*R*sin(angle)-X*R*cos(angle),
				  X,-Z,-plugFrameAngle/2.0);

  R += shaftWallThick;
  ModelSupport::buildPlane(SMap,tIndex+2,Origin-Y*(R*sin(angle)),Y); // auxiliary dividing plane
  ModelSupport::buildPlaneRotAxis(SMap,tIndex+21,
				  Origin-Y*R*sin(angle)+X*R*cos(angle),
				  X,-Z,plugFrameAngle/2.0);
  ModelSupport::buildPlaneRotAxis(SMap,tIndex+31,
				  Origin-Y*R*sin(angle)-X*R*cos(angle),
				  X,-Z,-plugFrameAngle/2.0);

  // shaft bearing
  ModelSupport::buildPlane(SMap,tIndex+35,Origin-Z*(plugFrameDepth+shaftBearingHeight),Z);
  ModelSupport::buildCylinder(SMap,tIndex+47,Origin,Z,shaftBearingRadius);
  ModelSupport::buildCylinder(SMap,tIndex+57,Origin,Z,shaftBearingRadius+shaftBearingWallThick);
  

  return; 
}

void
TwisterModule::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("TwisterModule","createObjects");

  std::string Out;
  // shaft
  Out=ModelSupport::getComposite(SMap,tIndex," -7 25 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,shaftMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,tIndex," -17 7 25 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,shaftWallMat,0.0,Out));
  //  setCell("lowBe",cellIndex-1);

  // plug frame
  Out=ModelSupport::getComposite(SMap,tIndex," -37 25 -26 17 2 -1 11"); //  inside sector
  System.addCell(MonteCarlo::Qhull(cellIndex++,plugFrameMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,tIndex," -37 25 -26 17 2 -21 1"); //  inside sector wall x+
  System.addCell(MonteCarlo::Qhull(cellIndex++,plugFrameWallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,tIndex," -27 5 -6 17 (-2:21:-31)"); // outside sector
  System.addCell(MonteCarlo::Qhull(cellIndex++,plugFrameMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,tIndex," -37 25 -26 17 2 31 -11"); //  inside sector wall x-
  System.addCell(MonteCarlo::Qhull(cellIndex++,plugFrameWallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,tIndex," -27 5 -6 (37:-25:26) 17 2 -21 31 "); // outer wall inside sector
  System.addCell(MonteCarlo::Qhull(cellIndex++,plugFrameWallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,tIndex," -17 47 5 -25 "); // bottom wall of the shaft
  System.addCell(MonteCarlo::Qhull(cellIndex++,shaftWallMat,0.0,Out));

  // shaft bearing
  Out=ModelSupport::getComposite(SMap,tIndex, " -47 35 -25 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,shaftMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,tIndex, " -57 47 35 -5 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,shaftWallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,tIndex," (-17 6 -16) : (-27 5 -6) : (-57 35 -5) ");
  addOuterSurf(Out);

  return; 
}

void
TwisterModule::createLinks()
  /*!
    Creates a full attachment set
    Links/directions going outwards true.
  */
{
  return;
  FixedComp::setConnect(0,Origin+Y*shaftRadius,-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(tIndex+17));
  FixedComp::addLinkSurf(0,-SMap.realSurf(tIndex+1));

  FixedComp::setConnect(1,Origin+Y*shaftRadius,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(tIndex+17));
  FixedComp::addLinkSurf(1,SMap.realSurf(tIndex+1));

  FixedComp::setConnect(2,Origin+Y*shaftRadius,-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(tIndex+17));
  FixedComp::addLinkSurf(2,-SMap.realSurf(tIndex+2));
  
  FixedComp::setConnect(3,Origin+Y*shaftRadius,-X);
  FixedComp::setLinkSurf(3,SMap.realSurf(tIndex+17));
  FixedComp::addLinkSurf(3,SMap.realSurf(tIndex+2));
  
  FixedComp::setConnect(4,Origin-Z*(shaftHeight/2.0+shaftWallThick),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(tIndex+15));

  FixedComp::setConnect(5,Origin+Z*(shaftHeight/2.0+shaftWallThick),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(tIndex+16));

  FixedComp::setConnect(6,Origin-Z*(shaftHeight/2.0),-Z);
  FixedComp::setLinkSurf(6,-SMap.realSurf(tIndex+5));

  FixedComp::setConnect(7,Origin+Z*(shaftHeight/2.0),Z);
  FixedComp::setLinkSurf(7,SMap.realSurf(tIndex+6));

  FixedComp::setConnect(8,Origin+Y*(shaftRadius),-Y);
  FixedComp::setLinkSurf(8,-SMap.realSurf(tIndex+7));

  return;
}


void
TwisterModule::createAll(Simulation& System,
		   const attachSystem::FixedComp& FC)
/*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
  */
{
  ELog::RegMethod RegA("TwisterModule","createAll");
  populate(System.getDataBase());

  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE instrumentSystem
