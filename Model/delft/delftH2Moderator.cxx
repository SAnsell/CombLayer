/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/delftH2Moderator.cxx
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
#include "virtualMod.h"
#include "delftH2Moderator.h"

namespace delftSystem
{


int
delftH2Moderator::calcDir(const double R)
  /*!
    Calculate the direction of the curvature
    \param R :: Radius direction 
    \return +/- direction [0 if none]
  */
{
  // Fix the back direction:
  if (fabs(R)<1e-5) return 0;
  return (R>0.0) ? 1 : -1;
}

delftH2Moderator::delftH2Moderator(const std::string& Key)  :
  virtualMod(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

delftH2Moderator::delftH2Moderator(const delftH2Moderator& A) : 
  virtualMod(A),depth(A.depth),
  sideRadius(A.sideRadius),innerXShift(A.innerXShift),
  frontDir(A.frontDir),frontRadius(A.frontRadius),
  backDir(A.backDir),backRadius(A.backRadius),alBack(A.alBack),
  alFront(A.alFront),alTop(A.alTop),alBase(A.alBase),
  alSide(A.alSide),modTemp(A.modTemp),modMat(A.modMat),
  alMat(A.alMat),FCentre(A.FCentre),BCentre(A.BCentre),
  HCell(A.HCell)
  /*!
    Copy constructor
    \param A :: delftH2Moderator to copy
  */
{}

delftH2Moderator&
delftH2Moderator::operator=(const delftH2Moderator& A)
  /*!
    Assignment operator
    \param A :: delftH2Moderator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      virtualMod::operator=(A);
      depth=A.depth;
      sideRadius=A.sideRadius;
      innerXShift=A.innerXShift;
      frontDir=A.frontDir;
      frontRadius=A.frontRadius;
      backDir=A.backDir;
      backRadius=A.backRadius;
      alBack=A.alBack;
      alFront=A.alFront;
      alTop=A.alTop;
      alBase=A.alBase;
      alSide=A.alSide;
      modTemp=A.modTemp;
      modMat=A.modMat;
      alMat=A.alMat;
      FCentre=A.FCentre;
      BCentre=A.BCentre;
      HCell=A.HCell;
    }
  return *this;
}

delftH2Moderator*
delftH2Moderator::clone() const
  /*!
    Clone copy constructor
    \return new this
  */
{
  return new delftH2Moderator(*this); 
}

delftH2Moderator::~delftH2Moderator() 
  /*!
    Destructor
  */
{}

void
delftH2Moderator::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase to used
  */
{
  ELog::RegMethod RegA("delftH2Moderator","populate");
  
  FixedRotate::populate(Control);
  
  depth=Control.EvalVar<double>(keyName+"Depth");

  sideRadius=Control.EvalVar<double>(keyName+"SideRadius");
  innerXShift=Control.EvalVar<double>(keyName+"InnerXShift");

  frontRadius=Control.EvalVar<double>(keyName+"FrontRadius");
  backRadius=Control.EvalVar<double>(keyName+"BackRadius");
  frontDir=calcDir(frontRadius);
  backDir=calcDir(backRadius);
  
  alBack=Control.EvalVar<double>(keyName+"AlBack"); 
  alFront=Control.EvalVar<double>(keyName+"AlFront"); 
  alTop=Control.EvalVar<double>(keyName+"AlTop"); 
  alBase=Control.EvalVar<double>(keyName+"AlBase"); 
  alSide=Control.EvalVar<double>(keyName+"AlSide"); 

  modTemp=Control.EvalVar<double>(keyName+"ModTemp");

  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");
  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");

  return;
}
  

void
delftH2Moderator::createUnitVector(const attachSystem::FixedComp& CUnit,
				   const long int sideIndex)
  /*!
    Create the unit vectors
    \param CUnit :: Fixed unit that it is connected to 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("delftH2Moderator","createUnitVector");
  // Opposite since other face:
  FixedComp::createUnitVector(CUnit,sideIndex);
  applyOffset();

  FCentre=calcCentre(frontDir,1,frontRadius);
  BCentre=calcCentre(backDir,-1,backRadius);
  ELog::EM<<"Front Centre == "<<FCentre<<ELog::endDebug;
  ELog::EM<<"Back Centre  == "<<BCentre<<ELog::endDebug;
  return;
}

Geometry::Vec3D
delftH2Moderator::calcCentre(const int curveType,const int side,
			     const double R) const
  /*!
    Calculate radius centre
    \param curveType :: Direction to bend [-ve inside / +ve outside]
    \param side :: -1 for back : 1 for front
    \param R :: Radius of curve [-ve for inward]
    \return centre of cylinder/sphere
  */
{
  ELog::RegMethod RegA("delftH2Moderator","calcCentre");
  const double sideD=(side>=0) ? 1.0 : -1.0;
  const double curveD=(curveType>=0) ? 1.0 : -1.0;
  Geometry::Vec3D CX=Origin+Y*(sideD*depth/2.0);
  // PLANE
  if (!curveType)  return CX;

  const double dist(sqrt(R*R-sideRadius*sideRadius));
  CX-=Y*(sideD*curveD*dist);
  return CX;
}
  
void
delftH2Moderator::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("delftH2Moderator","createSurfaces");

  // BACK PLANES:
  if (backDir)
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,BCentre,Y);
      ModelSupport::buildCylinder(SMap,buildIndex+11,BCentre,Z,
				  backDir*backRadius);
      ModelSupport::buildCylinder(SMap,buildIndex+21,
				  BCentre-Y*alBack,Z,
				  backDir*backRadius);
    }
  else // flat:
    {
      ModelSupport::buildPlane(SMap,buildIndex+11,BCentre,Y);
      ModelSupport::buildPlane(SMap,buildIndex+21,BCentre-Y*alBack,Y);
    }
  // FRONT SURFACES:
  if (frontDir)
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,FCentre,Y);
      ModelSupport::buildCylinder(SMap,buildIndex+12,FCentre,Z,
				  frontDir*frontRadius);
      ModelSupport::buildCylinder(SMap,buildIndex+22,
				  FCentre+Y*alFront,Z,
				  frontDir*frontRadius);
    }
  else // flat:
    {
      ModelSupport::buildPlane(SMap,buildIndex+12,FCentre,Y);
      ModelSupport::buildPlane(SMap,buildIndex+22,FCentre+Y*alFront,Y);
    }

  // delftH2Moderator Layers
  ModelSupport::buildCylinder(SMap,buildIndex+3,Origin,Y,sideRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+13,Origin,Y,sideRadius+alSide);
  
  return;
}

void
delftH2Moderator::createLinks()
  /*!
    Create links
  */
{
  ELog::RegMethod RegA("delftH2Moderator","createLinks");

  const int surfDirB=(backDir) ? backDir : 1;
  const int surfDirF=(frontDir) ? frontDir : 1;
  FixedComp::addLinkSurf(0,surfDirB*SMap.realSurf(buildIndex+21));
  FixedComp::addLinkSurf(1,surfDirF*SMap.realSurf(buildIndex+22));
  FixedComp::addLinkSurf(2,SMap.realSurf(buildIndex+13));
  FixedComp::addLinkSurf(3,SMap.realSurf(buildIndex+13));
  FixedComp::addLinkSurf(4,SMap.realSurf(buildIndex+13));
  FixedComp::addLinkSurf(5,SMap.realSurf(buildIndex+13));

  // set Links: BACK POINT
  if (backDir>0)   // outgoing:
    FixedComp::setConnect(0,BCentre-Y*(alBack+backRadius),-Y);
  else             // In going / Plane
    FixedComp::setConnect(0,Origin-Y*(alBack+depth/2.0),-Y);       

  // set Links: Front
  if (frontDir>0)   // outgoing:
    FixedComp::setConnect(1,FCentre+Y*(alFront+frontRadius),Y);       
  else             // In going / Plane
    FixedComp::setConnect(1,Origin+Y*(alFront+depth/2.0),Y);       

  FixedComp::setConnect(2,Origin-X*(alSide+sideRadius),-X);
  FixedComp::setConnect(3,Origin+X*(alSide+sideRadius),X);
  FixedComp::setConnect(4,Origin-Z*(alSide+sideRadius),-Z);
  FixedComp::setConnect(5,Origin+Z*(alSide+sideRadius),Z);

  return;
}
  
void
delftH2Moderator::createObjects(Simulation& System)
  /*!
    Builds 
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("delftH2Moderator","createObjects");

  HeadRule HR;

  HeadRule BOuterHR;
  HeadRule FOuterHR;
  HeadRule BInnerHR;
  HeadRule FInnerHR;

  if (backDir>0)
    {
      BOuterHR=HeadRule(SMap,buildIndex,-21);
      BInnerHR=HeadRule(SMap,buildIndex,-11);
    }
  else if (backDir<0)
    {
      BOuterHR=ModelSupport::getHeadRule(SMap,buildIndex,"1 21");
      BInnerHR=ModelSupport::getHeadRule(SMap,buildIndex,"1 11");
    }
  else
    {
      BOuterHR=HeadRule(SMap,buildIndex,21);
      BInnerHR=HeadRule(SMap,buildIndex,11);
    }
  ELog::EM<<"Bouter == "<<BOuterHR<<ELog::endTrace;

  if (frontDir>0)
    {
      FOuterHR=HeadRule(SMap,buildIndex,-22);
      FInnerHR=HeadRule(SMap,buildIndex,-12);
    }
  else if (frontDir<0)
    {
      FOuterHR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 22");
      FInnerHR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 12");
    }
  else
    {
      FOuterHR=HeadRule(SMap,buildIndex,-22);
      FInnerHR=HeadRule(SMap,buildIndex,-12);
    }
      // PLANE:
  HR=HeadRule(SMap,buildIndex,-13);
  addOuterSurf(HR*FOuterHR*BOuterHR);

  // Hydrogen:
  HR=HeadRule(SMap,buildIndex,-3);
  makeCell("HCell",System,cellIndex++,modMat,modTemp,HR*FInnerHR*BInnerHR);
  HCell=cellIndex-1;

  // AL Layers:
  // exclude part first:
  HR=HeadRule(SMap,buildIndex,3);
  HR+=HeadRule(SMap,buildIndex,(frontDir>=0 ? 12 : -12));
  HR+=HeadRule(SMap,buildIndex,(backDir>=0 ? 11 : -11));
	       
  HR*=HeadRule(SMap,buildIndex,-13);
  System.addCell(cellIndex++,alMat,modTemp,HR*FOuterHR*BOuterHR);
  return;
}

void
delftH2Moderator::postCreateWork(Simulation&)
  /*!
    Add pipework
    \param :: Simulation for pipework
  */
{
  return;
}
  
void
delftH2Moderator::createAll(Simulation& System,
			    const attachSystem::FixedComp& FUnit,
			    const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FUnit :: Fixed Base unit
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("delftH2Moderator","createAll");
  populate(System.getDataBase());

  createUnitVector(FUnit,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  
  return;
}
  
}  // NAMESPACE delftSystem
