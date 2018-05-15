/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuildInc/F5Calc.cxx
 *
 * Copyright (c) 2015-2017 Konstantin Batkov 
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"

#include "F5Calc.h"


namespace tallySystem
{

F5Calc::F5Calc()
  /*!
    Constructor
  */
{}


F5Calc::~F5Calc()
  /*!
    Destructor
  */
{}
  
  
  
void
F5Calc::SetTally(double lx,double ly,double lz)
   /*!
     Simple seter
     \param lx ::  ??
     \param ly ::  ??
     \param lz ::  ??
   */
{
  F5(lx,ly,lz);
  return;
}
  
void
F5Calc::SetPoints(const Geometry::Vec3D& lB,
		  const Geometry::Vec3D& lC,
		  const Geometry::Vec3D& lB2)
   /*!
     Simple seter
     \param lB ::  ??
     \param lC ::  ??
     \param lB2 ::  ??
   */
{
  B = lB;
  C = lC;
  B2 = lB2;
  return;
}

void
F5Calc::SetLength(const double laf)
{
  CalculateMiddleOfModerator();
  af=laf;
  
  const double mf=F5.Distance(M);
  A=F5+(M-F5)*(af/mf);
  return;
}

void
F5Calc::CalculateMiddleOfModerator()
{
  M=C+((B-C)+(B2-B))/2.0;
  return;
}
  
double
F5Calc::GetZAngle()
  
  /*!
    The angle is calculated between the plane, which is normal 
    to the moderator surface and includes point M, and the line M->F5
    The angle is negative when the F5 tally is below the plane
  */
{

  Geometry::Vec3D lF5_Projected;
  //Normalvector of the plane containing B,C and B2
  Geometry::Vec3D lNV; 
  Geometry::Vec3D lNV2; 
  Geometry::Vec3D lM;

  lM=M+(B-C)/2.0;

  lNV=CalculateNormalVector(B,C,B2);

  lNV2=lNV*(lM-M);
  lNV2.makeUnit();

  lF5_Projected=CalculateProjectionOntoPlane(lNV2,M,F5); 
  
  const double lAngle=(180.0/M_PI)*
    fabs(atan(F5.Distance(lF5_Projected))/
	 M.Distance(lF5_Projected));

  // This is junk: Should be relative to the lF5-projection anlge
  return (lF5_Projected.Z()-F5.Z() >= 0.0) ? -lAngle : lAngle;

}
  
double F5Calc::GetXYAngle()

  /*!
    The angle is calculated between
    1. the line consisting of
    a. M (middle of the moderator)
    b. lF5_Projected (the projection of the F5 tally onto the plane which is 
    perpendicular to the moderator surface and includes M)
    2. the line which is perpendicular to the moderator surface and 
    goes through M (middle of the moderator)
  */
{
  return 180.0+atan2(F5.Y()-M.Y(), F5.X()-M.X())*180/M_PI;
}

double
F5Calc::GetHalfSizeX() const
  /*!
    This is a horrible way to to this -- use standard pythgerous.
   */
{
  double val = B.Distance(C)/2.0*af/F5.Distance(M);

  const double ab = (F5.X()-M.X())*(B.X()-C.X()) +
    (F5.Y()-M.Y())*(B.Y()-C.Y());

  const double len =
    sqrt(pow(F5.X()-M.X(),2)+pow(F5.Y()-M.Y(),2)) *
    sqrt(pow(B.X()-C.X(),2)+pow(B.Y()-C.Y(),2));
  
  const double cosa = ab/len;
  val *= cos(M_PI/2.0-acos(cosa));
  return val;
};
  
double
F5Calc::GetHalfSizeZ() const
{
  return B.Distance(B2)/2.0*af/F5.Distance(M);
}

Geometry::Vec3D
F5Calc::CalculateNormalVector(const Geometry::Vec3D& O,
			      const Geometry::Vec3D& P1,
			      const Geometry::Vec3D& P2) const
   // STATIC
{
  // bizare way to do cross product normals
  const Geometry::Vec3D pA=O-P2;
  const Geometry::Vec3D pB=O-P1;
  return (pA*pB).unit();
}


Geometry::Vec3D
F5Calc::CalculateProjectionOntoPlane
    (const Geometry::Vec3D& NV,
     const Geometry::Vec3D& M,
     const Geometry::Vec3D& P) const
  /*!
    \param NV :: the normal vector of the plane
    \param M :: a point on the plane
    \param P :: a point which projection (P2) is calculated on the plane
*/
{

    const double lDistance=fabs(NV.dotProd(M-P)/NV.abs());
    //Calculate the possible projections
    return P+NV*lDistance;
}

  
}  // NAMESPACE tallySystem 
