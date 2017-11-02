/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/geomSupport.cxx
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
#include <complex>
#include <cmath>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "GTKreport.h"
#include "FileReport.h"
#include "NameStack.h"
#include "OutputLog.h"
#include "RegMethod.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Rules.h"
#include "HeadRule.h"
#include "SurInter.h"
#include "geomSupport.h"


namespace Geometry
{

std::tuple<Vec3D,Vec3D,Vec3D>
findCornerCircle(const HeadRule& zoneRule,
		 const Geometry::Plane& APln,
		 const Geometry::Plane& BPln,
		 const Geometry::Plane& ZPln,
		 const double radius)

  /*!
    Given a circle of radius , find the centre 
    relative to the two lines between A-Cpt and B-Cpt
    where Cpt is the intersection between Plane A-B.
    
    \parma zoneRule :: Two Plane rule for valid section
    \param APln :: Plane not parallel to other
    \param BPln :: Plane not parallel to other
    \param ZPln :: Z-plane : normal is axis of cylinder

    \param radius :: Radius of circle
    \return 
      - Centre point of circle of the cylinder intersecting Z plane :
      - left touch point of circle
      - right touch point of circle
    
  */
{
  ELog::RegMethod RegA("geomSupport[F]","findCornerCircle(Plane)");

  // corner point
  const Geometry::Vec3D CPt=SurInter::getPoint(&APln,&BPln,&ZPln);
  
  const Geometry::Vec3D& Zunit=ZPln.getNormal();
  const Geometry::Vec3D& Na=APln.getNormal();
  const Geometry::Vec3D& Nb=BPln.getNormal();

  const Geometry::Vec3D Za=(Na-Zunit*(Na.dotProd(Zunit))).unit();
  const Geometry::Vec3D Zb=(Nb-Zunit*(Nb.dotProd(Zunit))).unit();

  const Geometry::Vec3D Ma=Za*Zunit;
  const Geometry::Vec3D Mb=Zb*Zunit;

  // construct two points on the plane/lines that are valid to the headrule
  const Geometry::Vec3D PA=
    (zoneRule.isValid(CPt+Ma,APln.getName())) ? CPt+Ma : CPt-Ma;
  const Geometry::Vec3D PB=
    (zoneRule.isValid(CPt+Mb,BPln.getName())) ? CPt+Mb : CPt-Mb;

  return findCornerCircle(CPt,PA,PB,radius);
}

std::tuple<Vec3D,Vec3D,Vec3D>
findCornerCircle(const Geometry::Vec3D& C,
		 const Geometry::Vec3D& A,
		 const Geometry::Vec3D& B,
		 const double radius)
/*!
    Given a circle of radius R, find the centre 
    relative to the two lines between A-C and B-C
    
    Lines must not be parallel.
    
    \param C :: Corner
    \param A :: Extend point (1)
    \param B :: Extend point (2)
    \param radius :: Radius of circle
    \return Centre : Edge T1 : Edge T2
  */
{
  ELog::RegMethod RegA("geomSupport[F]","findCornerCircle<Vec3D>");

  const Geometry::Vec3D a=(A-C).unit();
  const Geometry::Vec3D b=(B-C).unit();

  const Geometry::Vec3D midDivide=((a+b)/2.0).unit();
  
  const double aDotM=a.dotProd(midDivide);
  const double lambda=sqrt((radius*radius)/(1.0-aDotM*aDotM));

  const Geometry::Vec3D RCent=midDivide*lambda;   // c out by step*midDivde
  const double alpha=a.dotProd(RCent);
  const double beta=b.dotProd(RCent);

  return std::tuple<Vec3D,Vec3D,Vec3D>
    (C+RCent,C+a*alpha,C+b*beta);
}

}  // NAMESPACE Geometry
