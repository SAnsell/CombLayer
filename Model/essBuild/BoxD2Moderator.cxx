/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/BoxD2Moderator.cxx
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
#include "BoxD2Moderator.h"
#include "AttachSupport.h"

namespace essSystem
{

BoxD2Moderator::BoxD2Moderator(const std::string& Key) :
  EssModBase(Key,12),
  MidD2(new Box(Key+"MidD2")),
  MidBeNNBAR(new Box(Key+"MidBeNNBAR")),
  MidBeOther(new Box(Key+"MidBeOther"))
  
  // ,  LeftWater(new EdgeWater(Key+"LeftWater")),
  // RightWater(new EdgeWater(Key+"RightWater"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

   OR.addObject(MidD2);
   OR.addObject(MidBeNNBAR);
   OR.addObject(MidBeOther);
   // OR.addObject(LeftWater);
   // OR.addObject(RightWater);
}

BoxD2Moderator::BoxD2Moderator(const BoxD2Moderator& A) : 
  essSystem::EssModBase(A),
  MidD2(A.MidD2->clone()),
  MidBeNNBAR(A.MidBeNNBAR->clone()),
  MidBeOther(A.MidBeOther->clone()),
  // LeftWater(A.LeftWater->clone()),
  // RightWater(A.LeftWater->clone()),
  outerRadius(A.outerRadius)
  /*!
    Copy constructor
    \param A :: BoxD2Moderator to copy
  */
{}

BoxD2Moderator&
BoxD2Moderator::operator=(const BoxD2Moderator& A)
  /*!
    Assignment operator
    \param A :: BoxD2Moderator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      essSystem::EssModBase::operator=(A);
      cellIndex= A.cellIndex;
      *MidD2= *A.MidD2;
      *MidBeNNBAR= *A.MidBeNNBAR;
      *MidBeOther= *A.MidBeOther;
      //  *LeftWater= *A.LeftWater;
      //  *RightWater= *A.RightWater;
      outerRadius=A.outerRadius;
    }
  return *this;
}

BoxD2Moderator*
BoxD2Moderator::clone() const
  /*!
    virtual copy constructor
    \return new BoxD2Moderator(*this)
  */
{
  return new BoxD2Moderator(*this);
}


BoxD2Moderator::~BoxD2Moderator()
  /*!
    Destructor
  */
{}

void
BoxD2Moderator::populate(const FuncDataBase& Control)
  /*!
    Populate the variables
    \param Control :: DataBase
   */
{
  ELog::RegMethod RegA("BoxD2Moderator","populate");

  EssModBase::populate(Control);
  
  totalHeight=Control.EvalVar<double>(keyName+"TotalHeight");
  outerMat=ModelSupport::EvalMat<int>(Control,keyName+"OuterMat");
  return;
}

void
BoxD2Moderator::createUnitVector(const attachSystem::FixedComp& orgFC,
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
  ELog::RegMethod RegA("BoxD2Moderator","createUnitVector");

  EssModBase::createUnitVector(orgFC,orgIndex,axisFC,axisIndex);
  applyShift(0,0,totalHeight/2);

  return;
}


void
BoxD2Moderator::createSurfaces()
  /*!
    Create/hi-jack all the surfaces
  */
{
  ELog::RegMethod RegA("BoxD2Moderator","createSurface");

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,outerRadius);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(totalHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(totalHeight/2.0),Z);

  return;
}

void
BoxD2Moderator::createObjects(Simulation& System)
  /*!
    Adds the main components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("BoxD2Moderator","createObjects");

  const std::string Exclude=ContainedComp::getExclude();

  std::string Out;
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 5 -6 ");
  //  std::cout << "Outer mat " << outerMat << std::endl;
  System.addCell(MonteCarlo::Object(cellIndex++,outerMat,0.0,Out+Exclude));
  addCell("MainVolume",cellIndex-1);

  clearRules();
  addOuterSurf(Out);

  return;
}


int
BoxD2Moderator::getCommonSurf(const long int) const
  /*!
    Only components have reference values
    \param  :: sideIndex
    \return surface number
  */

{
  ELog::RegMethod RegA("BoxD2Moderator","getCommonSurf");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

int
BoxD2Moderator::getLayerSurf(const size_t,const long int) const
/*!
  [PLACEHOLDER] Only components have reference values
    \param  :: layer, 0 is inner moderator [0-6]
    \param  :: Side [0-3] // mid sides
  \return layer surface
  */
{
  ELog::RegMethod RegA("BoxD2Moderator","getLayerSurf");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

std::string
BoxD2Moderator::getLayerString(const size_t,const long int) const
  /*!
    Only components have reference values [PLACEHOLDER]
    \param  :: layer, 0 is inner moderator [0-6]
    \param  :: Side [0-3] // mid sides
    \return surface string
  */
{
  ELog::RegMethod RegA("BoxD2Moderator","getLayerString");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

Geometry::Vec3D
BoxD2Moderator::getSurfacePoint(const size_t,
				    const long int) const
  /*!
    [PLACEHOLDER] Given a side and a layer calculate the link point
    \param  :: layer, 0 is inner moderator [0-6]
    \param  :: Side [0-3] // mid sides
    \return Surface point
  */
{
  ELog::RegMethod RegA("BoxD2Moderator","getSurfacePoint");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

void
BoxD2Moderator::createLinks()
  /*!
    Create links but currently incomplete
  */
{
  ELog::RegMethod RegA("BoxD2Moderator","createLinks");

  FixedComp::setConnect(0,Origin-Y*outerRadius,-Y);
  FixedComp::setConnect(1,Origin+Y*outerRadius,Y);
  FixedComp::setConnect(2,Origin-X*outerRadius,-X);
  FixedComp::setConnect(3,Origin+X*outerRadius,X);
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+7));

  // copy surface top/bottom from H2Wing and Orign from center
  
  FixedComp::setLinkSignedCopy(4,*MidD2,5);
  FixedComp::setLinkSignedCopy(5,*MidD2,6);

  const double LowV= LU[4].getConnectPt().Z();
  const double HighV= LU[5].getConnectPt().Z();
  const Geometry::Vec3D LowPt(Origin.X(),Origin.Y(),LowV);
  const Geometry::Vec3D HighPt(Origin.X(),Origin.Y(),HighV);
  FixedComp::setConnect(4,LowPt,-Z);
  FixedComp::setConnect(5,HighPt,Z);

  //  FixedComp::setLinkCopy(6,*MidD2,12); ELog::EM << "is it correct?" << ELog::endDiag;

  return;
}


void
BoxD2Moderator::createExternal()
  /*!
    Constructs the full outer exclude object
  */
{
  ELog::RegMethod RegA("BoxD2Moderator","createExternal");

  addOuterUnionSurf(MidD2->getCompExclude());
  // addOuterUnionSurf(LeftWater->getCompExclude());
  // addOuterUnionSurf(RightWater->getCompExclude());

  return;
}

const attachSystem::FixedComp&
BoxD2Moderator::getComponent(const std::string& compName) const
  /*!
    Simple way to get a named component of this object
    \param compName :: Component name
    \return FixedComp object
  */
{
  ELog::RegMethod RegA("BoxD2Moderator","getComponent");

  const std::string TStr=keyName+compName;
  if (TStr==MidD2->getKeyName())
    return *MidD2;
  if (TStr==MidBeNNBAR->getKeyName())
    return *MidBeNNBAR;
  if (TStr==MidBeOther->getKeyName())
    return *MidBeOther;
  // if (TStr==LeftWater->getKeyName())
  //   return *LeftWater;
  // if (TStr==RightWater->getKeyName())
  //  return *RightWater;
  throw ColErr::InContainerError<std::string>(compName,keyName+" component");
}



std::string
BoxD2Moderator::getLeftFarExclude() const
  /*!
    Get the outer exclude surface without top/base
    (uses the standard link points)
    x>0
    \return outer sidewards link exclude
  */
{
  ELog::RegMethod RegA("BoxD2Moderator","getLeftFarExclude");

  std::string Out="";
  //  Out=LeftWater->getLinkString(4);
  //  Out+=RightWater->getLinkString(3);
  return Out;
}

std::string
BoxD2Moderator::getRightFarExclude() const
  /*!
    Get the outer exclude surface without top/base
    (uses the standard link points)
    x<0
    \return outer sidewards link exclude
  */
{
  ELog::RegMethod RegA("BoxD2Moderator","getRightFarExclude");

  std::string Out="";
  //  Out+=LeftWater->getLinkString(3);
  //  Out+=RightWater->getLinkString(4);

  return Out;
}

std::string
BoxD2Moderator::getLeftExclude() const
  /*!
    Get the complete exclude surface without top/base
    (uses the standard link points)
    x>0
    \return full sidewards link exclude
  */
{
  ELog::RegMethod RegA("BoxD2Moderator","getLeftExclude");
  std::string Out;

  Out+=MidD2->getLinkString(11);
  // Out+=getLeftFarExclude();

  return Out;
}

std::string
BoxD2Moderator::getRightExclude() const
  /*!
    Get the complete exclude surface without top/base
    (uses the standard link points) [+ve Y]
    x<0
    \return full sidewards link exclude
  */
{
  ELog::RegMethod RegA("BoxD2Moderator","getRightExclude");
  std::string Out;

  Out=MidD2->getLinkString(10);
  // Out+=getRightFarExclude();

  return Out;

}

void
BoxD2Moderator::createAll(Simulation& System,
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
  ELog::RegMethod RegA("BoxD2Moderator","createAll");

  populate(System.getDataBase());
  createUnitVector(orgFC,orgIndex,axisFC,axisIndex);
  createSurfaces();

  MidD2->createAll(System,*this,0);

  MidBeNNBAR->createAll(System,*MidD2,2);
  MidBeOther->createAll(System,*MidD2,1);
  attachSystem::addToInsertSurfCtrl(System,*MidD2,*MidBeNNBAR);
  attachSystem::addToInsertSurfCtrl(System,*MidD2,*MidBeOther);

  
  const std::string Exclude=
    ModelSupport::getComposite(SMap,buildIndex," -7 5 -6 ");
  // LeftWater->createAll(System,*MidD2,4,Exclude);
  // RightWater->createAll(System,*MidD2,3,Exclude);

  //  Origin=MidD2->getCentre();
  createExternal();  // makes intermediate

  createObjects(System);
  createLinks();
  return;
}

}  // NAMESPACE essSystem
