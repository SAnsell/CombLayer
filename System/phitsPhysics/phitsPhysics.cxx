/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsPhysics/phitsPhysics.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <functional>
#include <memory>
#include <array>
#include <tuple>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "writeSupport.h"
#include "MapRange.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"

#include "particleConv.h"
#include "cellValueSet.h"
#include "pairValueSet.h"
#include "phitsPhysics.h"

namespace phitsSystem
{
		       
phitsPhysics::phitsPhysics() 
  /*!
    Constructor
  */
{}

phitsPhysics::phitsPhysics(const phitsPhysics& A)   
  /*!
    Copy constructor
    \param A :: phitsPhysics to copy
  */
{}

phitsPhysics&
phitsPhysics::operator=(const phitsPhysics& A)
  /*!
    Assignment operator
    \param A :: phitsPhysics to copy
    \return *this
  */
{
  if (this!=&A)
    {
    }
  return *this;
}

phitsPhysics::~phitsPhysics()
  /*!
    Destructor
  */
{}

  
} // NAMESPACE phitsSystem
      
   
