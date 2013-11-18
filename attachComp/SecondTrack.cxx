/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   attachComp/SecondTrack.cxx
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
#include <boost/shared_ptr.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "Rules.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"

namespace attachSystem
{

SecondTrack::SecondTrack()
 /*!
    Constructor 
  */
{}

SecondTrack::SecondTrack(const SecondTrack& A) : 
  bX(A.bX),bY(A.bY),bZ(A.bZ),bEnter(A.bEnter),bExit(A.bExit)
  /*!
    Copy constructor
    \param A :: SecondTrack to copy
  */
{}

SecondTrack&
SecondTrack::operator=(const SecondTrack& A)
  /*!
    Assignment operator
    \param A :: SecondTrack to copy
    \return *this
  */
{
  if (this!=&A)
    {
      bX=A.bX;
      bY=A.bY;
      bZ=A.bZ;
      bEnter=A.bEnter;
      bExit=A.bExit;
    }
  return *this;
}

void
SecondTrack::createUnitVector(const FixedComp& FC)
  /*!
    Create the unit vectors
  */
{
  ELog::RegMethod RegA("SecondTrack","createUnitVector");

  bX=FC.getX();
  bY=FC.getY();
  bZ=FC.getZ();
  bEnter=FC.getCentre();

  return;
}

void
SecondTrack::createUnitVector(const SecondTrack& SC)
  /*!
    Create the unit vectors
    \param SC :: Relative to another SecondTrack
  */
{
  ELog::RegMethod RegA("SecondTrack","createUnitVector(SecondTrack)");

  bX=SC.bX;
  bY=SC.bY;
  bZ=SC.bZ;
  bEnter=SC.bExit;

  return;
}

void 
SecondTrack::setBeamExit(const Geometry::Vec3D& C,
			 const Geometry::Vec3D& A) 
  /*!
    User Interface to LU[1] to set Point + Axis
    \param C :: Connect point
    \param A :: Axis
  */
{
  ELog::RegMethod RegA("SecondComp","setBeamSurf");

  LExit.setAxis(A);
  LExit.setConnectPt(C);
  bExit=C;
  bY=A;
  return;
}

int
SecondTrack::getExitSurf() const
  /*!
    Accessor to the exit surface number
    \return Surface Key number
  */
{
  ELog::RegMethod RegA("SecondTrack","getExitSurf");
  
  return LExit.getLinkSurf();
}

const Geometry::Vec3D&
SecondTrack::getExitAxis() const
  /*!
    Accessor to the link axis
    \return Link Axis
  */
{
  ELog::RegMethod RegA("SecondTrack","getExitAxis");
  
  return LExit.getAxis();
}


std::string
SecondTrack::getExitString() const
  /*!
    Accessor to the link surface string
    \return String of link
  */
{
  ELog::RegMethod RegA("SecondTrack","getLinkSurf");
  
  return LExit.getLinkString();
}

void
SecondTrack::applyBeamAngleRotate(const double xyAngle,
				  const double zAngle)
/*!
  Apply a rotation to the beam angle
  \param xyAngle :: XY Rotation [second]
  \param zAngle :: Z Rotation [first]
*/
{
  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRotDeg(zAngle,bX);
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRotDeg(xyAngle,bZ);
  Qz.rotate(bY);
  Qz.rotate(bZ);
  Qxy.rotate(bY);
  Qxy.rotate(bX);
  Qxy.rotate(bZ);
  return;
}
  
void
SecondTrack::applyRotation(const Geometry::Vec3D& Axis,
			   const double Angle)
  /*!
    Apply a rotation to the basis set
    \param Axis :: rotation axis 
    \param Angle :: rotation angle
  */
{
  ELog::RegMethod RegA("SecondTrack","applyRotation");

  const Geometry::Quaternion Qrot=
    Geometry::Quaternion::calcQRotDeg(Angle,Axis.unit());
  
  Qrot.rotate(bX);
  Qrot.rotate(bY);
  Qrot.rotate(bZ);
  return;
}


  
}  // NAMESPACE attachSystem
