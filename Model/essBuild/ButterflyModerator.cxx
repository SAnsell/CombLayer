/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/ButterflyModerator.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include <array>
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
#include "Plane.h"
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
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ModBase.h"
#include "H2Wing.h"
#include "MidWaterDivider.h"
#include "EdgeWater.h"
#include "ButterflyModerator.h"

namespace essSystem
{

ButterflyModerator::ButterflyModerator(const std::string& Key) :
  constructSystem::ModBase(Key,12),
  flyIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(flyIndex+1),
  LeftUnit(new H2Wing(Key,"LeftLobe",90.0)),
  RightUnit(new H2Wing(Key,"RightLobe",270.0)),
  MidWater(new MidWaterDivider(Key,"MidWater")),
  LeftWater(new EdgeWater(Key+"LeftWater")),
  RightWater(new EdgeWater(Key+"RightWater"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
   ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

   OR.addObject(LeftUnit);
   OR.addObject(RightUnit);
   OR.addObject(MidWater);
   OR.addObject(LeftWater);
   OR.addObject(RightWater);
}

ButterflyModerator::ButterflyModerator(const ButterflyModerator& A) : 
  constructSystem::ModBase(A),
  flyIndex(A.flyIndex),cellIndex(A.cellIndex),
  LeftUnit(A.LeftUnit->clone()),
  RightUnit(A.RightUnit->clone()),
  MidWater(A.MidWater->clone()),
  LeftWater(A.LeftWater->clone()),
  RightWater(A.LeftWater->clone()),
  totalHeight(A.totalHeight),
  outerRadius(A.outerRadius),
  wallMat(A.wallMat),
  wallDepth(A.wallDepth),
  wallHeight(A.wallHeight)
  /*!
    Copy constructor
    \param A :: ButterflyModerator to copy
  */
{}

ButterflyModerator&
ButterflyModerator::operator=(const ButterflyModerator& A)
  /*!
    Assignment operator
    \param A :: ButterflyModerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      constructSystem::ModBase::operator=(A);
      cellIndex= A.cellIndex;
      *LeftUnit= *A.LeftUnit;
      *RightUnit= *A.RightUnit;
      *MidWater= *A.MidWater;
      *LeftWater= *A.LeftWater;
      *RightWater= *A.RightWater;
      totalHeight=A.totalHeight;
      outerRadius=A.outerRadius;
      wallMat=A.wallMat;
      wallDepth=A.wallDepth;
      wallHeight=A.wallHeight;
    }
  return *this;
}

ButterflyModerator*
ButterflyModerator::clone() const
  /*!
    virtual copy constructor
    \return new ButterflyModerator(*this)
  */
{
  return new ButterflyModerator(*this);
}

  
ButterflyModerator::~ButterflyModerator()
  /*!
    Destructor
  */
{}

void
ButterflyModerator::populate(const FuncDataBase& Control)
  /*!
    Populate the variables
    \param Control :: DataBase
   */
{
  ELog::RegMethod RegA("ButterflyModerator","populate");

  ModBase::populate(Control);
  totalHeight=Control.EvalVar<double>(keyName+"TotalHeight");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  wallDepth = Control.EvalVar<double>(keyName+"WallDepth");
  wallHeight = Control.EvalVar<double>(keyName+"WallHeight");

  return;
}

void
ButterflyModerator::createUnitVector(const attachSystem::FixedComp& axisFC,
				     const attachSystem::FixedComp* orgFC,
				     const long int sideIndex)
  /*!
    Create the unit vectors. This one uses axis from ther first FC
    but the origin for the second. Futher shifting the origin on the
    Z axis to the centre.
    \param axisFC :: FixedComp to get axis [origin if orgFC == 0]
    \param orgFC :: Extra origin point if required
    \param sideIndex :: link point for origin if given
  */
{
  ELog::RegMethod RegA("ButterflyModerator","createUnitVector");

  ModBase::createUnitVector(axisFC,orgFC,sideIndex);
  applyShift(0,0,totalHeight/2.0);
  
  return;
}

  
void
ButterflyModerator::createSurfaces()
  /*!
    Create/hi-jack all the surfaces
  */
{
  ELog::RegMethod RegA("ButterflyModerator","createSurface");
  
  ModelSupport::buildCylinder(SMap,flyIndex+7,Origin,Z,outerRadius);
  ModelSupport::buildPlane(SMap,flyIndex+5,Origin-Z*(totalHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,flyIndex+6,Origin+Z*(totalHeight/2.0),Z);

  ModelSupport::buildPlane(SMap,flyIndex+15,Origin-Z*(totalHeight/2.0-wallDepth),Z);
  ModelSupport::buildPlane(SMap,flyIndex+16,Origin+Z*(totalHeight/2.0-wallHeight),Z);

  return;
}

void
ButterflyModerator::createObjects(Simulation& System)
  /*!
    Adds the main components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ButterflyModerator","createObjects");

  // getSideRule contains only side surfaces, while getExclude - also top/bottom
  const std::string sideRule=getSideRule(); // ContainedComp::getExclude();

  HeadRule HR(sideRule);
  HR.makeComplement();

  std::string Out;

  if (wallDepth>Geometry::zeroTol) // \todo SA: why CL can't take care about it?
    {
      Out=ModelSupport::getComposite(SMap,flyIndex," -7 5 -15 ");  
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+HR.display()));
    }

  if (wallHeight>Geometry::zeroTol) // \todo SA: why CL can't take care about it?
    {
      Out=ModelSupport::getComposite(SMap,flyIndex," -7 16 -6 ");  
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+HR.display()));
      // otherwise split complicated cell by parts:
      /*      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+LeftWater->getSideRule()));
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+RightWater->getSideRule()));
      HeadRule bfHR;
      bfHR.procString(LeftUnit->getSideRule());
      bfHR.addUnion(RightUnit->getSideRule());
      bfHR.addUnion(MidWater->getSideRule());
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+bfHR.display()));*/
    }

  
  
  // getSideRule contains only side surfaces, while getExclude - also top/bottom
  const std::string Exclude=sideRule;//ContainedComp::getExclude();

  Out=ModelSupport::getComposite(SMap,flyIndex," -7 5 -6 ");  
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+Exclude));
  setCell("ambientVoid", cellIndex-1);

  clearRules();
  addOuterSurf(Out); // Out does not take into account wallDepth and wallHeight
  
  return;
}
  

int
ButterflyModerator::getCommonSurf(const long int) const
  /*!
    Only components have reference values
  */
{
  ELog::RegMethod RegA("ButterflyModerator","getCommonSurf");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

int
ButterflyModerator::getLayerSurf(const size_t,const long int) const
  /*!
    Only components have reference values
  */
{
  ELog::RegMethod RegA("ButterflyModerator","getLayerSurf");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

std::string
ButterflyModerator::getLayerString(const size_t,const long int) const
  /*!
    Only components have reference values
  */
{
  ELog::RegMethod RegA("ButterflyModerator","getLayerString");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

Geometry::Vec3D
ButterflyModerator::getSurfacePoint(const size_t,
				    const long int) const
  /*!
    Given a side and a layer calculate the link point
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \param sideIndex :: Side [0-3] // mid sides   
    \return Surface point
  */
{
  ELog::RegMethod RegA("ButterflyModerator","getSurfacePoint");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

void
ButterflyModerator::createLinks()
  /*!
    Create links but currently incomplete
  */
{
  ELog::RegMethod RegA("ButterflyModerator","createLinks");

  FixedComp::setConnect(0,Origin-Y*outerRadius,-Y);
  FixedComp::setConnect(1,Origin+Y*outerRadius,Y);
  FixedComp::setConnect(2,Origin-X*outerRadius,-X);
  FixedComp::setConnect(3,Origin+X*outerRadius,X);
  FixedComp::setLinkSurf(0,SMap.realSurf(flyIndex+7));
  FixedComp::setLinkSurf(1,SMap.realSurf(flyIndex+7));
  FixedComp::setLinkSurf(2,SMap.realSurf(flyIndex+7));
  FixedComp::setLinkSurf(3,SMap.realSurf(flyIndex+7));

  // copy surface top/bottom from H2Wing and Orign from center
  
  FixedComp::setLinkCopy(4,*LeftUnit,4);
  FixedComp::setLinkCopy(5,*LeftUnit,5);
  const double LowV= LU[4].getConnectPt().Z()-wallDepth*Z[2];
  const double HighV= LU[5].getConnectPt().Z()+wallHeight*Z[2];
  const Geometry::Vec3D LowPt(Origin.X(),Origin.Y(),LowV);
  const Geometry::Vec3D HighPt(Origin.X(),Origin.Y(),HighV);
  FixedComp::setConnect(4,LowPt,-Z);
  FixedComp::setConnect(5,HighPt,Z);

  return;
}

  
void
ButterflyModerator::createExternal()
  /*!
    Constructs the full outer exclude object 
  */
{
  ELog::RegMethod RegA("ButterflyModerator","createExternal");

  addOuterUnionSurf(LeftUnit->getCompExclude());
  addOuterUnionSurf(RightUnit->getCompExclude());
  addOuterUnionSurf(MidWater->getCompExclude());
  addOuterUnionSurf(LeftWater->getCompExclude());
  addOuterUnionSurf(RightWater->getCompExclude());
  
  return;
}

const attachSystem::FixedComp&
ButterflyModerator::getComponent(const std::string& compName) const
  /*!
    Simple way to get a named component of this object
    \param compName :: Component name
    \return FixedComp object
  */
{
  ELog::RegMethod RegA("ButterflyModerator","getComponent");

  const std::string TStr=keyName+compName;
  if (TStr==LeftUnit->getKeyName())
    return *LeftUnit;
  if (TStr==RightUnit->getKeyName())
    return *RightUnit;
  if (TStr==MidWater->getKeyName())
    return *MidWater;
  if (TStr==LeftWater->getKeyName())
    return *LeftWater;
  if (TStr==RightWater->getKeyName())
    return *RightWater;
  throw ColErr::InContainerError<std::string>(compName,keyName+" component");
}


std::string
ButterflyModerator::getSideRule() const
/*
  Return side rule
  \todo // SA: Use union of link points as it is faster
*/
{
  ELog::RegMethod RegA("ButterflyModerator","getSideRule");

  HeadRule HR;
  HR.procString(LeftUnit->getSideRule());
  HR.addUnion(RightUnit->getSideRule());
  HR.addUnion(MidWater->getSideRule());
  HR.addUnion(LeftWater->getSideRule());
  HR.addUnion(RightWater->getSideRule());
  HR.makeComplement();

  return HR.display();
}

std::string
ButterflyModerator::getLeftRightWaterSideRule() const
/*
  Return left+right water side rule
  \todo // SA: Use union of link points as it is faster
*/
{
  std::string side("");
  HeadRule HR;
  HR.procString(LeftWater->getSideRule());
  HR.addUnion(RightWater->getSideRule());
  HR.makeComplement();

  return HR.display();
}

  
void
ButterflyModerator::createAll(Simulation& System,
			      const attachSystem::FixedComp& axisFC,
			      const attachSystem::FixedComp* orgFC,
			      const long int sideIndex)
  /*!
    Construct the butterfly components
    \param System :: Simulation 
    \param axisFC :: FixedComp to get axis [origin if orgFC == 0]
    \param orgFC :: Extra origin point if required
    \param sideIndex :: link point for origin if given
   */
{
  ELog::RegMethod RegA("ButterflyModerator","createAll");

  populate(System.getDataBase());
  createUnitVector(axisFC,orgFC,sideIndex);
  createSurfaces();
  
  LeftUnit->createAll(System,*this);
  RightUnit->createAll(System,*this);
  MidWater->createAll(System,*this,*LeftUnit,*RightUnit);
    
  const std::string Exclude=
    ModelSupport::getComposite(SMap,flyIndex," -7 15 -16 ");
  LeftWater->createAll(System,*LeftUnit,2,Exclude);
  RightWater->createAll(System,*RightUnit,2,Exclude);

  Origin=MidWater->getCentre();
  createExternal();  // makes intermediate 


  createObjects(System);
  createLinks();
  
  return;
}




}  // NAMESPACE essSystem
