/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   lensModel/outerConstruct.cxx
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
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NList.h"
#include "NRange.h"
#include "Rules.h"
#include "Code.h"
#include "FItem.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "KGroup.h"
#include "Source.h"
#include "Simulation.h"
#include "SimProcess.h"
#include "outerConstruct.h"

namespace lensSystem
{
  
void
outerConstruct(Simulation& System)
  /*!
    Construct the outer skin for the void
    \param System :: Simulation system
  */
{
  // Add outer void
  System.addCell(MonteCarlo::Qhull(100,0,0.0,"-1"));

  SimProcess::registerOuter(System,100,20000);  // Arm 
  SimProcess::registerOuter(System,100,60000);  // outer clearance [top]
  SimProcess::registerOuter(System,100,70000);  // outer clearance [top]
  SimProcess::registerOuter(System,100,80000);  // outer clearance [low]
  SimProcess::registerOuter(System,100,90000);  // outer clearance [low]

  /*  SimProcess::registerOuter(System,100,30001);  // [Water]
  SimProcess::registerOuter(System,100,30002);  // [Al]
  SimProcess::registerOuter(System,100,30003);  // [DC]
  SimProcess::registerOuter(System,100,30004);  // [LEAD]
  SimProcess::registerOuter(System,100,30005);  // [BPOLY]
  SimProcess::registerOuter(System,100,30006);  // [EPOXY]
  SimProcess::registerOuter(System,100,30007);  // [POLY]
  */
  
  return;
}

}  // NAMESPACE LensSystem
