/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/activeFluxPt.cxx
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
#include <algorithm>
#include <boost/format.hpp>
#include <cmath>
#include <complex>
#include <fstream> 
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "FileReport.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "activeFluxPt.h"

namespace SDef
{

activeFluxPt::activeFluxPt(const int CN,
                           Geometry::Vec3D  Pt) :
  cellN(CN),fluxPt(std::move(Pt))
  /*!
    Constructor 
    \param CN :: cell Number
    \param Pt :: Flux point
  */
{}

  
activeFluxPt::activeFluxPt(const activeFluxPt& A) :
  cellN(A.cellN),fluxPt(A.fluxPt) 
  /*!
    Copy Constructor 
    \param A :: activeFluxPt to copy
  */
{}

activeFluxPt&
activeFluxPt::operator=(const activeFluxPt&)
  /*!
    Assignement operator
    \param A :: activeFluxPt to copy
    \return *this
  */
{
  return *this;
}  

activeFluxPt::~activeFluxPt() 
  /*!
    Destructor
  */
{}



  
  

  
} // NAMESPACE SDef
