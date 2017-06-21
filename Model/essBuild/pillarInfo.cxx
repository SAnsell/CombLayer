/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/pillarInfo
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
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "pillarInfo.h"

namespace essSystem
{

pillarInfo::pillarInfo(const std::string& S,
		       const size_t RNum,
		       const size_t SNum,
		       const int RIndex,
		       const int A,
		       const Geometry::Vec3D& CP,
		       const Geometry::Vec3D& YA) :
  Name(S),radN(RNum),sectN(SNum),
  RI(RIndex),active(A),centPoint(CP),YAxis(YA.unit())
  /*!
    Constructor
    \param S :: Name
    \param RNum :: Radial number
    \param SNum :: Sector number
    \param RIndex :: Offset number
    \param A :: Active flag
    \param CP :: Central point
    \param YA :: YAxis direction
  */
{}

pillarInfo::pillarInfo(const pillarInfo& A) :
  Name(A.Name),radN(A.radN),sectN(A.sectN),
  RI(A.RI),active(A.active),
  centPoint(A.centPoint),YAxis(A.YAxis)
  /*!
    Copy constructor
    \param A :: pillarInfo object to construct
  */
{}

pillarInfo&
pillarInfo::operator=(const pillarInfo& A)
  /*!
    Assignment operator
    \param A :: pillarInfo object to construct
    \return *this
  */
{
  if (this!=&A)
    {
      centPoint=A.centPoint;
      YAxis=A.YAxis;
    }
  return *this;
}
  
std::string
pillarInfo::getNext(const long int dirR,const long int dirX) const
  /*!
    Get the linking pillar based on +dir +dirX
    \param dirR :: Increase / decrease in R
    \param dirX :: Increase / decrease in S
    \return String
  */
{
  ELog::RegMethod RegA("pillarInfo","getNext");
  
  const long int RN(static_cast<long int>(radN)+dirR);
  const long int SN(static_cast<long int>(sectN)+dirX);

  if (RN<0 || SN<0)
    throw ColErr::RangeError<long int>(0,RN,SN,"RN/SN < 0");
  const std::string Out="R_"+std::to_string(RN)+
	 "S_"+std::to_string(SN);
  
  return Out;
}

  
}  // NAMESPACE essSystem
