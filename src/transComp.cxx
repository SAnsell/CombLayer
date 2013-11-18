/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   src/transComp.cxx
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
#include <boost/bind.hpp>

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
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "transComp.h"


transRot::transRot() : 
  transComp(),
  Axis(0,0,1),Offset(0,0,0),Angle(0.0),
  QVec(Geometry::Quaternion::calcQRotDeg(0.0,Axis)),
  MRot(QVec.rMatrix()),
  QRev(Geometry::Quaternion::calcQRotDeg(0.0,Axis)),
  MRev(QRev.rMatrix())
  /*!
    Constructor: 
    Note the that zero rotation has the same
    equivilent inverse
  */
{}

transRot::transRot(const Geometry::Vec3D& A,
		   const Geometry::Vec3D& O,
		   const double ang) : 
  transComp(),Axis(A),Offset(O),Angle(ang),
  QVec(Geometry::Quaternion::calcQRotDeg(ang,A)),
  MRot(QVec.rMatrix()),
  QRev(Geometry::Quaternion::calcQRotDeg(-ang,A)),
  MRev(QRev.rMatrix())
  /*!
    Constructor
    \param A :: Axis to rotate about
    \param O :: Offset
    \param ang :: Angle
  */
{}

transRot::transRot(const transRot& A) : transComp(A),
  Axis(A.Axis),Offset(A.Offset),Angle(A.Angle),
  QVec(A.QVec),MRot(A.MRot),QRev(A.QRev),MRev(A.MRev)
  /*!
    Copy Constructor
    \param A :: transRot to copy
  */
{}

transRot&
transRot::operator=(const transRot& A)
  /*!
    Assignment operator
    \param A :: transRot to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Axis=A.Axis;
      Offset=A.Offset;
      Angle=A.Angle;
      QVec=A.QVec;
      MRot=A.MRot;
      QRev=A.QRev;
      MRev=A.MRev;
    }
  return *this;
}

transRot*
transRot::clone() const
  /*!
    Clone function
    \return new(*this)
   */
{
  return new transRot(*this);
}

Geometry::Vec3D
transRot::calc(const Geometry::Vec3D& V) const
  /*!
    Calc a rotation 
    \param V :: Vector to move
    \return Vector after rotation
  */
{
  Geometry::Vec3D Vpt(V);
  this->apply(Vpt);
  return Vpt;
}
  
Geometry::Vec3D
transRot::calcAxis(const Geometry::Vec3D& V) const
  /*!
    Calc a rotation as an axis only
    \param V :: Vector to move
    \return Axis after rotation
  */
{
  Geometry::Vec3D Vpt(V);
  this->applyAxis(Vpt);
  return Vpt;
}

void
transRot::apply(Geometry::Surface* SPtr) const
  /*!
    Calc a rotation of a surface
    \param SPtr :: Surface to transform
  */
{
  SPtr->displace(-Offset);
  SPtr->rotate(MRot);
  SPtr->displace(Offset);
  return;
}

void
transRot::apply(MonteCarlo::Object* OPtr) const
  /*!
    Calc a rotation of a surface
    \param OPtr :: Object to transform
  */
{
  OPtr->displace(-Offset);
  OPtr->rotate(MRot);
  OPtr->displace(Offset);
  return;
}

void
transRot::apply(Geometry::Vec3D& V) const
  /*!
    Calc a rotation of a vector
    \param V :: Vector to rotate
  */
{
  V-=Offset;
  QVec.rotate(V);
  V+=Offset;
  return;
}

void
transRot::applyAxis(Geometry::Vec3D& V) const
  /*!
    Calc a rotation of an axis vector
    \param V :: Vector to rotate
  */
{
  QVec.rotate(V);
  return;
}

Geometry::Vec3D
transRot::calcRev(const Geometry::Vec3D& V) const
  /*!
    Calc a reverse rotation 
    \param V :: Vector to move
    \return Vector after rotation
  */
{
  Geometry::Vec3D Vpt(V);
  this->reverse(Vpt);
  return Vpt;
}
  
Geometry::Vec3D
transRot::calcRevAxis(const Geometry::Vec3D& V) const
  /*!
    Calc a reverse rotation as an axis only
    \param V :: Vector to move
    \return Axis after rotation
  */
{
  Geometry::Vec3D Vpt(V);
  this->reverseAxis(Vpt);
  return Vpt;
}

void
transRot::reverse(Geometry::Surface* SPtr) const
  /*!
    Calc a reverse rotation of a surface
    \param SPtr :: Surface to transform
  */
{
  SPtr->displace(-Offset);
  SPtr->rotate(MRev);
  SPtr->displace(Offset);
  return;
}

void
transRot::reverse(MonteCarlo::Object* OPtr) const
  /*!
    Calc a reverse rotation of a surface
    \param OPtr :: Object to transform
  */
{
  OPtr->displace(-Offset);
  OPtr->rotate(MRev);
  OPtr->displace(Offset);
  return;
}

void
transRot::reverse(Geometry::Vec3D& V) const
  /*!
    Calc a reverse rotation of a vector
    \param V :: Vector to rotate
  */
{
  V-=Offset;
  QRev.rotate(V);
  V+=Offset;
  return;
}

void
transRot::reverseAxis(Geometry::Vec3D& V) const
  /*!
    Calc a reverse rotation of an axis vector
    \param V :: Vector to rotate
  */
{
  QRev.rotate(V);
  return;
}

// -------------------------------------------------------------------
//             TRANSMIRROR
// -------------------------------------------------------------------

transMirror::transMirror() : transComp()
  /*!
    Constructor
  */
{}

transMirror::transMirror(const Geometry::Plane& A) : transComp(),
   Mirror(A)						     
  /*!
    Constructor
    \param A :: Axis to rotate about
  */
{}

transMirror::transMirror(const transMirror& A) : transComp(A),
						 Mirror(A.Mirror)					 
  /*!
    Copy Constructor
    \param A :: transMirror to copy
  */
{}

transMirror&
transMirror::operator=(const transMirror& A)
  /*!
    Assignment operator
    \param A :: transMirror to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Mirror=A.Mirror;
    }
  return *this;
}

transMirror*
transMirror::clone() const
  /*!
    Clone function
    \return new(*this)
   */
{
  return new transMirror(*this);
}

Geometry::Vec3D
transMirror::calc(const Geometry::Vec3D& V) const
  /*!
    Calc a rotation 
    \param V :: Vector to move
    \return Vector after rotation
  */
{
  Geometry::Vec3D Vpt(V);
  this->apply(Vpt);
  return Vpt;
}
  
Geometry::Vec3D
transMirror::calcAxis(const Geometry::Vec3D& V) const
  /*!
    Calc a rotation as an axis only
    \param V :: Vector to move
    \return Axis after rotation
  */
{
  Geometry::Vec3D Vpt(V);
  this->applyAxis(Vpt);
  return Vpt;
}

void
transMirror::apply(Geometry::Surface* SPtr) const
  /*!
    Calc a rotation of a surface
    \param SPtr :: Surface to transform
  */
{
  // Note the reversal
  SPtr->mirror(Mirror);
  return;
}

void
transMirror::apply(MonteCarlo::Object* OPtr) const
  /*!
    Calc a rotation of a surface
    \param OPtr :: Object to transform
  */
{
  // Note the reversal
  OPtr->mirror(Mirror);
  return;
}

void
transMirror::apply(Geometry::Vec3D& V) const
  /*!
    Calc a rotation of a vector
    \param V :: Vector to rotate
  */
{
  Mirror.mirrorPt(V);
  return;
}


void
transMirror::applyAxis(Geometry::Vec3D& V) const
  /*!
    Calc a rotation of an axis vector
    \param V :: Vector to rotate
  */
{
  Mirror.mirrorAxis(V);
  return;
}

Geometry::Vec3D
transMirror::calcRev(const Geometry::Vec3D& V) const
  /*!
    Calc a reverse reflection
    \param V :: Vector to move
    \return Vector after rotation
  */
{
  Geometry::Vec3D Vpt(V);
  this->reverse(Vpt);
  return Vpt;
}
  
Geometry::Vec3D
transMirror::calcRevAxis(const Geometry::Vec3D& V) const
  /*!
    Calc a rotation as an axis only
    \param V :: Vector to move
    \return Axis after rotation
  */
{
  Geometry::Vec3D Vpt(V);
  this->reverseAxis(Vpt);
  return Vpt;
}

void
transMirror::reverse(Geometry::Surface* SPtr) const
  /*!
    Calc a rotation of a surface
    \param SPtr :: Surface to transform
  */
{
  // Note the reversal
  SPtr->mirror(Mirror);
  return;
}

void
transMirror::reverse(MonteCarlo::Object* OPtr) const
  /*!
    Calc a rotation of a surface
    \param OPtr :: Object to transform
  */
{
  // Note the reversal
  OPtr->mirror(Mirror);
  return;
}

void
transMirror::reverse(Geometry::Vec3D& V) const
  /*!
    Calc a reverse mirror of a vector
    \param V :: Vector to rotate
  */
{
  Mirror.mirrorPt(V);
  return;
}


void
transMirror::reverseAxis(Geometry::Vec3D& V) const
  /*!
    Calc a reverse mirror of an axis vector
    \param V :: Vector to rotate
  */
{
  Mirror.mirrorAxis(V);
  return;
}

// -------------------------------------------------------------------
//             TRANSDISPLACE
// -------------------------------------------------------------------

transDisplace::transDisplace() : transComp()
  /*!
    Constructor
  */
{}

transDisplace::transDisplace(const Geometry::Vec3D& A) : 
  transComp(),Offset(A)						     
  /*!
    Constructor
    \param A :: OffSet vector
  */
{}

transDisplace::transDisplace(const transDisplace& A) : 
  transComp(A),
  Offset(A.Offset)					 
  /*!
    Copy Constructor
    \param A :: transDisplace to copy
  */
{}

transDisplace&
transDisplace::operator=(const transDisplace& A)
  /*!
    Assignment operator
    \param A :: transDisplace to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Offset=A.Offset;
    }
  return *this;
}

transDisplace*
transDisplace::clone() const
  /*!
    Clone function
    \return new(*this)
   */
{
  return new transDisplace(*this);
}

Geometry::Vec3D
transDisplace::calc(const Geometry::Vec3D& V) const
  /*!
    Calc a rotation 
    \param V :: Vector to move
    \return Vector after rotation
  */
{
  Geometry::Vec3D Vpt(V);
  this->apply(Vpt);
  return Vpt;
}
  
Geometry::Vec3D
transDisplace::calcAxis(const Geometry::Vec3D& V) const
  /*!
    Calc a rotation as an axis only
    \param V :: Vector to move
    \return Axis after rotation
  */
{
  return V;
}

void
transDisplace::apply(Geometry::Surface* SPtr) const
  /*!
    Calc a rotation of a surface
    \param SPtr :: Surface to transform
  */
{
  SPtr->displace(Offset);
  return;
}

void
transDisplace::apply(MonteCarlo::Object* OPtr) const
  /*!
    Calc a rotation of a surface
    \param OPtr :: Object to transform
  */
{
  // Note the reversal
  OPtr->displace(Offset);
  return;
}

void
transDisplace::apply(Geometry::Vec3D& V) const
  /*!
    Calc a rotation of a vector
    \param V :: Vector to rotate
  */
{
  V+=Offset;
  return;
}

void
transDisplace::applyAxis(Geometry::Vec3D&) const
  /*!
    Calc a rotation of an axis vector
    \param :: Vector to rotate
  */
{
  return;
}

Geometry::Vec3D
transDisplace::calcRev(const Geometry::Vec3D& V) const
  /*!
    Calc a reversed displacement 
    \param V :: Vector to move
    \return Vector after rotation
  */
{
  Geometry::Vec3D Vpt(V);
  this->reverse(Vpt);
  return Vpt;
}
  
Geometry::Vec3D
transDisplace::calcRevAxis(const Geometry::Vec3D& V) const
  /*!
    Calc a reversed displacement  as an axis only
    \param V :: Vector to move
    \return Axis after displacement
  */
{
  return V;
}

void
transDisplace::reverse(Geometry::Surface* SPtr) const
  /*!
    Calc a rotation of a surface
    \param SPtr :: Surface to transform
  */
{
  SPtr->displace(-Offset);
  return;
}

void
transDisplace::reverse(MonteCarlo::Object* OPtr) const
  /*!
    Calc a rotation of a surface
    \param OPtr :: Object to transform
  */
{
  // Note the reversal
  OPtr->displace(-Offset);
  return;
}

void
transDisplace::reverse(Geometry::Vec3D& V) const
  /*!
    Calc a rotation of a vector
    \param V :: Vector to rotate
  */
{
  V-=Offset;
  return;
}

void
transDisplace::reverseAxis(Geometry::Vec3D&) const
  /*!
    Calc a rotation of an axis vector
    \param :: Vector to rotate
  */
{
  return;
}




