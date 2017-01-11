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
#include <boost/format.hpp>

#include "Exception.h"
#include "MersenneTwister.h"
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
#include "doubleErr.h"
#include "mathSupport.h"
#include "WorkData.h"
#include "activeFluxPt.h"

extern MTRand RNG;

namespace SDef
{

activeFluxPt::activeFluxPt(const int CN,
                           const Geometry::Vec3D& Pt) :
  cellN(CN),fluxPt(Pt)
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
