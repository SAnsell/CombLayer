/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaProcess/plotGeom.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include <iomanip>
#include <fstream>
#include <cmath>
#include <complex>
#include <string>
#include <sstream>
#include <list>
#include <map>
#include <set>
#include <vector>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "writeSupport.h"
#include "plotGeom.h"

namespace flukaSystem
{
  
plotGeom::plotGeom() :
  title("Basic plot")
  /*!
    Constructor
  */
{}

plotGeom::plotGeom(const plotGeom& A) : 
  title(A.title),APoint(A.APoint),BPoint(A.BPoint),
  xUnit(A.xUnit),yUnit(A.yUnit)
  /*!
    Copy constructor
    \param A :: plotGeom to copy
  */
{}

plotGeom&
plotGeom::operator=(const plotGeom& A)
  /*!
    Assignment operator
    \param A :: plotGeom to copy
    \return *this
  */
{
  if (this!=&A)
    {
      title=A.title;
      APoint=A.APoint;
      BPoint=A.BPoint;
      xUnit=A.xUnit;
      yUnit=A.yUnit;
    }
  return *this;
}

plotGeom::~plotGeom()
  /*!
    Destructor
   */
{}


void 
plotGeom::setBox(const Geometry::Vec3D& APt,
		 const Geometry::Vec3D& BPt)
  /*!
    Set the box start and end point
    \param startPt :: Point on near corner
    \param endPt :: Point on opposite corner
  */
{
  APoint=APt;
  BPoint=BPt;
  // get min distance
  size_t index(0);
  double mD(1e38);
  for(size_t i=0;i<3;i++)
    {
      const double D=std::abs(BPt[i]-APt[i]);
      if (D<mD)
	{
	  index=i;
	  mD=D;
	}
    }
  xUnit=Geometry::Vec3D(0,0,0);
  xUnit[((index) ? 0 : 1)] =1.0;

  yUnit=Geometry::Vec3D(0,0,0);
  yUnit[((index!=2) ? 2 : 1)] =1.0;

  APoint[index]=(APoint[index]+BPoint[index])/2.0;
  BPoint[index]=APoint[index];
  return;
}

void
plotGeom::setTitle(const std::string& T)
  /*!
    Set a given visit output type
    \param T :: title
  */
{
  if (!T.empty())
    title=T;

  return;
}


void
plotGeom::write(std::ostream& OX) const
  /*!
    Write out a PLOTGEOM card for fluka
    \param OX :: Output stream
  */
{
  std::ostringstream cx;

  StrFunc::writeFLUKA("PLOTGEOM 0.0 1.0 0.0 0.0 79.0 0.0",OX);
  OX<<" "<<title.substr(0,78)<<"\n";
  
  cx<<APoint<<" "<<BPoint;
  StrFunc::writeFLUKA(cx.str(),OX);
  cx.str("");
  cx<<xUnit<<" "<<yUnit;
  StrFunc::writeFLUKA(cx.str(),OX);
  StrFunc::writeFLUKA("1.0 1.0 0.0",OX); 
  return;
}

} // NAMESPACE flukaSystem
