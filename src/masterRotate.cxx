/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/masterRotate.cxx
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
#include "masterRotate.h"

masterRotate::masterRotate() : 
  localRotate(),globalApplied(0)
  /*!
    Constructor
  */
{}

masterRotate&
masterRotate::Instance()
  /*!
    Singleton this
    \return masterRotate object
   */
{
  static masterRotate MR;
  return MR;
}

Geometry::Vec3D
masterRotate::calcRotate(const Geometry::Vec3D& V) const
  /*!
    Rotate a vector
    \param V :: Vector to rotate
    \return Vector rotated
  */
{
  return (globalApplied) ? V : localRotate::calcRotate(V);
}

Geometry::Vec3D
masterRotate::forceCalcRotate(const Geometry::Vec3D& V) const
  /*!
    Rotate a vector
    \param V :: Vector to rotate
    \return Vector rotated
  */
{
  return localRotate::calcRotate(V);
}


Geometry::Vec3D
masterRotate::calcAxisRotate(const Geometry::Vec3D& V) const
  /*!
    Rotate a vector
    \param V :: Vector to rotate
    \return Vector rotated
  */
{
  return (globalApplied) ? V : localRotate::calcAxisRotate(V);
}

Geometry::Vec3D
masterRotate::forceCalcAxisRotate(const Geometry::Vec3D& V) const
  /*!
    Rotate a vector
    \param V :: Vector to rotate
    \return Vector rotated
  */
{
  return localRotate::calcAxisRotate(V);
}


Geometry::Vec3D
masterRotate::reverseRotate(const Geometry::Vec3D& V) const
  /*!
    Rotate a vector
    \param V :: Vector to rotate
    \return Vector rotated
  */
{
  return (globalApplied) ? V : 
    localRotate::reverseRotate(V);
}

Geometry::Vec3D
masterRotate::forceReverseRotate(const Geometry::Vec3D& V) const
  /*!
    Rotate a vector
    \param V :: Vector to rotate
    \return Vector rotated
  */
{
  return localRotate::reverseRotate(V);
}


Geometry::Vec3D
masterRotate::reverseAxisRotate(const Geometry::Vec3D& V) const
  /*!
    Rotate a vector
    \param V :: Vector to rotate
    \return Vector rotated
  */
{
  return (globalApplied) ? V : 
    localRotate::reverseAxisRotate(V);
}

Geometry::Vec3D
masterRotate::forceReverseAxisRotate(const Geometry::Vec3D& V) const
  /*!
    Rotate a vector
    \param V :: Vector to rotate
    \return Vector rotated
  */
{
  return localRotate::reverseAxisRotate(V);
}

void
masterRotate::applyFull(Geometry::Surface* SPtr) const
  /*!
    Apply full rotations:
    \param SPtr :: Surface Pointer
   */
{
  if (!globalApplied)
    localRotate::applyFull(SPtr);

  return;
}


void
masterRotate::applyFull(MonteCarlo::Object* OPtr) const
  /*!
    Apply full rotations:
    \param OPtr :: Object pointer
   */
{
  if (!globalApplied)
    localRotate::applyFull(OPtr);

  return;
}

void
masterRotate::applyFull(Geometry::Vec3D& Pt) const
  /*!
    Apply full rotations:
    \param Pt :: Point to rotate
  */
{
  if (!globalApplied)
    localRotate::applyFull(Pt);
  return;
}

void
masterRotate::applyFullAxis(Geometry::Vec3D& Axis) const
  /*!
    Apply full rotations:
    \param Axis :: Apply axis rotation
   */
{
  if (!globalApplied)
    localRotate::applyFullAxis(Axis);

  return;
}
