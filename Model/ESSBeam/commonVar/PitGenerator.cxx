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
PitGenerator::generatePit(FuncDataBase& Control,const std::string& keyName,
			  const double vLength,const double vWidth,
			  const double vHeight,const double vDepth,
			  const size_t NSeg,const size_t NLayer)  const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param vLength :: length of void
    \param side :: full extent at sides
    \param height :: Full height
    \param depth :: Full depth
    \param NSeg :: number of segments
    \param NLayer :: number of layers
  */
{
  ELog::RegMethod RegA("PitGenerator","generatorPit");


  Control.addVariable(keyName+"VoidHeight",vHeight);
  Control.addVariable(keyName+"VoidDepth",vDepth);
  Control.addVariable(keyName+"VoidWidth",vWidth);
  Control.addVariable(keyName+"VoidLength",vLength);
  
  Control.addVariable(keyName+"FeHeight",70.0);
  Control.addVariable(keyName+"FeDepth",60.0);
  Control.addVariable(keyName+"FeWidth",60.0);
  Control.addVariable(keyName+"FeFront",45.0);
  Control.addVariable(keyName+"FeBack",70.0);
  Control.addVariable(keyName+"FeMat",feMat);
  
  Control.addVariable(keyName+"ConcHeight",50.0);
  Control.addVariable(keyName+"ConcDepth",50.0);
  Control.addVariable(keyName+"ConcWidth",50.0);
  Control.addVariable(keyName+"ConcFront",50.0);
  Control.addVariable(keyName+"ConcBack",50.0);
  Control.addVariable(keyName+"ConcMat",concMat);

  Control.addVariable(keyName+"ColletHeight",15.0);
  Control.addVariable(keyName+"ColletDepth",15.0);
  Control.addVariable(keyName+"ColletWidth",40.0);
  Control.addVariable(keyName+"ColletLength",5.0);
  Control.addVariable(keyName+"ColletMat",colletMat);

  
  return;

}

}  // NAMESPACE setVariable
