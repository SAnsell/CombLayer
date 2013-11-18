/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Upgrade/CH4PreModBase.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <numeric>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>


#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "KGroup.h"
#include "Source.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "SimProcess.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "World.h"
#include "CH4PreModBase.h"


namespace ts1System
{
  
CH4PreModBase::CH4PreModBase(const std::string& Key,const size_t NLink)  : 
  ContainedComp(),FixedComp(Key,NLink),
  preIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(preIndex+1),voidCell(0)
  /*!
    Constructor
    \param Key :: Keyname 
    \param NLink :: Number of links
  */
{}

CH4PreModBase::CH4PreModBase(const CH4PreModBase& A) : 
  ContainedComp(A),FixedComp(A),
  preIndex(A.preIndex),cellIndex(A.cellIndex),
  voidCell(A.voidCell)
  /*!
    Copy constructor
    \param A :: CH4PreModBase to copy
  */
{}

CH4PreModBase&
CH4PreModBase::operator=(const CH4PreModBase& A)
  /*!
    Assignment operator
    \param A :: CH4PreModBase to copy
    \return *this
  */
{

  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      voidCell=A.voidCell;
    }
  return *this;
}

CH4PreModBase::~CH4PreModBase()
  /// Destructor
{}
  
}  // NAMESPACE ts1System
