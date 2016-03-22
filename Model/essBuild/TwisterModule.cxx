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
  yStep(A.yStep),zStep(A.zStep),xyAngle(A.xyAngle),
  zAngle(A.zAngle),radius(A.radius),height(A.height),
  wallThick(A.wallThick),wallThickLow(A.wallThickLow),lowVoidThick(A.lowVoidThick),
  topVoidThick(A.topVoidThick),targSepThick(A.targSepThick),
  refMat(A.refMat),wallMat(A.wallMat),
  targSepMat(A.targSepMat)
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
      radius=A.radius;
      height=A.height;
      wallThick=A.wallThick;
      wallThickLow=A.wallThickLow;
      lowVoidThick=A.lowVoidThick;
      topVoidThick=A.topVoidThick;
      targSepThick=A.targSepThick;
      refMat=A.refMat;
      wallMat=A.wallMat;
      targSepMat=A.targSepMat;
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
    \param targetThick :: thickness of the target
    \param topVThick :: thickness of the premod-void
    \param lowVThick :: thickness of the premod-void
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
  
  radius=Control.EvalVar<double>(keyName+"ShaftRadius");   
  height=Control.EvalVar<double>(keyName+"ShaftHeight");   
  wallThick=Control.EvalVar<double>(keyName+"ShaftWallThick");
  
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

  // DIVIDER PLANES:

  ModelSupport::buildPlane(SMap,tIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,tIndex+6,Origin+Z*height/2.0,Z);  
  
  ModelSupport::buildCylinder(SMap,tIndex+7,Origin,Z,radius);  
  ModelSupport::buildCylinder(SMap,tIndex+17,Origin,Z,radius+wallThick);  

  
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
  // low segment
  Out=ModelSupport::getComposite(SMap,tIndex," -7 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  //  setCell("lowBe",cellIndex-1);
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
  FixedComp::setConnect(0,Origin+Y*radius,-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(tIndex+17));
  FixedComp::addLinkSurf(0,-SMap.realSurf(tIndex+1));

  FixedComp::setConnect(1,Origin+Y*radius,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(tIndex+17));
  FixedComp::addLinkSurf(1,SMap.realSurf(tIndex+1));

  FixedComp::setConnect(2,Origin+Y*radius,-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(tIndex+17));
  FixedComp::addLinkSurf(2,-SMap.realSurf(tIndex+2));
  
  FixedComp::setConnect(3,Origin+Y*radius,-X);
  FixedComp::setLinkSurf(3,SMap.realSurf(tIndex+17));
  FixedComp::addLinkSurf(3,SMap.realSurf(tIndex+2));
  
  FixedComp::setConnect(4,Origin-Z*(height/2.0+wallThick),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(tIndex+15));

  FixedComp::setConnect(5,Origin+Z*(height/2.0+wallThick),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(tIndex+16));

  FixedComp::setConnect(6,Origin-Z*(height/2.0),-Z);
  FixedComp::setLinkSurf(6,-SMap.realSurf(tIndex+5));

  FixedComp::setConnect(7,Origin+Z*(height/2.0),Z);
  FixedComp::setLinkSurf(7,SMap.realSurf(tIndex+6));

  FixedComp::setConnect(8,Origin+Y*(radius),-Y);
  FixedComp::setLinkSurf(8,-SMap.realSurf(tIndex+7));

  FixedComp::setConnect(9,Origin-Z*(lowVoidThick+targSepThick/2.0+wallThickLow),-Z);
  FixedComp::setLinkSurf(9,-SMap.realSurf(tIndex+105));

  FixedComp::setConnect(10,Origin+Z*(lowVoidThick+targSepThick/2.0+wallThickLow),Z);
  FixedComp::setLinkSurf(10,SMap.realSurf(tIndex+106));

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
