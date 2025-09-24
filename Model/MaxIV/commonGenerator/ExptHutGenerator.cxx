/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/ExptHutGenerator.cxx
 *
 * Copyright (c) 2004-2025 by Stuart Ansell
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

#include "OpticsHutGenerator.h"
#include "ExptHutGenerator.h"

namespace setVariable
{


ExptHutGenerator::ExptHutGenerator() :
  OpticsHutGenerator(),
  ringWidth(200.0),pbFrontThick(-1.0),
  cornerAngle(45.0),cornerYStep(100000.0),
  fHoleXStep(0.0),fHoleZStep(0.0),fHoleRadius(3.0)
  /*!
    Constructor and defaults
  */
{
  // MODIFIED VARIABLES
  innerThick=0.1;
  pbBackThick=0.6;
  pbTiltedThick=pbBackThick;
  pbWallThick=0.4;
  pbRoofThick=0.4;
  outerThick=0.1;
}


void
ExptHutGenerator::setCorner(const double CA,const double CL)
  /*!
    Adds a corner with angle and length from back wall
    \param CA :: Corner angle (deg)
    \param CL :: Corner length from back wall (>lenght is not corner)
   */
{
  cornerAngle=CA;
  cornerYStep=CL;
  return;
}

void
ExptHutGenerator::setFrontHole(const double XS,const double ZS,
			       const double R)
  /*!
    Add a hole to the front wall
    \param XS :: Offset relative to electron beam
    \param ZS :: Offset relative to electron beam
    \param R :: Radius [inner]
   */
{
  fHoleXStep=XS;
  fHoleZStep=ZS;
  fHoleRadius=R;
  return;
}



void
ExptHutGenerator::generateHut(FuncDataBase& Control,
			      const std::string& hutName,
			      const double yStep,
			      const double length) const
/*!
    Primary function for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param yStep :: step distance
    \param length :: Full length of hut
  */
{
  ELog::RegMethod RegA("ExptHutGenerator","generateOpticsHut");

  OpticsHutGenerator::generateHut(Control,hutName,length);

  Control.addVariable(hutName+"YStep",yStep);
  Control.addVariable(hutName+"RingWidth",ringWidth);
  Control.addVariable(hutName+"PbFrontThick",pbFrontThick);
  Control.addVariable(hutName+"PbTiltedThick",pbTiltedThick);

  Control.addVariable(hutName+"CornerAngle",cornerAngle);
  Control.addVariable(hutName+"CornerLength",length-cornerYStep);

  Control.addVariable(hutName+"FHoleXStep",fHoleXStep);
  Control.addVariable(hutName+"FHoleZStep",fHoleZStep);
  Control.addVariable(hutName+"FHoleRadius",fHoleRadius);


  return;

}

}  // NAMESPACE setVariable
