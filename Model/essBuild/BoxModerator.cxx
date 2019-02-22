/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/BoxModerator.cxx
 *
 * Copyright (c) 2004-2018 by Konstantin Batkov / Stuart Ansell
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
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
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
//#include "H2Wing.h"
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
  essSystem::EssModBase(A),
  MidH2(A.MidH2->clone()),
  LeftWater(A.LeftWater->clone()),
  RightWater(A.LeftWater->clone()),
  outerRadius(A.outerRadius)
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
      essSystem::EssModBase::operator=(A);
      cellIndex= A.cellIndex;
      *MidH2= *A.MidH2;
      *LeftWater= *A.LeftWater;
      *RightWater= *A.RightWater;
      outerRadius=A.outerRadius;
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
    \param orgIndex :: link point for origin if given
    \param axisIndex :: link point for origin if given
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

  const std::string Exclude=ContainedComp::getExclude();

  std::string Out;
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 5 -6 ");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out+Exclude));
  addCell("MainVoid",cellIndex-1);

  clearRules();
  addOuterSurf(Out);

  return;
}


int
BoxModerator::getCommonSurf(const long int) const
  /*!
    Only components have reference values
    \param  :: sideIndex
    \return surface number
  */

{
  ELog::RegMethod RegA("BoxModerator","getCommonSurf");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

int
BoxModerator::getLayerSurf(const size_t,const long int) const
/*!
  [PLACEHOLDER] Only components have reference values
    \param  :: layer, 0 is inner moderator [0-6]
    \param  :: Side [0-3] // mid sides
  \return layer surface
  */
{
  ELog::RegMethod RegA("BoxModerator","getLayerSurf");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

std::string
BoxModerator::getLayerString(const size_t,const long int) const
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
  
  FixedComp::setLinkSignedCopy(4,*MidH2,5);
  FixedComp::setLinkSignedCopy(5,*MidH2,6);
  const double LowV= LU[4].getConnectPt().Z();
  const double HighV= LU[5].getConnectPt().Z();
  const Geometry::Vec3D LowPt(Origin.X(),Origin.Y(),LowV);
  const Geometry::Vec3D HighPt(Origin.X(),Origin.Y(),HighV);
  FixedComp::setConnect(4,LowPt,-Z);
  FixedComp::setConnect(5,HighPt,Z);

  //  FixedComp::setLinkCopy(6,*MidH2,12); ELog::EM << "is it correct?" << ELog::endDiag;

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



std::string
BoxModerator::getLeftFarExclude() const
  /*!
    Get the outer exclude surface without top/base
    (uses the standard link points)
    x>0
    \return outer sidewards link exclude
  */
{
  ELog::RegMethod RegA("BoxModerator","getLeftFarExclude");

  std::string Out;
  Out=LeftWater->getLinkString(4);
  Out+=RightWater->getLinkString(3);
  return Out;
}

std::string
BoxModerator::getRightFarExclude() const
  /*!
    Get the outer exclude surface without top/base
    (uses the standard link points)
    x<0
    \return outer sidewards link exclude
  */
{
  ELog::RegMethod RegA("BoxModerator","getRightFarExclude");

  std::string Out;
  Out+=LeftWater->getLinkString(3);
  Out+=RightWater->getLinkString(4);

  return Out;
}

std::string
BoxModerator::getLeftExclude() const
  /*!
    Get the complete exclude surface without top/base
    (uses the standard link points)
    x>0
    \return full sidewards link exclude
  */
{
  ELog::RegMethod RegA("BoxModerator","getLeftExclude");
  std::string Out;

  Out+=MidH2->getLinkString(11);
  Out+=getLeftFarExclude();

  return Out;
}

std::string
BoxModerator::getRightExclude() const
  /*!
    Get the complete exclude surface without top/base
    (uses the standard link points) [+ve Y]
    x<0
    \return full sidewards link exclude
  */
{
  ELog::RegMethod RegA("BoxModerator","getRightExclude");
  std::string Out;

  Out=MidH2->getLinkString(10);
  Out+=getRightFarExclude();

  return Out;

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

  const std::string Exclude=
    ModelSupport::getComposite(SMap,buildIndex," -7 5 -6 ");
  LeftWater->createAll(System,*MidH2,4,Exclude);
  RightWater->createAll(System,*MidH2,3,Exclude);

  createExternal();  // makes intermediate

  createObjects(System);
  createLinks();
  return;
}

}  // NAMESPACE essSystem
