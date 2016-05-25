/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/PitGenerator.cxx
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
#include <stack>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "surfExpand.h"
#include "PitGenerator.h"

namespace setVariable
{

PitGenerator::PitGenerator() :
  feHeight(40.0),
  feMat("CastIron"),concMat("Concrete"),
  colletMat("Tungsten")

  /*!
    Constructor and defaults
  */
{}

  
PitGenerator::~PitGenerator() 
 /*!
   Destructor
 */
{}

  
void
PitGenerator::generatePit
( FuncDataBase& Control,const std::string& keyName,
  const double length,
  const double side,const double height,
  const double depth,const size_t NSeg,const size_t NLayer)  const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param length :: overall length
    \param side :: full extent at sides
    \param height :: Full height
    \param depth :: Full depth
    \param NSeg :: number of segments
    \param NLayer :: number of layers
  */
{
  ELog::RegMethod RegA("PitGenerator","generatorPit");


  Control.addVariable("vespaPitBVoidHeight",167.0);
  Control.addVariable("vespaPitBVoidDepth",36.0);
  Control.addVariable("vespaPitBVoidWidth",246.0);
  Control.addVariable("vespaPitBVoidLength",105.0);
  
  Control.addVariable("vespaPitBFeHeight",70.0);
  Control.addVariable("vespaPitBFeDepth",60.0);
  Control.addVariable("vespaPitBFeWidth",60.0);
  Control.addVariable("vespaPitBFeFront",45.0);
  Control.addVariable("vespaPitBFeBack",70.0);
  Control.addVariable("vespaPitBFeMat",feMat);
  
  Control.addVariable("vespaPitBConcHeight",50.0);
  Control.addVariable("vespaPitBConcDepth",50.0);
  Control.addVariable("vespaPitBConcWidth",50.0);
  Control.addVariable("vespaPitBConcFront",50.0);
  Control.addVariable("vespaPitBConcBack",50.0);
  Control.addVariable("vespaPitBConcMat",concMat);

  Control.addVariable("vespaPitBColletHeight",15.0);
  Control.addVariable("vespaPitBColletDepth",15.0);
  Control.addVariable("vespaPitBColletWidth",40.0);
  Control.addVariable("vespaPitBColletLength",5.0);
  Control.addVariable("vespaPitBColletMat",colletMat);

  
  return;

}

}  // NAMESPACE setVariable
