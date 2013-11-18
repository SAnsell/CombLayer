/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/H2Moderator.cxx
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
#include "generateSurf.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "pipeUnit.h"
#include "PipeLine.h"
#include "H2Moderator.h"

namespace delftSystem
{

H2Moderator::H2Moderator(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6),
  hydIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(hydIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

H2Moderator::H2Moderator(const H2Moderator& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  hydIndex(A.hydIndex),cellIndex(A.cellIndex),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  depth(A.depth),radius(A.radius),sideRadius(A.sideRadius),
  innerXShift(A.innerXShift),alDivide(A.alDivide),
  alFront(A.alFront),alTop(A.alTop),alBase(A.alBase),
  alSide(A.alSide),modTemp(A.modTemp),modMat(A.modMat),
  alMat(A.alMat),HCentre(A.HCentre),HCell(A.HCell)
  /*!
    Copy constructor
    \param A :: H2Moderator to copy
  */
{}

H2Moderator&
H2Moderator::operator=(const H2Moderator& A)
  /*!
    Assignment operator
    \param A :: H2Moderator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      depth=A.depth;
      radius=A.radius;
      sideRadius=A.sideRadius;
      innerXShift=A.innerXShift;
      alDivide=A.alDivide;
      alFront=A.alFront;
      alTop=A.alTop;
      alBase=A.alBase;
      alSide=A.alSide;
      modTemp=A.modTemp;
      modMat=A.modMat;
      alMat=A.alMat;
      HCentre=A.HCentre;
      HCell=A.HCell;
    }
  return *this;
}


H2Moderator::~H2Moderator() 
  /*!
    Destructor
  */
{}

void
H2Moderator::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("H2Moderator","populate");
  
  const FuncDataBase& Control=System.getDataBase();

  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");

  depth=Control.EvalVar<double>(keyName+"Depth");
  radius=Control.EvalVar<double>(keyName+"Radius");
  sideRadius=Control.EvalVar<double>(keyName+"SideRadius");
  innerXShift=Control.EvalVar<double>(keyName+"InnerXShift");

  backRadius=Control.EvalVar<double>(keyName+"BackRadius");
  // Fix the back direction:
  if (fabs(backRadius)>1e-5)
    backDirection=(backRadius>0.0) ? 1 : -1;
  else
    backDirection=0;
  
  alDivide=Control.EvalVar<double>(keyName+"AlDivide"); 
  alFront=Control.EvalVar<double>(keyName+"AlFront"); 
  alTop=Control.EvalVar<double>(keyName+"AlTop"); 
  alBase=Control.EvalVar<double>(keyName+"AlBase"); 
  alSide=Control.EvalVar<double>(keyName+"AlSide"); 

  modTemp=Control.EvalVar<double>(keyName+"ModTemp");

  modMat=Control.EvalVar<int>(keyName+"ModMat");
  alMat=Control.EvalVar<int>(keyName+"AlMat");

  return;
}
  

void
H2Moderator::createUnitVector(const attachSystem::SecondTrack& CUnit)
  /*!
    Create the unit vectors
    - Y Points down the H2Moderator direction
    - X Across the H2Moderator
    - Z up (towards the target)
    \param CUnit :: Fixed unit that it is connected to 
  */
{
  ELog::RegMethod RegA("H2Moderator","createUnitVector");
  const masterRotate& MR=masterRotate::Instance();
  // Opposite since other face:
  X=CUnit.getBX();
  Y=CUnit.getBY();
  Z=CUnit.getBZ();

  Origin=CUnit.getBeamStart();
  Origin+=X*xStep+Y*yStep+Z*zStep;

  const double HLen=sqrt(radius*radius-sideRadius*sideRadius);
  HCentre=Origin-Y*(HLen-depth-alDivide)+X*innerXShift;;
  //  HCentre=Origin-Y*(radius-depth-alDivide)+X*innerXShift;

  if (backDirection)
    {
      const double GLen=sqrt(backRadius*backRadius-sideRadius*sideRadius);
      GCentre=Origin-Y*(backDirection*(GLen-alDivide));
    }
  ELog::EM<<"HCentre == "<<MR.calcRotate(HCentre)<<ELog::endTrace;
  ELog::EM<<"GCentre == "<<MR.calcRotate(GCentre)<<ELog::endTrace;

  return;
}
  
void
H2Moderator::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("H2Moderator","createSurfaces");

  ELog::EM<<"GCentre == "<<GCentre<<ELog::endTrace;
  ELog::EM<<"GCentre == "<<GCentre+Y*alSide<<ELog::endTrace;
  ModelSupport::buildPlane(SMap,hydIndex+1,GCentre,Y);
  ModelSupport::buildCylinder(SMap,hydIndex+11,GCentre,Z,backRadius);
  ModelSupport::buildCylinder(SMap,hydIndex+21,GCentre-Y*alSide,
			      Z,backRadius);

  // H2Moderator Layers
  ModelSupport::buildCylinder(SMap,hydIndex+3,Origin,Y,sideRadius);
  ModelSupport::buildCylinder(SMap,hydIndex+13,Origin,Y,sideRadius+alSide);

  // Front laters
  ModelSupport::buildPlane(SMap,hydIndex+2,HCentre,Y);
  ModelSupport::buildCylinder(SMap,hydIndex+12,HCentre,Z,radius);
  ModelSupport::buildCylinder(SMap,hydIndex+22,HCentre+Y*alFront,Z,radius);
  
  return;
}

void
H2Moderator::createLinks()
  /*!
    Create links
  */
{
  ELog::RegMethod RegA("H2Moderator","createLinks");


  FixedComp::addLinkSurf(0,-SMap.realSurf(hydIndex+21));
  FixedComp::addLinkSurf(1,SMap.realSurf(hydIndex+22));
  FixedComp::addLinkSurf(2,SMap.realSurf(hydIndex+13));
  FixedComp::addLinkSurf(3,SMap.realSurf(hydIndex+13));
  FixedComp::addLinkSurf(4,SMap.realSurf(hydIndex+13));
  FixedComp::addLinkSurf(5,SMap.realSurf(hydIndex+13));

  // set Links:
  FixedComp::setConnect(0,HCentre+Y*(alFront+radius),Y); 
  if (backDirection)
    {
      // Note modifed GLen to take into account the extra:
      const double SR=sideRadius;
      const double GLen=sqrt(backRadius*backRadius-SR*SR);
      FixedComp::setConnect(1,GCentre+Y*(GLen-alSide),-Y);
    }								  
  else
    FixedComp::setConnect(1,GCentre+Y*(backRadius-alSide),-Y);						  

  FixedComp::setConnect(2,Origin-X*(alSide+sideRadius),-X);
  FixedComp::setConnect(3,Origin+X*(alSide+sideRadius),X);
  FixedComp::setConnect(4,Origin-Z*(alSide+sideRadius),-Z);
  FixedComp::setConnect(5,Origin+Z*(alSide+sideRadius),Z);

  return;
}
  
void
H2Moderator::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("H2Moderator","createObjects");

  std::string Out;  

  // ARRANGEMENT FOR POSITIVE Radius:
  Out=ModelSupport::getComposite(SMap,hydIndex,"1 21 2 -22 -13 ");
  addOuterSurf(Out);
  Out=ModelSupport::getComposite(SMap,hydIndex,"1 11 2 -12 -3 ");
  addBoundarySurf(Out);
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));
  HCell=cellIndex-1;
  // Al layers :
  Out=ModelSupport::getComposite(SMap,hydIndex,"1 21 2 -22 -13  "
				 " (-11 : 12 : 3  ) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));
  
  return;
}

int
H2Moderator::getDividePlane() const
  /*!
    Get the dividing plane
    \return Dividing plane [pointing out]
  */
{
  return SMap.realSurf(hydIndex+1);
}

int
H2Moderator::viewSurf() const
  /*!
    View surface 
    \return view surface [pointing out]
   */
{
  return SMap.realSurf(hydIndex+2);
}
  
void
H2Moderator::createAll(Simulation& System,
		       const attachSystem::TwinComp& FUnit)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FUnit :: Fixed Base unit
  */
{
  ELog::RegMethod RegA("H2Moderator","createAll");
  populate(System);

  createUnitVector(FUnit);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  
  return;
}
  
}  // NAMESPACE moderatorSystem
