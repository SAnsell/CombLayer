/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/LinacQuadGenerator.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "LinacQuadGenerator.h"

namespace setVariable
{

LinacQuadGenerator::LinacQuadGenerator() :
  length(25.0),yokeRadius(7.5),yokeOuter(18.8),
  poleYAngle(45.0),poleGap(1.25),
  poleRadius(1.55),poleWidth(2.6),     // pole radius 7mm above flat line
  coilRadius(2.31),coilWidth(7.0),   // full gpa
  coilInner(3.86),coilBase(3.86),
  coilBaseDepth(8.0),coilAngle(45.0),
  coilEndExtra(3.2),coilEndRadius(6.0),
  poleMat("Iron"),
  coilMat("Copper"),yokeMat("Aluminium")
  /*!
    Constructor and defaults
  */
{}

LinacQuadGenerator::~LinacQuadGenerator()
 /*!
   Destructor
 */
{}

void
LinacQuadGenerator::setRadius(const double R,const double C)
  /*!
    Set the coil and pole radius
    \param R :: Pole radius [closest point]
    \param C :: Coil radius
   */
{
  poleGap=R;
  coilRadius=C;
  return;
}


void
LinacQuadGenerator::generateQuad(FuncDataBase& Control,
				 const std::string& keyName,
				 const double yStep)  const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
  */
{
  ELog::RegMethod RegA("LinacQuadGenerator","generateQuad");

  Control.addVariable(keyName+"YStep",yStep);

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"YokeRadius",yokeRadius);
  Control.addVariable(keyName+"YokeOuter",yokeOuter);
  Control.addVariable(keyName+"PoleYAngle",poleYAngle);
  Control.addVariable(keyName+"PoleGap",poleGap);
  Control.addVariable(keyName+"PoleRadius",poleRadius);
  Control.addVariable(keyName+"PoleWidth",poleWidth);

  Control.addVariable(keyName+"CoilRadius",coilRadius);
  Control.addVariable(keyName+"CoilWidth",coilWidth);
  Control.addVariable(keyName+"CoilInner",coilInner);
  Control.addVariable(keyName+"CoilBase",coilBase);
  Control.addVariable(keyName+"CoilBaseDepth",coilBaseDepth);
  Control.addVariable(keyName+"CoilAngle",coilAngle);
  Control.addVariable(keyName+"CoilEndExtra",coilEndExtra);
  Control.addVariable(keyName+"CoilEndRadius",coilEndRadius);

  Control.addVariable(keyName+"PoleMat",poleMat);
  Control.addVariable(keyName+"CoilMat",coilMat);
  Control.addVariable(keyName+"YokeMat",yokeMat);

  return;

}


}  // NAMESPACE setVariable
