/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/Circle.cxx
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
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "Intersect.h"
#include "Circle.h"

namespace Geometry
{

Circle::Circle() : Intersect(),
  index(0),NormV(0,0,1),Radius(0.0)
  /*!
    Default Constructor 
  */
{}  

Circle::Circle(const Geometry::Vec3D& C,
	       const Geometry::Vec3D& NV,
	       const double R) :  
  Intersect(),index(0),Cent(C),NormV(NV.unit()),Radius(R)
  /*!
    Constructor 
    \param C :: Centre
    \param NV :: Normal
    \param R :: Radius
  */
{}  

Circle::Circle(const int I,
	       const Geometry::Vec3D& C,
	       const Geometry::Vec3D& NV,
	       const double R) : 
  Intersect(),index(I),Cent(C),NormV(NV.unit()),Radius(R)
  /*!
    Constructor 
    \param I :: Index value
    \param C :: Centre
    \param NV :: Normal
    \param R :: Radius
  */
{}  

Circle::Circle(const Circle& A) :
  Intersect(A),
  index(A.index),Cent(A.Cent),
  NormV(A.NormV),Radius(A.Radius)
 /*!
    Copy Constructor 
    \param A :: Circle object 
  */
{}

Circle&
Circle::operator=(const Circle& A) 
  /*! 
    Assignment constructor
    \param A :: Circle object 
    \return *this
  */
{
  if (this!=&A)
    {
      index=A.index;
      Cent=A.Cent;
      NormV=A.NormV;
      Radius=A.Radius;
    }
  return *this;
}


Circle&
Circle::operator()(const Geometry::Vec3D& C,
		   const Geometry::Vec3D& NV,
		   const double R) 
  /*!
    Operator() cast
    \param C :: Centre
    \param NV :: Normal
    \param R :: Radius
    \return *this
  */
{
  Cent=C;
  NormV=NV.unit();
  Radius=R;
  return *this;
}  

Geometry::Vec3D
Circle::ParamPt(const double P) const
  /*!
    Get a parametric point out the system :
    Note that only positive side of the lines
    \param P :: Parameter value [-inf : inf ]
    \return Point on circle
  */
{
  Geometry::Vec3D AAxis;
  Geometry::Vec3D BAxis;
  
  // normV not == x
  if ((1.0-NormV[0])>Geometry::zeroTol)
    {
      AAxis(NormV[2],NormV[1]*NormV[2]/(NormV[0]-1.0),
	    1.0+NormV[2]*NormV[2]/(NormV[0]-1.0));
      BAxis=AAxis*NormV;
    }
  else
    {
      AAxis(0,1,0);
      BAxis(0,0,1);
    }
  return Cent+AAxis*(Radius*cos(P))+BAxis*(Radius*sin(P));
}

void
Circle::write(std::ostream& OX) const
  /*!
    Debug write out1
    \param OX :: Output stream
  */
{
  OX<<"Circle: ("<<index<<"):"<<Cent<<" : "<<NormV<<" R: "<<Radius;
  return;
}


} // NAMESPACE Geometry
