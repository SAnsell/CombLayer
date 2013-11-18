/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/MainProcess.cxx
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
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <boost/regex.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "InputControl.h"
#include "Element.h"
#include "Material.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Tensor.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "cellFluxTally.h"
#include "pointTally.h"
#include "heatTally.h"
#include "surfaceTally.h"
#include "tallyFactory.h"
#include "TallyCreate.h"
#include "Transform.h"
#include "Quaternion.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "MainProcess.h"

namespace mainSystem
{

void
setRunTimeVariable(FuncDataBase& Control,
		   const std::map<std::string,std::string>& VMap)
  /*!
    Set runtime variables 
    \param Control :: Function Data base to update
    \param VMap :: Map of variable + values
   */
{
  std::map<std::string,std::string>::const_iterator mc;
  for(mc=VMap.begin();mc!=VMap.end();mc++)
    {
      if (!Control.hasVariable(mc->first))
        {
	  ELog::EM<<"Failure to find variable name "<<mc->first;
	  ELog::EM.error();
	  exit(1);
	}
      Control.setVariable(mc->first,mc->second);
    }
  return;
}


}  // NAMESPACE mainSystem
