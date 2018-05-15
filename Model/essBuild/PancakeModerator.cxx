/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/PancakeModerator.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell / Konstantin Batkov
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "EssModBase.h"
#include "H2Wing.h"
#include "SurfMap.h"
#include "DiskPreMod.h"
#include "EdgeWater.h"
#include "PancakeModerator.h"

namespace essSystem
{

PancakeModerator::PancakeModerator(const std::string& Key) :
  EssModBase(Key,12),
  flyIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(flyIndex+1),
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
  flyIndex(A.flyIndex),cellIndex(A.cellIndex),
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
      cellIndex= A.cellIndex;
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

  ModelSupport::buildCylinder(SMap,flyIndex+7,Origin,Z,outerRadius);
  ModelSupport::buildPlane(SMap,flyIndex+5,Origin-Z*(totalHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,flyIndex+6,Origin+Z*(totalHeight/2.0),Z);

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

  const std::string Exclude=ContainedComp::getExclude();

  std::string Out;
  Out=ModelSupport::getComposite(SMap,flyIndex," -7 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+Exclude));
  addCell("MainVoid",cellIndex-1);

  clearRules();
  addOuterSurf(Out);

  return;
}


int
PancakeModerator::getCommonSurf(const long int) const
  /*!
    Only components have reference values
    \param  :: sideIndex
    \return surface number
  */

{
  ELog::RegMethod RegA("PancakeModerator","getCommonSurf");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

int
PancakeModerator::getLayerSurf(const size_t,const long int) const
/*!
  [PLACEHOLDER] Only components have reference values
    \param  :: layer, 0 is inner moderator [0-6]
    \param  :: Side [0-3] // mid sides
  \return layer surface
  */
{
  ELog::RegMethod RegA("PancakeModerator","getLayerSurf");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

std::string
PancakeModerator::getLayerString(const size_t,const long int) const
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
  FixedComp::setLinkSurf(0,SMap.realSurf(flyIndex+7));
  FixedComp::setLinkSurf(1,SMap.realSurf(flyIndex+7));
  FixedComp::setLinkSurf(2,SMap.realSurf(flyIndex+7));
  FixedComp::setLinkSurf(3,SMap.realSurf(flyIndex+7));

  // copy surface top/bottom from H2Wing and Origin from center

  FixedComp::setLinkSignedCopy(4,*MidH2,5);
  FixedComp::setLinkSignedCopy(5,*MidH2,6);
  const double LowV= LU[4].getConnectPt().Z();
  const double HighV= LU[5].getConnectPt().Z();
  const Geometry::Vec3D LowPt(Origin.X(),Origin.Y(),LowV);
  const Geometry::Vec3D HighPt(Origin.X(),Origin.Y(),HighV);
  FixedComp::setConnect(4,LowPt,-Z);
  FixedComp::setConnect(5,HighPt,Z);

  //  FixedComp::setLinkSignedCopy(6,*MidH2,13); ELog::EM << "is it correct?" << ELog::endDiag;

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



std::string
PancakeModerator::getLeftFarExclude() const
  /*!
    Get the outer exclude surface without top/base
    (uses the standard link points)
    \return outer sidewards link exclude
  */
{
  ELog::RegMethod RegA("PancakeModerator","getLeftFarExclude");

  std::string Out;
  Out=LeftWater->getLinkString(4);
  Out+=RightWater->getLinkString(3);
  return Out;
}

std::string
PancakeModerator::getRightFarExclude() const
  /*!
    Get the outer exclude surface without top/base
    (uses the standard link points)
    \return outer sidewards link exclude
  */
{
  ELog::RegMethod RegA("PancakeModerator","getRightFarExclude");

  std::string Out;
  Out+=LeftWater->getLinkString(3);
  Out+=RightWater->getLinkString(4);

  return Out;
}

std::string
PancakeModerator::getLeftExclude() const
  /*!
    Get the complete exclude surface without top/base
    (uses the standard link points)
    \return full sidewards link exclude
  */
{
  ELog::RegMethod RegA("PancakeModerator","getLeftExclude");
  std::string Out;

  Out+=MidH2->getLinkString(1);
  Out+= getLeftFarExclude();

  return Out;
}

std::string
PancakeModerator::getRightExclude() const
  /*!
    Get the complete exclude surface without top/base
    (uses the standard link points) [+ve Y]
    \return full sidewards link exclude
  */
{
  ELog::RegMethod RegA("PancakeModerator","getRightExclude");
  std::string Out;

  Out+=MidH2->getLinkString(2);
  Out+= getRightFarExclude();

  return Out;

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
    \param orgFC :: Extra origin point if required
    \param sideIndex :: link point for origin if given
   */
{
  ELog::RegMethod RegA("PancakeModerator","createAll");

  populate(System.getDataBase());
  createUnitVector(orgFC,orgIndex,axisFC,axisIndex);
  createSurfaces();

  MidH2->createAll(System,*this,0,false,0.0);

  const std::string Exclude=
    ModelSupport::getComposite(SMap,flyIndex," -7 5 -6 ");
  LeftWater->createAll(System,*MidH2,4,Exclude);
  RightWater->createAll(System,*MidH2,3,Exclude);

  createExternal();  // makes intermediate

  createObjects(System);
  createLinks();
  return;
}

}  // NAMESPACE essSystem
