/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/PancakeModerator.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell / Konstantin Batkov
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
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "FixedOffsetUnit.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "EssModBase.h"
#include "SurfMap.h"
#include "DiskPreMod.h"
#include "EdgeWater.h"
#include "PancakeModerator.h"

namespace essSystem
{

PancakeModerator::PancakeModerator(const std::string& Key) :
  EssModBase(Key,12),
  MidH2(new DiskPreMod(Key+"MidH2")),
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

PancakeModerator::PancakeModerator(const PancakeModerator& A) : 
  essSystem::EssModBase(A),
  MidH2(A.MidH2->clone()),
  LeftWater(A.LeftWater->clone()),
  RightWater(A.LeftWater->clone()),
  outerRadius(A.outerRadius)
  /*!
    Copy constructor
    \param A :: PancakeModerator to copy
  */
{}

PancakeModerator&
PancakeModerator::operator=(const PancakeModerator& A)
  /*!
    Assignment operator
    \param A :: PancakeModerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      essSystem::EssModBase::operator=(A);
      *MidH2= *A.MidH2;
      *LeftWater= *A.LeftWater;
      *RightWater= *A.RightWater;
      outerRadius=A.outerRadius;
    }
  return *this;
}

PancakeModerator*
PancakeModerator::clone() const
  /*!
    virtual copy constructor
    \return new PancakeModerator(*this)
  */
{
  return new PancakeModerator(*this);
}


PancakeModerator::~PancakeModerator()
  /*!
    Destructor
  */
{}

void
PancakeModerator::populate(const FuncDataBase& Control)
  /*!
    Populate the variables
    \param Control :: DataBase
   */
{
  ELog::RegMethod RegA("PancakeModerator","populate");

  EssModBase::populate(Control);
  
  totalHeight=Control.EvalVar<double>(keyName+"TotalHeight");
  return;
}

void
PancakeModerator::createUnitVector(const attachSystem::FixedComp& orgFC,
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
  ELog::RegMethod RegA("PancakeModerator","createUnitVector");

  EssModBase::createUnitVector(orgFC,orgIndex,axisFC,axisIndex);
  applyShift(0,0,totalHeight/2.0);

  return;
}


void
PancakeModerator::createSurfaces()
  /*!
    Create/hi-jack all the surfaces
  */
{
  ELog::RegMethod RegA("PancakeModerator","createSurface");

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,outerRadius);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(totalHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(totalHeight/2.0),Z);

  return;
}

void
PancakeModerator::createObjects(Simulation& System)
  /*!
    Adds the main components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("PancakeModerator","createObjects");

  const HeadRule excludeHR=ContainedComp::getExcludeSurf();
  HeadRule HR; 

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 5 -6");
  makeCell("MainVoid",System,cellIndex++,0,0.0,HR*excludeHR);

  clearRules();
  addOuterSurf(HR);

  return;
}

HeadRule
PancakeModerator::getLayerHR(const size_t,const long int) const
  /*!
    Only components have reference values [PLACEHOLDER]
    \param  :: layer, 0 is inner moderator [0-6]
    \param  :: Side [0-3] // mid sides
    \return surface string
  */
{
  ELog::RegMethod RegA("PancakeModerator","getLayerString");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

Geometry::Vec3D
PancakeModerator::getSurfacePoint(const size_t,
				    const long int) const
  /*!
    [PLACEHOLDER] Given a side and a layer calculate the link point
    \param  :: layer, 0 is inner moderator [0-6]
    \param  :: Side [0-3] // mid sides
    \return Surface point
  */
{
  ELog::RegMethod RegA("PancakeModerator","getSurfacePoint");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

void
PancakeModerator::createLinks()
  /*!
    Create links but currently incomplete
  */
{
  ELog::RegMethod RegA("PancakeModerator","createLinks");

  FixedComp::setConnect(0,Origin-Y*outerRadius,-Y);
  FixedComp::setConnect(1,Origin+Y*outerRadius,Y);
  FixedComp::setConnect(2,Origin-X*outerRadius,-X);
  FixedComp::setConnect(3,Origin+X*outerRadius,X);
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+7));

  // copy surface top/bottom from H2Wing and Origin from center

  FixedComp::setLinkCopy(4,*MidH2,5);
  FixedComp::setLinkCopy(5,*MidH2,6);
  const double LowV= LU[4].getConnectPt().Z();
  const double HighV= LU[5].getConnectPt().Z();
  const Geometry::Vec3D LowPt(Origin.X(),Origin.Y(),LowV);
  const Geometry::Vec3D HighPt(Origin.X(),Origin.Y(),HighV);
  FixedComp::setConnect(4,LowPt,-Z);
  FixedComp::setConnect(5,HighPt,Z);

  //  FixedComp::setLinkCopy(6,*MidH2,13); ELog::EM << "is it correct?" << ELog::endDiag;

  return;
}


void
PancakeModerator::createExternal()
  /*!
    Constructs the full outer exclude object
  */
{
  ELog::RegMethod RegA("PancakeModerator","createExternal");

  addOuterUnionSurf(MidH2->getCompExclude());
  addOuterUnionSurf(LeftWater->getCompExclude());
  addOuterUnionSurf(RightWater->getCompExclude());

  return;
}

const attachSystem::FixedComp&
PancakeModerator::getComponent(const std::string& compName) const
  /*!
    Simple way to get a named component of this object
    \param compName :: Component name
    \return FixedComp object
  */
{
  ELog::RegMethod RegA("PancakeModerator","getComponent");

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
PancakeModerator::getLeftFarExclude() const
  /*!
    Get the outer exclude surface without top/base
    (uses the standard link points)
    \return outer sidewards link exclude
  */
{
  ELog::RegMethod RegA("PancakeModerator","getLeftFarExclude");

  HeadRule HR;
  HR=LeftWater->getFullRule(4);
  HR*=RightWater->getFullRule(3);

  return HR;
}

HeadRule
PancakeModerator::getRightFarExclude() const
  /*!
    Get the outer exclude surface without top/base
    (uses the standard link points)
    \return outer sidewards link exclude
  */
{
  ELog::RegMethod RegA("PancakeModerator","getRightFarExclude");

  HeadRule HR;
  HR=LeftWater->getFullRule(3);
  HR*=RightWater->getFullRule(4);

  return HR;
}

HeadRule
PancakeModerator::getLeftExclude() const
  /*!
    Get the complete exclude surface without top/base
    (uses the standard link points)
    \return full sidewards link exclude
  */
{
  ELog::RegMethod RegA("PancakeModerator","getLeftExclude");

  HeadRule HR;

  HR=MidH2->getFullRule(1);
  HR*= getLeftFarExclude();

  return HR;
}

HeadRule
PancakeModerator::getRightExclude() const
  /*!
    Get the complete exclude surface without top/base
    (uses the standard link points) [+ve Y]
    \return full sidewards link exclude
  */
{
  ELog::RegMethod RegA("PancakeModerator","getRightExclude");
  HeadRule HR;

  HR=MidH2->getFullRule(2);
  HR*= getRightFarExclude();

  return HR;

}

void
PancakeModerator::createAll(Simulation& System,
			    const attachSystem::FixedComp& FC,
			    const long int sideIndex)

  /*!
    Construct the butterfly components
    \param System :: Simulation
    \param FC :: FixedComp to get axis [origin if orgFC == 0]
    \param sideIndex :: link point for origin if given
   */
{
  ELog::RegMethod RegA("PancakeModerator","createAll(FC)");

  createAll(System,FC,sideIndex,FC,sideIndex);
  return;
}
  
void
PancakeModerator::createAll(Simulation& System,
			      const attachSystem::FixedComp& orgFC,
                              const long int orgIndex,
			      const attachSystem::FixedComp& axisFC,
			      const long int axisIndex)
  /*!
    Construct the butterfly components
    \param System :: Simulation
    \param axisFC :: FixedComp to get axis [origin if orgFC == 0]
    \param orgIndex :: link point for origin if given
    \param orgFC :: Extra origin point if required
    \param axisIndex :: link point for origin if given

   */
{
  ELog::RegMethod RegA("PancakeModerator","createAll");

  populate(System.getDataBase());
  createUnitVector(orgFC,orgIndex,axisFC,axisIndex);
  createSurfaces();

  MidH2->createAll(System,*this,0);

  const HeadRule ExcludeHR=
    ModelSupport::getHeadRule(SMap,buildIndex,"-7 5 -6");
  LeftWater->setCutSurf("Container",ExcludeHR);
  RightWater->setCutSurf("Container",ExcludeHR);
  
  LeftWater->createAll(System,*MidH2,4);
  RightWater->createAll(System,*MidH2,3);

  createExternal();  // makes intermediate

  createObjects(System);
  createLinks();
  return;
}

}  // NAMESPACE essSystem
