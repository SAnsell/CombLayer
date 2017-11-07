/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/localRotate.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <functional>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "mathSupport.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "transComp.h"
#include "localRotate.h"

localRotate::localRotate()
  /*!
    Constructor
  */
{}

localRotate::localRotate(const localRotate& A) 
  /*!
    Copy constructor
    \param A :: Rotation block to copy
  */
{
  std::vector<TPtr>::const_iterator vc;
  for(vc=A.Transforms.begin();vc!=A.Transforms.end();vc++)
    Transforms.push_back(TPtr((*vc)->clone()));
} 

void
localRotate::addRotation(const Geometry::Vec3D& A,
			  const Geometry::Vec3D& O,
			  const double ang)
  /*!
    Push back a rotation
    \param A :: Axis to rotate about
    \param O :: Offset
    \param ang :: Angle [deg]
   */
{
  Transforms.push_back(TPtr(new transRot(A,O,ang)));
  return;
}


void
localRotate::addMirror(const Geometry::Plane& A)
  /*!
    Push back a plane
    \param A :: Plane to add
   */
{
  Transforms.push_back(TPtr(new transMirror(A)));
  return;
}

void
localRotate::addDisplace(const Geometry::Vec3D& DV)
  /*!
    Push back a plane
    \param DV :: Displacement Vec
   */
{
  Transforms.push_back(TPtr(new transDisplace(DV)));
  return;
}

Geometry::Vec3D
localRotate::reverseRotate(const Geometry::Vec3D& V) const
  /*!
    Rotate/dispalce a vector
    \param V :: Vector to rotate [in reverse]
    \return Vector rotated/dispalces
  */
{
  Geometry::Vec3D Vpt(V);
  // Helper func ptr for boost since can't resolve type as overloaded:
  void (transComp::*fn)(Geometry::Vec3D&) const =  
    &transComp::reverse;

  for_each(Transforms.rbegin(),Transforms.rend(),
	   std::bind<void>(fn,std::placeholders::_1,std::ref(Vpt)));
	   //	   [&Vpt,fn](const TPtr& transCompPtr)
	   //	   { ( (*transCompPtr).*fn)(Vpt); });
  return Vpt;
}

Geometry::Vec3D
localRotate::calcRotate(const Geometry::Vec3D& V) const
  /*!
    Rotate and displace a vector
    \param V :: Vector to rotate / displace
    \return Vector rotated/displaces
  */
{
  Geometry::Vec3D Vpt(V);
  // Helper func ptr for boost since can't resolve type as overloaded:
  void (transComp::*fn)(Geometry::Vec3D&) const =  &transComp::apply;
  for_each(Transforms.begin(),Transforms.end(),
	     std::bind<void>(fn,std::placeholders::_1,
                             std::ref(Vpt)));
  return Vpt;
}

void
localRotate::axisRotate(Geometry::Vec3D& V) const
  /*!
    Rotate a vector as axis only
    \param V :: Vector to rotate
  */
{
  for_each(Transforms.begin(),Transforms.end(),
	     std::bind<void>(&transComp::applyAxis,std::placeholders::_1,
			       std::ref(V)));
  return;
}

void
localRotate::axisRotateReverse(Geometry::Vec3D& V) const
  /*!
    Rotate a vector as axis only
    \param V :: Vector to rotate
  */
{
  for_each(Transforms.rbegin(),Transforms.rend(),
	     std::bind<void>(&transComp::reverseAxis,
			       std::placeholders::_1,std::ref(V)));
  return;
}

Geometry::Vec3D
localRotate::calcAxisRotate(const Geometry::Vec3D& V) const
  /*!
    Rotate a vector as axis only
    \param V :: Vector to rotate
    \return Rotated vector
  */
{
  Geometry::Vec3D VP(V);
  axisRotate(VP);
  return VP;
}

Geometry::Vec3D
localRotate::reverseAxisRotate(const Geometry::Vec3D& V) const
  /*!
    Rotate a vector as axis only [Reverse]
    \param V :: Vector to rotate
    \return Rotated vector
  */
{
  Geometry::Vec3D VP(V);
  axisRotateReverse(VP);
  return VP;
}

void
localRotate::applyFull(Geometry::Surface* SPtr) const
  /*!
    Apply full rotations:
    \param SPtr :: Surface Pointer
  */
{
  void (transComp::*fn)(Geometry::Surface*) const =  &transComp::apply;
  for_each(Transforms.begin(),Transforms.end(),
	   std::bind<void>(fn,std::placeholders::_1,SPtr));
  
  return;
}

void
localRotate::applyFull(MonteCarlo::Object* OPtr) const
  /*!
    Apply full rotations:
    \param OPtr :: Object pointer
   */
{
  void (transComp::*fn)(MonteCarlo::Object*) const =  &transComp::apply;
  for_each(Transforms.begin(),Transforms.end(),
	   std::bind<void>(fn,std::placeholders::_1,OPtr));
  return;
}

void
localRotate::applyFull(Geometry::Vec3D& Pt) const
  /*!
    Apply full rotations:
    \param Pt :: Point to rotate
  */
{
  void (transComp::*fn)(Geometry::Vec3D&) const =  &transComp::apply;
  for_each(Transforms.begin(),Transforms.end(),
	   std::bind<void>(fn,std::placeholders::_1,
			     std::ref(Pt)));
  return;
}

void
localRotate::applyFullAxis(Geometry::Vec3D& Pt) const
  /*!
    Apply full rotations:
    \param Pt :: Point to rotate
  */
{  
  axisRotate(Pt);
  return;
}

void
localRotate::reset()
  /*!
    Clear all rotations
  */
{
  Transforms.clear();
  return;
}
