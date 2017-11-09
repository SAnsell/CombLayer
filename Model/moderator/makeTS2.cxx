/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderator/makeTS2.cxx
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
#include <algorithm>
#include <iterator>
#include <memory>
#include <array>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LayerComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"

#include "shutterBlock.h"
#include "GeneralShutter.h"
#include "BulkInsert.h"
#include "FBBlock.h"
#include "makeChipIR.h"
#include "makeZoom.h"

#include "makeIMat.h"
#include "makeTS2Bulk.h"

#include "makeTS2.h"

namespace moderatorSystem
{

makeTS2::makeTS2()
/*!
  Constructor
 */
{}

makeTS2::~makeTS2()
/*!
  Destructor
 */
{}

void 
makeTS2::build(Simulation* SimPtr,
	       const mainSystem::inputParam& IParam)
  
  /*!
    Carry out the full build
    \param SimPtr :: Simulation system
    \param IParam :: Input parameters
   */
{
  // For output stream
  ELog::RegMethod RControl("makeTS2","build");

  moderatorSystem::makeTS2Bulk bulkObj;
  hutchSystem::makeChipIR chipObj;
  zoomSystem::makeZoom zoomObj;
  imatSystem::makeIMat imatObj;
  
  if (IParam.flag("isolate") && IParam.compValue("I",std::string("chipIR")))
    {
      chipObj.buildIsolated(*SimPtr,IParam);
      return;
    }

  if (IParam.flag("isolate") && IParam.compValue("I",std::string("zoom")))
    {
      zoomObj.buildIsolated(*SimPtr);
      return;
    }

  bulkObj.build(SimPtr,IParam);

  if (!IParam.flag("exclude") ||
      (!IParam.compValue("E",std::string("Bulk"))) ) 
    {
      if (!IParam.compValue("E",std::string("chipIR")))  
	chipObj.build(SimPtr,IParam,*bulkObj.getBulkShield());
      if (!IParam.compValue("E",std::string("zoom")))  
	zoomObj.build(*SimPtr,IParam,*bulkObj.getBulkShield());
      if (!IParam.compValue("E",std::string("imat")))
	imatObj.build(SimPtr,IParam,*bulkObj.getBulkShield());
    }
  // Insert pipes [make part of makeTS2Bulk]
  bulkObj.insertPipeObjects(SimPtr,IParam);

  return;
}

}   // NAMESPACE moderatorSystem

