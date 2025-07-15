/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/Ellipse.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#include <algorithm>
#include <complex>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "FileReport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"

#include "Vec3D.h"
#include "polySupport.h"
#include "Line.h"
#include "Intersect.h"
#include "Ellipse.h"

namespace Geometry
{

std::ostream&
operator<<(std::ostream& OX,const Ellipse& A)
{
  A.write(OX);
  return OX;
}

Ellipse::Ellipse() :
  Intersect(),
  index(0),Cent(0,0,0),
  a2(1.0),b2(1.0),
  majorAxis(0,1,0),minorAxis(1,0,0),
  NormV(0,0,1)
  /*!
    Default Constructor 
  */
{}  

  Ellipse::Ellipse(Geometry::Vec3D C,
		 const Geometry::Vec3D& mxAxis,
		 const Geometry::Vec3D& mnAxis) :
  index(0),Cent(std::move(C)),
  a2(mxAxis.abs()),
  b2(mnAxis.abs()),
  majorAxis(mxAxis),
  minorAxis(mnAxis),
  NormV((majorAxis*minorAxis).unit())
  /*!
    Constructor 
    \param C :: Centre
    \param mnA :: minimum Axis [orthogonal to mxA]
    \param mxA :: maximum Axis 
  */
{
  if (a2<b2)
    {
      std::swap(a2,b2);
      std::swap(majorAxis,minorAxis);
    }
  minorAxis=minorAxis.cutComponent(majorAxis);
  majorAxis.makeUnit();
  b2=minorAxis.makeUnit();
}  

Ellipse::Ellipse(Geometry::Vec3D C,
		 const Geometry::Vec3D& mxAxis,
		 const Geometry::Vec3D& mnAxis,
		 const Geometry::Vec3D& nV) :
  Ellipse(C,mxAxis,mnAxis)
  /*!
    Constructor 
    \param C :: Centre
    \param mnA :: minimum Axis
    \param mxA :: maximum Axis
    \param nV :: Normal Axis
  */
{
  if (NormV.dotProd(nV)<0.0)
    NormV *= -1.0;
}  

Ellipse::Ellipse(const int I,
		 Geometry::Vec3D C,
		 const Geometry::Vec3D& mxAxis,
		 const Geometry::Vec3D& mnAxis,
		 const Geometry::Vec3D& nV) :
  Ellipse(C,mxAxis,mnAxis,nV)
{
  index=I;
}
  
Ellipse::Ellipse(const Ellipse& A) :
  Intersect(A),
  index(A.index),Cent(A.Cent),
  a2(A.a2),b2(A.b2),
  majorAxis(A.majorAxis),
  minorAxis(A.minorAxis),
  NormV(A.NormV)
 /*!
    Copy Constructor 
    \param A :: Ellipse object 
  */
{}

Ellipse&
Ellipse::operator=(const Ellipse& A) 
  /*! 
    Assignment constructor
    \param A :: Ellipse object 
    \return *this
  */
{
  if (this!=&A)
    {
      Intersect::operator=(A);
      index=A.index;
      Cent=A.Cent;
      a2=A.a2;
      b2=A.b2;
      minorAxis=A.minorAxis;
      majorAxis=A.majorAxis;
      NormV=A.NormV;
    }
  return *this;
}


Ellipse&
Ellipse::operator()(const Geometry::Vec3D& C,
		    const Geometry::Vec3D& mnA,
		    const Geometry::Vec3D& mxA,
		    const Geometry::Vec3D& nV) 
  /*!
    Operator() cast
    \param C :: Centre
    \param mnA :: minimum Axis
    \param mxA :: maximum Axis
    \param nV :: Normal Axis
    \return *this
  */
{
  Cent=C;
  a2=mxA.abs();
  b2=mnA.abs();
  minorAxis=mnA;
  NormV=minorAxis*majorAxis;
  if (NormV.dotProd(nV)<0.0)
    NormV*= -1.0;
  return *this;
}

Geometry::Vec3D
Ellipse::ParamPt(const double P) const
  /*!
    Get a parametric point out the system :
    \param P :: Parameter value [0 : 2pi ]
    \return Point 
  */
{
   return Cent+minorAxis*(b2*cos(P))+
    majorAxis*(a2*sin(P));
}

int
Ellipse::side(const Geometry::Vec3D& A) const
  /*!
    Given a 3d Point -- project along Z axis
    and then determin if the point is inside
    on / outside the ellipse
    \param A :: Test Point
    \return 1 if outside -1 if inside and 0 if on ellipse
  */
{
  // axis are units:
  const Geometry::Vec3D APt=A-Cent;
  const double minorDist=APt.dotProd(minorAxis);
  const double majorDist=APt.dotProd(majorAxis);

  const double rAbs=
    (majorDist*majorDist)/(a2*a2)+
    (minorDist*minorDist)/(b2*b2);
  if (std::abs(rAbs-1.0)<Geometry::zeroTol)
    return 0;
  return (rAbs>1.0) ? 1 : -1;
}

double
Ellipse::scaleFactor(const Geometry::Vec3D& A) const
  /*!
    Given a 3d Point -- project along Z axis
    and then determine the scale factor of this ellipse
    to convert to that point to be on the ellipse.
    \param A :: Test Point
    \return scale factor needed to scale the ellipse to
    be on the point
  */
{
  Geometry::Vec3D APt=A-Cent;
  const double minorDist=APt.dotProd(minorAxis);
  const double majorDist=APt.dotProd(majorAxis);

  const double rAbs=
    (majorDist*majorDist*b2*b2+
     minorDist*minorDist*a2*a2)/(a2*a2*b2*b2);
  
  return std::sqrt(rAbs);
}

void
Ellipse::scale(const double SF)
{
  a2*=SF;
  b2*=SF;
  return;
}

size_t
Ellipse::lineIntercept(const Geometry::Line& L,
		       Geometry::Vec3D& aPt,
		       Geometry::Vec3D& bPt) const
  /*!
    Calculate the intersect between a line and the ellipse
    \param L :: Line
  */
{
  Geometry::Vec3D lineOrg=L.getOrigin();
  Geometry::Vec3D lineAxis=L.getDirect();

  // reduce line to the plane
  lineOrg-=Cent;
  lineOrg=lineOrg.cutComponent(NormV);
  lineAxis=lineAxis.cutComponent(NormV);
  lineAxis.makeUnit();


  if (lineAxis.abs()<1e-12) return 0;
  // calc x,y components of line in the direction of the
  // axis const. Calc both to deal with 1/0 issue by using
  // eq x=my+c or y=mx+c
  const double lineX=lineOrg.dotProd(majorAxis);
  const double lineY=lineOrg.dotProd(minorAxis);

  const double lineAX=lineAxis.dotProd(majorAxis);
  const double lineAY=lineAxis.dotProd(minorAxis);

  // results:
  double xA,xB,yA,yB;

  if (std::abs(lineAY)<std::abs(lineAX))
    {
      const double m=lineAY/lineAX;
      const double c=lineY;
      const double aa=b2*b2+a2*a2*m*m;
      const double bb=2.0*a2*a2*m*c;
      const double cc=a2*a2*c*c-a2*a2*b2*b2;

      const size_t out=
	solveRealQuadratic(aa,bb,cc,xA,xB);

      if (!out) return 0;
      yA=m*xA+c;
      yB=m*xB+c;
      aPt=majorAxis*xA+minorAxis*yA;
      bPt=majorAxis*xB+minorAxis*yB;
      return 2;
    }
  else if (std::abs(lineAY)>1e-12)
    {
      const double m=lineAX/lineAY;
      const double c=lineX;
      const double aa=a2*a2+b2*b2*m*m;
      const double bb=2.0*b2*b2*m*c;
      const double cc=b2*b2*c*c-a2*a2*b2*b2;
      const size_t out=
	solveRealQuadratic(aa,bb,cc,yA,yB);
      if (!out) return 0;
      xA=m*yA+c;
      xB=m*yB+c;
      aPt=majorAxis*xA+minorAxis*yA;
      bPt=majorAxis*xB+minorAxis*yB;
      return 2;
    }
  return 0;
}

  
void
Ellipse::write(std::ostream& OX) const
  /*!
    Debug write out1
    \param OX :: Output stream
  */
{
  OX<<"Ellipse: ("<<index<<") C:"<<Cent
    <<" Maj ["<<a2<<"]: "<<majorAxis
    <<" Min ["<<b2<<"]: "<<minorAxis
    <<" Norm "<<NormV;
  return;
}

} // NAMESPACE Geometry
