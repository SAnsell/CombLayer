/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGenerator/BremMonoCollGenerator.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"
#include "BremMonoCollGenerator.h"

namespace setVariable
{

BremMonoCollGenerator::BremMonoCollGenerator() :
  radius(4.5),gap(4.55),wallThick(0.3),
  flangeRadius(setVariable::CF150::innerRadius),
  flangeLength(setVariable::CF150::flangeLength),
  holeXStep(2.45),holeZStep(0.0),
  holeWidth(2.5),holeHeight(0.7),
  inRadius(setVariable::CF63::innerRadius),
  voidMat("Void"),innerMat("Tungsten"),
  wallMat("Stainless304")
  /*!
    Constructor and defaults
  */
{}

BremMonoCollGenerator::~BremMonoCollGenerator() 
 /*!
   Destructor
 */
{}

  
void
BremMonoCollGenerator::setMaterial(const std::string& IMat,
				   const std::string& WMat)
  /*!
    Set the materials
    \param IMat :: Inner Material
    \param WMat :: wall Material
  */
{
  innerMat=IMat;
  wallMat=WMat;
  return;
}

void
BremMonoCollGenerator::setAperature(const double W,const double H)
  /*!
    Set the widths
    \param W ::  width
    \param H :: height
  */
{
  holeWidth=W;
  holeHeight=H;
  return;
}

				  
void
BremMonoCollGenerator::generateColl(FuncDataBase& Control,
				const std::string& keyName,
				const double yStep,
				const double length) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: Forward step
    \param length :: length of W block
  */
{
  ELog::RegMethod RegA("BremMonoCollGenerator","generatorColl");

  Control.addVariable(keyName+"YStep",yStep);
  
  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"Length",length);
    
  Control.addVariable(keyName+"Gap",gap);
  Control.addVariable(keyName+"WallThick",wallThick);

  Control.addVariable(keyName+"FlangeRadius",flangeRadius);
  Control.addVariable(keyName+"FlangeLength",flangeLength);

  Control.addVariable(keyName+"HoleXStep",holeXStep);
  Control.addVariable(keyName+"HoleZStep",holeZStep);
  Control.addVariable(keyName+"HoleWidth",holeWidth);
  Control.addVariable(keyName+"HoleHeight",holeHeight);

  Control.addVariable(keyName+"InRadius",inRadius);
  
  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"InnerMat",innerMat);
  Control.addVariable(keyName+"WallMat",wallMat);

       
  return;

}

  
}  // NAMESPACE setVariable
