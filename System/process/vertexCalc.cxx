/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/ModelSupport.cxx
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
#include "OutputLog.h"
#include "Surface.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "SurfVertex.h"
#include "Line.h"
#include "SurInter.h"
#include "vertexCalc.h"

namespace ModelSupport
{

std::vector<Geometry::Vec3D>
calcVertex(const MonteCarlo::Object& obj)
  /*!
    Calculate the vertex of an object
   */
{
  ELog::RegMethod RegA("","calcVertex");
  
  const std::vector<const Geometry::Surface*>& surListget=
    obj.SurList();

  VList.clear();                       // clear list of Vertex
  std::vector<const Geometry::Surface*>::const_iterator ic,jc,kc;
  int cnt(0);
  for(ic=SurList.begin();ic!=SurList.end();ic++)
    for(jc=ic+1;jc!=SurList.end();jc++)
      for(kc=jc+1;kc!=SurList.end();kc++)
        {
	  // This adds intersections to the VList
	  cnt+=getIntersect(*ic,*jc,*kc);
	}
  // return number of item found
  return cnt;
}

  
}
  

}  // NAMESPACE ModelSupport
