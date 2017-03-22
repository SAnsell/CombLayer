/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weight/MarkovProcess.cxx
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
#include <cmath>
#include <complex> 
#include <vector>
#include <list>
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
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "mathSupport.h"
#include "support.h"
#include "MapSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "MarkovProcess.h"


namespace WeightSystem
{


MarkovProcess::MarkovProcess()
 /*! 
    Constructor 
  */
{}

MarkovProcess::MarkovProcess(const MarkovProcess& A)  
  /*! 
    Copy Constructor 
    \param A :: MarkovProcess to copy
  */
{}

MarkovProcess&
MarkovProcess::operator=(const MarkovProcess& A)
  /*! 
    Assignment operator
    \param A :: MarkovProcess to copy
    \return *this
  */
{
  if (this!=&A)
    {
    }
  return *this;
}
  
  
  
} // namespace WeightSystem
