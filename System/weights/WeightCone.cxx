/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weights/WeightCone.cxx
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
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cone.h"
#include "support.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "Object.h"
#include "Qhull.h"
#include "weightManager.h"
#include "WForm.h"
#include "WItem.h"
#include "WCells.h"
#include "CellWeight.h"
#include "Simulation.h"
#include "objectRegister.h"
#include "inputParam.h"
#include "PositionSupport.h"
#include "TallyCreate.h"
#include "TempWeights.h"
#include "ImportControl.h"

#include "LineTrack.h"
#include "ObjectTrackAct.h"
#include "ObjectTrackPoint.h"
#include "ObjectTrackPlane.h"
#include "Mesh3D.h"
#include "WeightControl.h"

namespace WeightSystem
{
  
/*   
void
WeightControl::calcCellTrack(const Simulation& System,
                             const Geometry::Cone& curCone,
                             CellWeight& CTrack)
  / *!
    Calculate a given cone : calculate those cells
    that the cone intersects
    \param System :: Simulation to use
    \param curPlane :: current plane
    \param cellVec :: Cells to track
    \param CTrack :: Cell Weights for output 
   * /
{
  ELog::RegMethod RegA("WeightCone","calcCellTrack<Cone>");

  CTrack.clear();
  std::vector<Geometry::Vec3D> Pts;
  std::vector<long int> index;

  /*
  for(const int cellN : cellVec)
    {
      const MonteCarlo::Qhull* CellPtr=System.findQhull(cellN);
      if (CellPtr && CellPtr->getMat())
	{
	  index.push_back(CellPtr->getName());  // this should be cellN ??
	  Pts.push_back(CellPtr->getCofM());
	}
    }
  cTrack(System,curPlane,Pts,index,CTrack);
  * /
  return;
}
  */

}  // NAMESPACE weightSystem

