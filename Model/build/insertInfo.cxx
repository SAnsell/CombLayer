/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   build/insertInfo.cxx
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
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "InputControl.h"
#include "Element.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "insertInfo.h"

namespace shutterSystem
{

std::ostream&
operator<<(std::ostream& OX,const insertInfo& A)
  /*!
    Write to the standard output
    \param OX :: Output stream
    \param A :: insertInfo to write
    \return Stream
  */
{
  A.write(OX);
  return OX;
}

insertInfo::insertInfo(const int ln,const int rn) :
  lNum(ln),rNum(rn),matID(0),Doffset(0.0),Length(0.0),
  ORad(0.0),innerRID(0),outerRID(0),
  fPlane(0),bPlane(0)
 /*!
   Minimal Constructor 
   \param ln :: length number
   \param rn :: radius number
 */
{}

insertInfo::insertInfo(const int ln,const int rn,const int mN,
		       const double Offset,const double R,const double L) :
  lNum(ln),rNum(rn),matID(mN),Doffset(Offset),Length(L),
  ORad(R),innerRID(0),outerRID(0),
  fPlane(0),bPlane(0)
 /*!
   Constructor 
   \param ln :: length number
   \param rn :: radius number
   \param mN :: Material number
   \param Offset :: Front offest
   \param R :: Outer radius [ for external ]
   \param L :: Length
 */
{}

insertInfo::insertInfo(const insertInfo& A) :
  lNum(A.lNum),rNum(A.rNum),matID(A.matID),CPt(A.CPt),
  Axis(A.Axis),Doffset(A.Doffset),
  Length(A.Length),ORad(A.ORad),innerRID(A.innerRID),outerRID(A.outerRID),
  fPlane(A.fPlane),bPlane(A.bPlane),Sides(A.Sides)
 /*!
   Copy Constructor 
   \param A :: insertInfo object to copy
 */
  
{}

insertInfo::insertInfo(const int BP,const double L,const insertInfo& A) :
  lNum(A.lNum+1),rNum(A.rNum),matID(0),
  CPt(A.CPt),Axis(A.Axis),Doffset(A.Doffset+A.Length),
  Length(L),ORad(A.ORad),innerRID(A.innerRID),outerRID(A.outerRID),
  fPlane(A.bPlane),bPlane(BP),Sides(A.Sides)
 /*!
   Special constructor for a component that follows A
   \param BP :: backplane number
   \param L :: Length
   \param A :: insertInfo to follow from 
 */
{}

insertInfo&
insertInfo::operator=(const insertInfo& A) 
 /*!
   Assignment operator
   \param A :: insertInfo object to copy
   \return *this
 */
{
  if (this!=&A)
    {
      lNum=A.lNum;
      rNum=A.rNum;
      matID=A.matID;
      CPt=A.CPt;
      Axis=A.Axis;
      Doffset=A.Doffset;
      Length=A.Length;
      ORad=A.ORad;
      innerRID=A.innerRID;
      outerRID=A.outerRID;
      fPlane=A.fPlane;
      bPlane=A.bPlane;
      Sides=A.Sides;
    }
  return *this;
}

void
insertInfo::setCoordinates(const Geometry::Vec3D& C,
			   const Geometry::Vec3D& A)
  /*!
    Sets the coordinates
    \param C :: Centre
    \param A :: Axis
   */
{
  CPt=C;
  Axis=A;
  Axis.makeUnit();
  return;
}

Geometry::Vec3D
insertInfo::calcFrontPoint() const
  /*!
    Calc step point at the beginning of the system
    \return point of plane at far point
  */
{
  return CPt+Axis*Doffset;
}

Geometry::Vec3D
insertInfo::calcStepPoint() const
  /*!
    Calc step point
    \return point of plane at far point
  */
{
  return CPt+Axis*(Length+Doffset);
}

std::string
insertInfo::cellString() const
 /*!
   Calc cell string
   \return string for MCNPX
  */
{
  std::ostringstream cx;
  cx<<fPlane<<" "<<-bPlane;
  if (innerRID) cx<<" "<<innerRID;
  if (outerRID) cx<<" "<<-outerRID;
  cx<<" "<<Sides;
  return cx.str();
}

void
insertInfo::write(std::ostream& OX) const
  /*!
    Write out to a file
    \param OX :: Output stream
  */
{
  OX<<"Comp"<<rNum<<"_"<<lNum<<" "<<matID<<" "<<innerRID<<" "
    <<outerRID<<" "<<fPlane<<" "<<bPlane<<" : "
    <<Doffset<<" "<<Length<<" "<<ORad;
  return;
}

}  // NAMESPACE shutterSystem
