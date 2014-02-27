/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/boxValues.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <cmath>
#include <complex> 
#include <vector>
#include <map> 
#include <list> 
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
 
#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "boxValues.h"

namespace ModelSupport
{

boxValues::boxValues(const size_t NS,const int& M,
		     const double& T) :
  NSides(NS),setFlag(0),
  SDist(NS),SAxis(NS),MatN(M),Temp(T)
  /*!
    Constructor
    \param NS :: Number of sides
    \param M :: Material number
    \param T :: Temperature (kelvin)
   */
{
  if (NS>10)
    throw ColErr::RangeError<size_t>
      (NS,0,10,"boxValues::Too many sides specified");
}

boxValues::boxValues(const boxValues& A) : 
  NSides(A.NSides),setFlag(A.setFlag),SDist(A.SDist),
  SAxis(A.SAxis),MatN(A.MatN),Temp(A.Temp)
  /*!
    Copy constructor
    \param A :: boxValues to copy
  */
{}

boxValues&
boxValues::operator=(const boxValues& A)
  /*!
    Assignment operator
    \param A :: boxValues to copy
    \return *this
  */
{
  if (this!=&A)
    {
      NSides=A.NSides;
      setFlag=A.setFlag;
      SDist=A.SDist;
      SAxis=A.SAxis;
      MatN=A.MatN;
      Temp=A.Temp;
    }
  return *this;
}

void
boxValues::symPlanes(const double& XD,const double& ZD)
  /*!
    Calculate the relative vectors and angles
    \param XD :: X distances
    \param ZD :: Z distances
   */
{
  ELog::RegMethod RegA("boxValues","symPlanes");
  
  const Geometry::Vec3D XUnit(1,0,0);
  const Geometry::Vec3D ZUnit(0,0,1);

  const double angleStep(2.0*M_PI/static_cast<double>(NSides));
  double Angle=0.0;
  for(size_t i=0;i<NSides;i++)
    {
      SDist[i]=fabs(cos(Angle)*XD+sin(Angle)*ZD);
      SAxis[i]=XUnit*cos(Angle)+ZUnit*sin(Angle);
      Angle+=angleStep;
    }
  setFlag=1;
  return; 
} 

double
boxValues::getExtent() const
  /*!
    Determine the maximum extent
    \return Extent value
   */
{
  if  (SDist.empty())
    return 0.0;
  return *max_element(SDist.begin(),SDist.end());
}

Geometry::Vec3D
boxValues::getDatum(const size_t Index,
		    const Geometry::Vec3D& Origin,
		    const Geometry::Vec3D& XDir,
		    const Geometry::Vec3D& ZDir) const
  /*!
    Calculate the point on the plane 
    \param Index :: Side number
    \param Origin :: Origin
    \param XDir  :: X Unit vector
    \param ZDir  :: Z Unit vector
    \return Point on the side
  */
{
  ELog::RegMethod RegA("boxValues","getDatum");
  if (Index>=NSides || !setFlag)
    throw ColErr::IndexError<size_t>(Index,NSides,"Index");
  return Origin+getAxis(Index,XDir,ZDir)*SDist[Index];
}

Geometry::Vec3D
boxValues::getAxis(const size_t Index,
		   const Geometry::Vec3D& XDir,
		   const Geometry::Vec3D& ZDir) const
  /*!
    Calculate the point on the plane direction
    \param XDir  :: X Unit vector
    \param ZDir  :: Z Unit vector
  */
{
  ELog::RegMethod RegA("boxValues","getDatum");
  if (Index>=NSides || !setFlag)
    throw ColErr::IndexError<size_t>(Index,NSides,RegA.getFull());

      
   Geometry::Vec3D Axis(SAxis[Index]);
   return XDir*Axis.X()+ZDir*Axis.Z();
}


}  // NAMESPACE ModelSupport
