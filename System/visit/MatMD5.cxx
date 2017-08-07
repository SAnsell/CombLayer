/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   visit/MatMD5.cxx
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
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "masterWrite.h"
#include "MatMD5.h"

std::ostream&
operator<<(std::ostream& OX,const MatMD5& A)
  /*!
    Write to a stream
    \param OX :: Output stream
    \param A :: MatMD5 to write
    \return Stream state
  */
{
  A.write(OX);
  return OX;
}

MatMD5::MatMD5() :
  N(0)
  /*!
    Constructor
  */
{}

MatMD5::MatMD5(const MatMD5& A) : 
  N(A.N),sumXYZ(A.sumXYZ),sqrXYZ(A.sqrXYZ)
  /*!
    Copy constructor
    \param A :: MatMD5 to copy
  */
{};

MatMD5&
MatMD5::operator=(const MatMD5& A)
  /*!
    Assignment operator
    \param A :: MatMD5 to copy
    \return *this
  */
{
  if (this!=&A)
    {
      N=A.N;
      sumXYZ=A.sumXYZ;
      sqrXYZ=A.sqrXYZ;
    }
  return *this;
}

MatMD5::~MatMD5()
  /*!
    Destructor
   */
{}

void
MatMD5::addUnit(const Geometry::Vec3D& Pt)
  /*!
    Add a specific point
    \param Pt :: Point to add
   */
{
  sumXYZ+=Pt;
  for(int i=0;i<3;i++)
    sqrXYZ[i]+=Pt[i]*Pt[i];
  N++;
  return;
}

void
MatMD5::write(std::ostream& OX) const 
  /*!
    Write the object to a string
    \param OX :: Output stream
  */
{
  if (N)
    {
      masterWrite& MW=masterWrite::Instance();
      OX<<N<<" ";
      for(int i=0;i<3;i++)
	OX<<MW.Num(sumXYZ[i]/static_cast<double>(N))<<" ";
      for(int i=0;i<3;i++)
	OX<<MW.Num(sqrXYZ[i]/static_cast<double>(N))<<" ";
    }
  return;
}

