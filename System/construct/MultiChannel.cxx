/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/MultiChannel.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Surface.h"
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
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "surfDBase.h"
#include "MultiChannel.h"


namespace constructSystem
{

MultiChannel::MultiChannel(const std::string& Key) :
  attachSystem::FixedRotate(Key,2),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut()
  /*!
    Default constructor
    \param Key :: Key name for variables
  */
{}
  
MultiChannel::MultiChannel(const MultiChannel& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::ExternalCut(A),
  nBlades(A.nBlades),
  bladeThick(A.bladeThick),length(A.length),
  bladeMat(A.bladeMat),voidMat(A.voidMat)
  /*!
    Copy constructor
    \param A :: MultiChannel to copy
  */
{}

MultiChannel&
MultiChannel::operator=(const MultiChannel& A)
  /*!
    Assignment operator
    \param A :: MultiChannel to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      
      nBlades=A.nBlades;
      bladeThick=A.bladeThick;
      length=A.length;
      bladeMat=A.bladeMat;
      voidMat=A.voidMat;
    }
  return *this;
}


  
void
MultiChannel::populate(const FuncDataBase& Control)
  /*!
    Sets the size of the object
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("MultiChannel","populate");

  FixedRotate::populate(Control);

  nBlades=Control.EvalVar<size_t>(keyName+"NBlades");
  bladeThick=Control.EvalVar<double>(keyName+"BladeThick");
  length=Control.EvalVar<double>(keyName+"Length");

  
  bladeMat=ModelSupport::EvalMat<int>(Control,keyName+"BladeMat");
  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  
  return;
}

void
MultiChannel::processSurface(const size_t index,
                             const double aFrac,
                             const double bFrac)
  /*!
    Process the surfaces and convert them into merged layers
    \param index :: index offset 
    \param aFrac :: Frac of side A
    \param bFrac :: Frac of side B 
  */
{
  ELog::RegMethod RegA("MultiChannel","processSurface");
  //
  // Currently we only can deal with two types of surface [ plane/plane
  // and plane/cylinder
  
  const int surfN(buildIndex+static_cast<int>(index)*10+3);
  int surfNX(surfN);

  Geometry::Surface* PX=
    ModelSupport::surfDBase::generalSurf(topSurf,baseSurf,aFrac,surfNX);
  SMap.addToIndex(surfN,PX->getName());
  addSurf("bladesA",surfN);

  PX=ModelSupport::surfDBase::generalSurf(topSurf,baseSurf,bFrac,surfNX);
  SMap.addToIndex(surfN+1,PX->getName());
  addSurf("bladesB",surfN+1);
  return;
}

void
MultiChannel::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("MultiChannel","createSurface");

  baseSurf=getRule("Base").primarySurface();
  topSurf=getRule("Top").primarySurface();

  
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  
  // first problem is to determine the build step:
  const double TotalD=topSurf->distance(Origin)+
    baseSurf->distance(Origin);

  const double BladeTotal(static_cast<double>(nBlades)*bladeThick);
  const double voidThick((TotalD-BladeTotal)/
			 static_cast<double>(nBlades+1));

  if (TotalD<BladeTotal+Geometry::zeroTol)
    throw ColErr::SizeError<double>
      (TotalD,BladeTotal,"Distance:BladeThick["+std::to_string(nBlades)+"]");
  

  double DPosA(voidThick);
  double DPosB(voidThick+bladeThick);
  for(size_t i=0;i<nBlades;i++)
    {
      processSurface(i,1.0-DPosA/TotalD,1.0-DPosB/TotalD);
      DPosA+=voidThick+bladeThick;
      DPosB+=voidThick+bladeThick;
    }
  return;
}

void
MultiChannel::createObjects(Simulation& System) 
  /*!
    Creates the colllimator block
    \param System :: Simuation for object
  */
{
  ELog::RegMethod RegA("MultiChannel","createObjects");

  const HeadRule baseRule=getRule("Base");
  const HeadRule topRule=getRule("Top");
  HeadRule HR;

  
  HeadRule fbHR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2");
  fbHR*=getRule("Left")*getRule("Right")*getRule("Divider");
  
  //  topRule=getRule("Top");
  HeadRule BHR(getRule("Base"));

  int SN(buildIndex);
  for(size_t i=0;i<nBlades;i++)
    {
      HR=BHR*HeadRule(SMap,SN,-3);
      System.addCell(cellIndex++,voidMat,0.0,HR*fbHR);

      HR=ModelSupport::getHeadRule(SMap,SN,"3 -4");
      System.addCell(cellIndex++,bladeMat,0.0,HR*fbHR);

      BHR=HeadRule(SMap,SN,4);
      SN+=10;
    }
  // LAST Volume
  HR=BHR*topRule;
  System.addCell(cellIndex++,voidMat,0.0,HR*fbHR);
  
  HR=fbHR*baseRule*topRule;
  addOuterSurf(HR);
  return;
}

void
MultiChannel::createLinks()
  /*!
    Construct the links for the system
  */
{
  ELog::RegMethod RegA("MultiChannel","createLinks");

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));      
  
  return;
}
    
void
MultiChannel::createAll(Simulation& System,
                     const attachSystem::FixedComp& FC,
                     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation 
    \param FC :: Fixed component to set axis etc
    \param sideIndex :: position of linkpoint
  */
{
  ELog::RegMethod RegA("MultiChannel","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  return;
}

  
}  // NAMESPACE constructSystem
