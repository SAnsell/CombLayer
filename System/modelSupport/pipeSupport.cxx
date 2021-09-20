/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   modelSupport/pipeSupport.cxx
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
#include <cmath>
#include <complex> 
#include <vector>
#include <map> 
#include <list> 
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <memory>
 
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "LineTrack.h"

///\file System/modelSupport/pipeSupport.cxx

namespace ModelSupport
{
  
void
calcLineTrack(Simulation& System,
	      const Geometry::Vec3D& XP,
	      const Geometry::Vec3D& YP,
	      std::map<int,MonteCarlo::Object*>& OMap)
  /*!
    From a line determine thos points that the system
    intersect
    \param System :: Simuation to use
    \param XP :: A-Point of line
    \param YP :: B-Point of line
    \param OMap :: Object Map
  */
{
  ELog::RegMethod RegA("pipeSupport[F]","calcLineTrack");
  
  typedef std::map<int,MonteCarlo::Object*> MTYPE;
  
  LineTrack LT(XP,YP);
  LT.calculate(System);
  const std::vector<MonteCarlo::Object*>& OVec=LT.getObjVec();
  std::vector<MonteCarlo::Object*>::const_iterator oc;
  for(oc=OVec.begin();oc!=OVec.end();oc++)
    {
      const int ONum=(*oc)->getName();
      if (OMap.find(ONum)==OMap.end())
	OMap.insert(MTYPE::value_type(ONum,(*oc)));
    }
  return;
}


} // NAMESPACE ModelSupport

