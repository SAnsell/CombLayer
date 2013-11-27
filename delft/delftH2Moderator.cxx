/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delft/delftH2Moderator.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

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
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
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
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "KGroup.h"
#include "Source.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "pipeUnit.h"
#include "PipeLine.h"
#include "virtualMod.h"
#include "delftH2Moderator.h"

namespace delftSystem
{


int
delftH2Moderator::calcDir(const double R)
  /*!
    Calculate the direction of the curvature
  */
{
  // Fix the back direction:
  if (fabs(R)<1e-5) return 0;
  return (R>0.0) ? 1 : -1;
}

delftH2Moderator::delftH2Moderator(const std::string& Key)  :
  virtualMod(Key),
  hydIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(hydIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

delftH2Moderator::delftH2Moderator(const delftH2Moderator& A) : 
  virtualMod(A),
  hydIndex(A.hydIndex),cellIndex(A.cellIndex),xStep(A.xStep),
  yStep(A.yStep),zStep(A.zStep),depth(A.depth),
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
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
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
delftH2Moderator::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("delftH2Moderator","populate");
  
  const FuncDataBase& Control=System.getDataBase();

  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");

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
delftH2Moderator::createUnitVector(const attachSystem::SecondTrack& CUnit)
  /*!
    Create the unit vectors
    - Y Points down the delftH2Moderator direction
    - X Across the delftH2Moderator
    - Z up (towards the target)
    \param CUnit :: Fixed unit that it is connected to 
  */
{
  ELog::RegMethod RegA("delftH2Moderator","createUnitVector");
  // Opposite since other face:
  X=CUnit.getBX();
  Y=CUnit.getBY();
  Z=CUnit.getBZ();

  Origin=CUnit.getBeamStart();
  Origin+=X*xStep+Y*yStep+Z*zStep;

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
    \param side -1 for back : 1 for front
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
      ModelSupport::buildPlane(SMap,hydIndex+1,BCentre,Y);
      ModelSupport::buildCylinder(SMap,hydIndex+11,BCentre,Z,
				  backDir*backRadius);
      ModelSupport::buildCylinder(SMap,hydIndex+21,
				  BCentre-Y*alBack,Z,
				  backDir*backRadius);
    }
  else // flat:
    {
      ModelSupport::buildPlane(SMap,hydIndex+11,BCentre,Y);
      ModelSupport::buildPlane(SMap,hydIndex+21,BCentre-Y*alBack,Y);
    }
  // FRONT SURFACES:
  if (frontDir)
    {
      ModelSupport::buildPlane(SMap,hydIndex+2,FCentre,Y);
      ModelSupport::buildCylinder(SMap,hydIndex+12,FCentre,Z,
				  frontDir*frontRadius);
      ModelSupport::buildCylinder(SMap,hydIndex+22,
				  FCentre+Y*alFront,Z,
				  frontDir*frontRadius);
    }
  else // flat:
    {
      ModelSupport::buildPlane(SMap,hydIndex+12,FCentre,Y);
      ModelSupport::buildPlane(SMap,hydIndex+22,FCentre+Y*alFront,Y);
    }

  // delftH2Moderator Layers
  ModelSupport::buildCylinder(SMap,hydIndex+3,Origin,Y,sideRadius);
  ModelSupport::buildCylinder(SMap,hydIndex+13,Origin,Y,sideRadius+alSide);
  
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
  FixedComp::addLinkSurf(0,surfDirB*SMap.realSurf(hydIndex+21));
  FixedComp::addLinkSurf(1,surfDirF*SMap.realSurf(hydIndex+22));
  FixedComp::addLinkSurf(2,SMap.realSurf(hydIndex+13));
  FixedComp::addLinkSurf(3,SMap.realSurf(hydIndex+13));
  FixedComp::addLinkSurf(4,SMap.realSurf(hydIndex+13));
  FixedComp::addLinkSurf(5,SMap.realSurf(hydIndex+13));

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
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("delftH2Moderator","createObjects");

  std::string Out;  

  std::string BOuter;
  std::string FOuter;
  std::string BInner;
  std::string FInner;

  if (backDir>0)
    {
      BOuter=ModelSupport::getComposite(SMap,hydIndex," -21 ");
      BInner=ModelSupport::getComposite(SMap,hydIndex," -11 ");
    }
  else if (backDir<0)
    {
      BOuter=ModelSupport::getComposite(SMap,hydIndex," 1 21 ");
      BInner=ModelSupport::getComposite(SMap,hydIndex," 1 11 ");
    }
  else
    {
      BOuter=ModelSupport::getComposite(SMap,hydIndex," 21 ");
      BInner=ModelSupport::getComposite(SMap,hydIndex," 11 ");
    }
  ELog::EM<<"Bouter == "<<BOuter<<ELog::endTrace;

  if (frontDir>0)
    {
      FOuter=ModelSupport::getComposite(SMap,hydIndex," -22 ");
      FInner=ModelSupport::getComposite(SMap,hydIndex," -12 ");
    }
  else if (frontDir<0)
    {
      FOuter=ModelSupport::getComposite(SMap,hydIndex," -2 22 ");
      FInner=ModelSupport::getComposite(SMap,hydIndex," -2 12 ");
    }
  else
    {
      FOuter=ModelSupport::getComposite(SMap,hydIndex," -22 ");
      FInner=ModelSupport::getComposite(SMap,hydIndex," -12 ");
    }
      // PLANE:
  Out=ModelSupport::getComposite(SMap,hydIndex," -13 ");
  addOuterSurf(Out+FOuter+BOuter);

  // Hydrogen:
  Out=ModelSupport::getComposite(SMap,hydIndex," -3 ");
  Out+=FInner+BInner;
  addBoundarySurf(Out);
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));
  HCell=cellIndex-1;

  // AL Layers:
  // exclude part first:
  Out=" ( 3 :";
  Out+=(frontDir>=0) ? " 12 : " : " -12 : ";
  Out+=(backDir>=0) ? " 11 )" : " -11 )";
  Out=ModelSupport::getComposite(SMap,hydIndex,Out);

  Out+=ModelSupport::getComposite(SMap,hydIndex," -13 ");
  Out+=FOuter+BOuter;
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));
  return;
}

int
delftH2Moderator::getDividePlane() const
  /*!
    Get the dividing plane
    \return Dividing plane [pointing out]
  */
{
  return SMap.realSurf(hydIndex+1);
}

int
delftH2Moderator::viewSurf() const
  /*!
    View surface 
    \return view surface [pointing out]
   */
{
  return SMap.realSurf(hydIndex+12);
}

void
delftH2Moderator::postCreateWork(Simulation&)
  /*!
    Add pipework
   */
{
  return;
}
  
void
delftH2Moderator::createAll(Simulation& System,
		       const attachSystem::TwinComp& FUnit)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FUnit :: Fixed Base unit
  */
{
  ELog::RegMethod RegA("delftH2Moderator","createAll");
  populate(System);

  createUnitVector(FUnit);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  
  return;
}
  
}  // NAMESPACE moderatorSystem
