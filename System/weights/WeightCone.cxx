/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weights/WeightCone.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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

#include "FileReport.h"
#include "OutputLog.h"


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
      const MonteCarlo::Object* CellPtr=System.findObject(cellN);
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

