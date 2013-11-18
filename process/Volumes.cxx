/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/Volumes.cxx
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
#include <cmath>
#include <complex> 
#include <vector>
#include <map> 
#include <list> 
#include <set>
#include <string>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "KGroup.h"
#include "SurInter.h"
#include "Simulation.h"
#include "inputParam.h"
#include "volUnit.h"
#include "VolSum.h"
#include "Volumes.h"

namespace ModelSupport
{

void
calcVolumes(Simulation* SimPtr,const mainSystem::inputParam& IParam)
  /*!
    Calculate the volumes for all f4 tallies
    \param SimPtr :: Simulation to use
    \param IParam :: Simulation to use
  */
{
  ELog::RegMethod RegA("createDivide","calcVols");

  if (SimPtr && IParam.flag("volume"))
    {
      SimPtr->createObjSurfMap();
      Geometry::Vec3D Org;
      for(size_t i=0;i<3;i++)
	Org[i]=IParam.getValue<double>("volume",i);
      const double R=IParam.getValue<double>("volume",3);
      const int NP=IParam.getValue<int>("volNum");
      VolSum VTally(Org,R);
      VTally.populateTally(*SimPtr);
      VTally.pointRun(*SimPtr,NP);
      VTally.write("volumes");
    }

  return;
}
  

} // NAMESPACE ModelSupport
