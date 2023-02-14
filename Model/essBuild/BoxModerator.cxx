/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/BoxModerator.cxx
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov / Stuart Ansell
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
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Vec3D.h"
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "EssModBase.h"
#include "Box.h"
#include "EdgeWater.h"
#include "BoxModerator.h"

namespace essSystem
{

BoxModerator::BoxModerator(const std::string& Key) :
  EssModBase(Key,12),
  MidH2(new Box(Key+"MidH2")),
  LeftWater(new EdgeWater(Key+"LeftWater")),
  RightWater(new EdgeWater(Key+"RightWater"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
   ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

   OR.addObject(MidH2);
   OR.addObject(LeftWater);
   OR.addObject(RightWater);
}

BoxModerator::BoxModerator(const BoxModerator& A) : 
  EssModBase(A),
  MidH2(A.MidH2->clone()),
  LeftWater(A.LeftWater->clone()),
  RightWater(A.LeftWater->clone()),
  totalHeight(A.totalHeight),
  outerRadius(A.outerRadius),
  wallMat(A.wallMat),
  wallDepth(A.wallDepth),
  wallHeight(A.wallHeight)
  /*!
    Copy constructor
    \param A :: BoxModerator to copy
  */
{}

BoxModerator&
BoxModerator::operator=(const BoxModerator& A)
  /*!
    Assignment operator
    \param A :: BoxModerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      EssModBase::operator=(A);
      *MidH2= *A.MidH2;
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

BoxModerator*
BoxModerator::clone() const
  /*!
    virtual copy constructor
    \return new BoxModerator(*this)
  */
{
  return new BoxModerator(*this);
}

  
BoxModerator::~BoxModerator()
  /*!
    Destructor
  */
{}

void
BoxModerator::populate(const FuncDataBase& Control)
  /*!
    Populate the variables
    \param Control :: DataBase
   */
{
  ELog::RegMethod RegA("BoxModerator","populate");

  EssModBase::populate(Control);

  totalHeight=Control.EvalVar<double>(keyName+"TotalHeight");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  wallDepth = Control.EvalVar<double>(keyName+"WallDepth");
  wallHeight = Control.EvalVar<double>(keyName+"WallHeight");

  return;
}

void
BoxModerator::createUnitVector(const attachSystem::FixedComp& orgFC,
                                   const long int orgIndex,
                                   const attachSystem::FixedComp& axisFC,
                                   const long int axisIndex)
  /*!
    Create the unit vectors. This one uses axis from ther first FC
    but the origin for the second. Futher shifting the origin on the
    Z axis to the centre.
    \param axisFC :: FixedComp to get axis [origin if orgFC == 0]
    \param orgFC :: Extra origin point if required
    \param sideIndex :: link point for origin if given
  */
{
  ELog::RegMethod RegA("BoxModerator","createUnitVector");

  EssModBase::createUnitVector(orgFC,orgIndex,axisFC,axisIndex);
  applyShift(0,0,totalHeight/2.0);
  
  return;
}

  
void
BoxModerator::createSurfaces()
  /*!
    Create/hi-jack all the surfaces
  */
{
  ELog::RegMethod RegA("BoxModerator","createSurface");
  
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,outerRadius);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(totalHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(totalHeight/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(totalHeight/2.0-wallDepth),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(totalHeight/2.0-wallHeight),Z);

  return;
}

void
BoxModerator::createObjects(Simulation& System)
  /*!
    Adds the main components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("BoxModerator","createObjects");

  // getSideRule contains only side surfaces, while getExclude - also top/bottom
  const HeadRule sideHR(getSideRule().complement());


  HeadRule HR;

  if (wallDepth>Geometry::zeroTol) 
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 5 -15");  
      System.addCell(cellIndex++,wallMat,0.0,HR*sideHR);
    }

  if (wallHeight>Geometry::zeroTol) 
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 16 -6");  
      System.addCell(cellIndex++,wallMat,0.0,HR*sideHR);
    }

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 15 -16");
  makeCell("ambientVoid",System,cellIndex++,0,0.0,HR*sideHR);

  clearRules();
  addOuterSurf(HR);
  
  return;
}

HeadRule 
BoxModerator::getLayerHR(const size_t,const long int) const
  /*!
    Only components have reference values [PLACEHOLDER]
    \param  :: layer, 0 is inner moderator [0-6]
    \param  :: Side [0-3] // mid sides   
    \return surface string
  */
{
  ELog::RegMethod RegA("BoxModerator","getLayerString");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

Geometry::Vec3D
BoxModerator::getSurfacePoint(const size_t,
				    const long int) const
  /*!
    [PLACEHOLDER] Given a side and a layer calculate the link point
    \param  :: layer, 0 is inner moderator [0-6]
    \param  :: Side [0-3] // mid sides   
    \return Surface point
  */
{
  ELog::RegMethod RegA("BoxModerator","getSurfacePoint");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

void
BoxModerator::createLinks()
  /*!
    Create links but currently incomplete
  */
{
  ELog::RegMethod RegA("BoxModerator","createLinks");

  FixedComp::setConnect(0,Origin-Y*outerRadius,-Y);
  FixedComp::setConnect(1,Origin+Y*outerRadius,Y);
  FixedComp::setConnect(2,Origin-X*outerRadius,-X);
  FixedComp::setConnect(3,Origin+X*outerRadius,X);
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+7));

  // copy surface top/bottom from H2Wing and Orign from center
  
  FixedComp::setLinkCopy(4,*MidH2,5);
  FixedComp::setLinkCopy(5,*MidH2,6);
  const double LowV= LU[4].getConnectPt().Z()-wallDepth*Z[2];
  const double HighV= LU[5].getConnectPt().Z()+wallHeight*Z[2];
  const Geometry::Vec3D LowPt(Origin.X(),Origin.Y(),LowV);
  const Geometry::Vec3D HighPt(Origin.X(),Origin.Y(),HighV);
  FixedComp::setConnect(4,LowPt,-Z);
  FixedComp::setConnect(5,HighPt,Z);

  return;
}

  
void
BoxModerator::createExternal()
  /*!
    Constructs the full outer exclude object 
  */
{
  ELog::RegMethod RegA("BoxModerator","createExternal");

  addOuterUnionSurf(MidH2->getCompExclude());
  addOuterUnionSurf(LeftWater->getCompExclude());
  addOuterUnionSurf(RightWater->getCompExclude());
  
  return;
}

const attachSystem::FixedComp&
BoxModerator::getComponent(const std::string& compName) const
  /*!
    Simple way to get a named component of this object
    \param compName :: Component name
    \return FixedComp object
  */
{
  ELog::RegMethod RegA("BoxModerator","getComponent");

  const std::string TStr=keyName+compName;
  if (TStr==MidH2->getKeyName())
    return *MidH2;
  if (TStr==LeftWater->getKeyName())
    return *LeftWater;
  if (TStr==RightWater->getKeyName())
    return *RightWater;
  throw ColErr::InContainerError<std::string>(compName,keyName+" component");
}


HeadRule
BoxModerator::getSideRule() const
/*
  Return side rule
*/
{
  ELog::RegMethod RegA("BoxModerator","getSideRule");

  HeadRule HR;
  HR.addUnion(MidH2->getSideRule());
  //  HR.addUnion(LeftWater->getSideRule());
  //  HR.addUnion(RightWater->getSideRule());
  HR.makeComplement();

  return HR;
}

HeadRule 
BoxModerator::getLeftRightWaterSideRule() const
/*
  Return left+right water side rule
  \todo // SA: Use union of link points as it is faster
*/
{
  HeadRule HR;
  //  HR.procString(LeftWater->getSideRule());
  //  HR.addUnion(RightWater->getSideRule());
  HR.makeComplement();

  return HR;
}

  
void
BoxModerator::createAll(Simulation& System,
                        const attachSystem::FixedComp& FC,
                        const long int sideIndex)
/*!
  Construct the butterfly components iwth common point and axis
  \param System :: Simulation 
  \param FC :: FixedComp
  \param sideIndex :: link point
*/
{
  ELog::RegMethod RegA("BoxModerator","createAll");
  createAll(System,FC,sideIndex,FC,sideIndex);
  return;
}
  

void
BoxModerator::createAll(Simulation& System,
                        const attachSystem::FixedComp& orgFC,
                        const long int orgIndex,
                        const attachSystem::FixedComp& axisFC,
                        const long int axisIndex)
 /*!
    Construct the butterfly components
    \param System :: Simulation 
    \param axisFC :: FixedComp to get axis [origin if orgFC == 0]
    \param orgFC :: Extra origin point if required
    \param sideIndex :: link point for origin if given
   */
{
  ELog::RegMethod RegA("BoxModerator","createAll");

  populate(System.getDataBase());
  createUnitVector(orgFC,orgIndex,axisFC,axisIndex);
  createSurfaces();
  
  MidH2->createAll(System,*this,0);
    
  const Headrule ExcludeHR=
    ModelSupport::getHeadRule(SMap,buildIndex,"-7 15 -16");
  LeftWater->setCutSurf("Container",ExcludeHR);
  RightWater->setCutSurf("Container",ExcludeHR);
  LeftWater->createAll(System,*MidH2,4); 
  RightWater->createAll(System,*MidH2,3);

  Origin=MidH2->getCentre();
  createExternal();  // makes intermediate 


  createObjects(System);
  createLinks();
  
  return;
}


}  // NAMESPACE essSystem
