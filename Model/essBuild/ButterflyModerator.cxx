/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/ButterflyModerator.cxx
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
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "EssModBase.h"
#include "H2Wing.h"
#include "MidWaterDivider.h"
#include "EdgeWater.h"
#include "ButterflyModerator.h"

namespace essSystem
{

ButterflyModerator::ButterflyModerator(const std::string& Key) :
  essSystem::EssModBase(Key,12),
  bfType("BF1"),
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
   OR.addObject(LeftWater);
   OR.addObject(RightWater);
}

ButterflyModerator::ButterflyModerator(const ButterflyModerator& A) : 
  essSystem::EssModBase(A),
  bfType(A.bfType),
  LeftUnit(A.LeftUnit->clone()),
  RightUnit(A.RightUnit->clone()),
  MidWater(A.MidWater->clone()),
  LeftWater(A.LeftWater->clone()),
  RightWater(A.LeftWater->clone()),
  outerRadius(A.outerRadius)
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
      essSystem::EssModBase::operator=(A);
      bfType= A.bfType;
      *LeftUnit= *A.LeftUnit;
      *RightUnit= *A.RightUnit;
      *MidWater= *A.MidWater;
      *LeftWater= *A.LeftWater;
      *RightWater= *A.RightWater;
      outerRadius=A.outerRadius;
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

  EssModBase::populate(Control);
  
  bfType=Control.EvalDefVar<std::string>(keyName+"Type", "BF1");
  totalHeight=Control.EvalVar<double>(keyName+"TotalHeight");
  return;
}

void
ButterflyModerator::createUnitVector(const attachSystem::FixedComp& orgFC,
				     const long int orgIndex,
                                     const attachSystem::FixedComp& axisFC,
                                     const long int axisIndex)
  /*!
    Create the unit vectors. This one uses axis from ther first FC
    but the origin for the second. Futher shifting the origin on the
    Z axis to the centre.
    \param axisFC :: FixedComp to get axis [origin if orgFC == 0]
    \param orgFC :: Extra origin point if required
    \param orgIndex :: link point for origin if given
    \param axisIndex :: link point for origin if given
  */
{
  ELog::RegMethod RegA("ButterflyModerator","createUnitVector");

  EssModBase::createUnitVector(orgFC,orgIndex,axisFC,axisIndex);
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
  
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,outerRadius);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(totalHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(totalHeight/2.0),Z);

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
  
  HeadRule excludeHR=
    ExternalCut::getRule("Outer");

  excludeHR=getOuterSurf().complement();

  HeadRule HR;
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 5 -6");  
  makeCell("MainVoid",System,cellIndex++,0,0.0,HR*excludeHR);  
  //  clearRules();
  addOuterSurf(HR);
  
  return;
}
  
HeadRule
ButterflyModerator::getLayerHR(const size_t,const long int) const
  /*!
    Only components have reference values [PLACEHOLDER]
    \param  :: layer, 0 is inner moderator [0-6]
    \param  :: Side [0-3] // mid sides   
    \return surface string
  */
{
  ELog::RegMethod RegA("ButterflyModerator","getLayerString");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

Geometry::Vec3D
ButterflyModerator::getSurfacePoint(const size_t,
				    const long int) const
  /*!
    [PLACEHOLDER] Given a side and a layer calculate the link point
    \param  :: layer, 0 is inner moderator [0-6]
    \param  :: Side [0-3] // mid sides   
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
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+7));

  // copy surface top/bottom from H2Wing and Origin from center
  
  FixedComp::setLinkCopy(4,*LeftUnit,5);
  FixedComp::setLinkCopy(5,*LeftUnit,6);
  const double LowV= LU[4].getConnectPt().Z();
  const double HighV= LU[5].getConnectPt().Z();
  const Geometry::Vec3D LowPt(Origin.X(),Origin.Y(),LowV);
  const Geometry::Vec3D HighPt(Origin.X(),Origin.Y(),HighV);
  FixedComp::setConnect(4,LowPt,-Z);
  FixedComp::setConnect(5,HighPt,Z);

  FixedComp::setLinkCopy(6,*MidWater,13);  
  
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

  

HeadRule
ButterflyModerator::getLeftFarExclude() const
  /*!
    Get the outer exclude surface without top/base
    (uses the standard link points)
    \return outer sidewards link exclude
  */
{
  ELog::RegMethod RegA("ButterflyModerator","getLeftFarExclude");

  HeadRule HR;
  HR=LeftWater->getFullRule(4);   
  HR*=RightWater->getFullRule(3);
  return HR;
}

HeadRule
ButterflyModerator::getRightFarExclude() const
  /*!
    Get the outer exclude surface without top/base
    (uses the standard link points)
    \return outer sidewards link exclude
  */
{
  ELog::RegMethod RegA("ButterflyModerator","getRightFarExclude");

  HeadRule HR;
  HR=LeftWater->getFullRule(3);   
  HR*=RightWater->getFullRule(4);

  return HR;
}

HeadRule 
ButterflyModerator::getLeftExclude() const
  /*!
    Get the complete exclude surface without top/base
    (uses the standard link points)
    \return full sidewards link exclude
  */
{
  ELog::RegMethod RegA("ButterflyModerator","getLeftExclude");
  HeadRule HR;

  HR*=LeftUnit->getFullRule(8);
  HR*=RightUnit->getFullRule(9);
  HR*=MidWater->getFullRule(11);
  HR*= getLeftFarExclude();
  
  return HR;
}

HeadRule
ButterflyModerator::getRightExclude() const
  /*!
    Get the complete exclude surface without top/base
    (uses the standard link points) [+ve Y]
    \return full sidewards link exclude
  */
{
  ELog::RegMethod RegA("ButterflyModerator","getRightExclude");
  HeadRule HR;

  HR=LeftUnit->getFullRule(9);
  HR*=RightUnit->getFullRule(8);
  HR*=MidWater->getFullRule(12);

  HR*=getRightFarExclude();
  
  return HR;
}

void
ButterflyModerator::createAll(Simulation& System,
			      const attachSystem::FixedComp& FC,
                              const long int sideIndex)
/*!
  Construct components
  \param System :: Simulation 
  \param FC :: Origin/axis component
  \param sideIndex :: link point for origin 
*/
{
  ELog::RegMethod RegA("ButterflyModerator","createAll(FC)");
  
  createAll(System,FC,sideIndex,FC,sideIndex);
  return;
}

void
ButterflyModerator::createAll(Simulation& System,
			      const attachSystem::FixedComp& orgFC,
                              const long int orgIndex,
			      const attachSystem::FixedComp& axisFC,
			      const long int axisIndex)
  /*!
    Construct the butterfly components
    \param System :: Simulation 
    \param orgFC :: Extra origin point if required
    \param orgIndex :: link point for origin if given
    \param axisFC :: FixedComp to get axis [origin if orgFC == 0]
    \param axisIndex :: link point for axis
   */
{
  ELog::RegMethod RegA("ButterflyModerator","createAll");

  populate(System.getDataBase());
  createUnitVector(orgFC,orgIndex,axisFC,axisIndex);
  createSurfaces();
  
  LeftUnit->createAll(System,*this,0);
  RightUnit->createAll(System,*this,0);
  MidWater->setH2Wing(*LeftUnit,*RightUnit);
  MidWater->createAll(System,*this,0);
    
  const HeadRule ExcludeHR=
    ModelSupport::getHeadRule(SMap,buildIndex,"-7 5 -6");
  LeftWater->setCutSurf("Container",ExcludeHR);
  RightWater->setCutSurf("Container",ExcludeHR);
  LeftWater->createAll(System,*LeftUnit,2);
  RightWater->createAll(System,*RightUnit,2);

  Origin=MidWater->getCentre();
  createExternal();  // makes intermediate 


  createObjects(System);
  createLinks();
  return;
}

}  // NAMESPACE essSystem
