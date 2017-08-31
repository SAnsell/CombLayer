/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/voidCylVolume.cxx
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "voidCylVolume.h"

namespace constructSystem
{

voidCylVolume::voidCylVolume(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,0),
  voidIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(voidIndex+1),nSegment(0)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

voidCylVolume::voidCylVolume(const voidCylVolume& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  voidIndex(A.voidIndex),cellIndex(A.cellIndex),
  nSegment(A.nSegment),radius(A.radius),thick(A.thick),
  height(A.height)
  /*!
    Copy constructor
    \param A :: voidCylVolume to copy
  */
{}

voidCylVolume&
voidCylVolume::operator=(const voidCylVolume& A)
  /*!
    Assignment operator
    \param A :: voidCylVolume to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
      nSegment=A.nSegment;
      radius=A.radius;
      thick=A.thick;
      height=A.height;
    }
  return *this;
}

voidCylVolume::~voidCylVolume()
  /*!
    Destructor
  */
{}

void
voidCylVolume::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("voidCylVolume","populate");

  FixedOffset::populate(Control);
      
    // Master values
  nSegment=Control.EvalVar<size_t>(keyName+"NSegment");

  radius=Control.EvalVar<double>(keyName+"Radius");
  thick=Control.EvalVar<double>(keyName+"Thick");
  height=Control.EvalVar<double>(keyName+"Height");
  return;
}

void
voidCylVolume::createUnitVector(const attachSystem::FixedComp& FC,
				const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component for axis/origin
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("voidCylVolume","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
voidCylVolume::createSurfaces()
  /*!
    Create the surfaces [cylinder divided into segments]
  */
{
  ELog::RegMethod RegA("voidCylVolume","createSurfaces");


  ModelSupport::buildPlane(SMap,voidIndex+5,Origin-Z*(height/2.0),Z);  
  ModelSupport::buildPlane(SMap,voidIndex+6,Origin+Z*(height/2.0),Z);  

  const double angleStep(2.0*M_PI/static_cast<double>(nSegment));  
  int SI(voidIndex+11);
  int PI(voidIndex+111);
  double angle(0.0);
  double plateAngle(angleStep/2.0);

  for(size_t i=0;i<nSegment;i++)
    {
      const Geometry::Vec3D UVec(X*cos(angle)-Y*sin(angle));
      const Geometry::Vec3D PVec(X*sin(plateAngle)+Y*cos(plateAngle));
      ModelSupport::buildPlane(SMap,SI,Origin,UVec);
      ModelSupport::buildPlane(SMap,PI,Origin+PVec*radius,PVec);
      ModelSupport::buildPlane(SMap,PI+100,Origin+PVec*(radius+thick),PVec);
      angle+=angleStep;
      plateAngle+=angleStep;
      SI++;
      PI++;
    }
  return; 
}

void
voidCylVolume::createObjects(Simulation& System)
  /*!
    Create the volume [cylinder divided into segments]
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("voidCylVolume","createObjects");

  std::string Out;
  
  int SI(voidIndex+10);
  for(size_t i=1;i<nSegment;i++)
    {
      Out=ModelSupport::getComposite(SMap,voidIndex,SI,
				     "5 -6  1M -2M 101M -201M");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
      addOuterUnionSurf(Out);
      SI++;
    }
  // Join
  Out=ModelSupport::getComposite(SMap,voidIndex,SI,
				 "5 -6  1M -11 101M -201M");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addOuterUnionSurf(Out);

  return; 
}

void
voidCylVolume::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("voidCylVolume","createLinks");
  setNConnect(nSegment+2);

  const double angleStep(2.0*M_PI/voidIndex);  
  int PI(voidIndex+111);
  double plateAngle(angleStep/2.0);

  for(size_t i=0;i<nSegment;i++)
    {
      const Geometry::Vec3D PVec(X*sin(plateAngle)+Y*cos(plateAngle));
      FixedComp::setConnect(i,Origin+PVec*radius,-PVec);
      FixedComp::setLinkSurf(i,-SMap.realSurf(PI));
      plateAngle+=angleStep;
      PI++;
    }
  return;
}


void
voidCylVolume::createAll(Simulation& System,
			 const attachSystem::FixedComp& FC,
			 const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: link point
   */
{
  ELog::RegMethod RegA("voidCylVolume","createAll");

  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE constructSystem
