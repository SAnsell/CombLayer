/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photon/plateInfo.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "plateInfo.h"

namespace photonSystem
{
      
plateInfo::plateInfo() :
  thick(0.0),vHeight(0.0),vWidth(0.0),mat(0),temp(0.0)
  /*!
    Constructor
  */
{}

plateInfo::plateInfo(const std::string& N) :
  name(N),thick(0.0),vHeight(0.0),vWidth(0.0),mat(0),temp(0.0)
  /*!
    Constructor
    \param N :: name
  */
{}

plateInfo::plateInfo(const plateInfo& A) : 
  name(A.name),thick(A.thick),vHeight(A.vHeight),
  vWidth(A.vWidth),mat(A.mat),temp(A.temp)
  /*!
    Copy constructor
    \param A :: plateInfo to copy
  */
{}

plateInfo&
plateInfo::operator=(const plateInfo& A)
  /*!
    Assignment operator
    \param A :: plateInfo to copy
    \return *this
  */
{
  if (this!=&A)
    {
      name=A.name;
      thick=A.thick;
      vHeight=A.vHeight;
      vWidth=A.vWidth;
      mat=A.mat;
      temp=A.temp;
    }
  return *this;
}

}  // NAMESPACE photonSystem
