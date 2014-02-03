/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuild/essMod.cxx
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
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

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
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "LayerComp.h"
#include "essMod.h"

namespace essSystem
{

essMod::essMod(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::LayerComp(0),
  attachSystem::FixedComp(Key,6)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

essMod::essMod(const essMod& A) : 
  attachSystem::ContainedComp(A),attachSystem::LayerComp(A),
  attachSystem::FixedComp(A)
  /*!
    Copy constructor
    \param A :: essMod to copy
  */
{}

essMod&
essMod::operator=(const essMod& A)
  /*!
    Assignment operator
    \param A :: essMod to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::LayerComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
    }
  return *this;
}

essMod::~essMod() 
  /*!
    Destructor
  */
{}

  
}  // NAMESPACE moderatorSystem
