/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/LeadPipe.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include <array>

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
#include "Quadratic.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SurfMap.h"

#include "SplitFlangePipe.h"
#include "LeadPipe.h"

namespace constructSystem
{

LeadPipe::LeadPipe(const std::string& Key) :
  SplitFlangePipe(Key,1)
  /*!
    Build a lead pipe
    \param Key :: Keyname
  */
{}

LeadPipe::LeadPipe(const LeadPipe& A) : 
  SplitFlangePipe(A)
  /*!
    Copy constructor
    \param A :: LeadPipe to copy
  */
{}

LeadPipe&
LeadPipe::operator=(const LeadPipe& A)
  /*!
    Assignment operator
    \param A :: LeadPipe to copy
    \return *this
  */
{
  if (this!=&A)
    {
      SplitFlangePipe::operator=(A);
    }
  return *this;
}

LeadPipe::~LeadPipe() 
  /*!
    Destructor
  */
{}
  
}  // NAMESPACE constructSystem
