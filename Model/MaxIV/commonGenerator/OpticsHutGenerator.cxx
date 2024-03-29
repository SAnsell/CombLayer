/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGenerator/OpticsHutGenerator.cxx
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
#include "CFFlanges.h"

#include "OpticsHutGenerator.h"

namespace setVariable
{


OpticsHutGenerator::OpticsHutGenerator() :
  height(277.0),outWidth(259.6),
  innerThick(0.3),pbWallThick(1.6),
  pbBackThick(7.0),pbRoofThick(1.6),
  outerThick(0.3),backPlateThick(5.0),
  backPlateWidth(120.0),backPlateHeight(120.0),
  innerOutVoid(10.0),outerOutVoid(10.0),
  frontVoid(0.0),backVoid(0.0),
  outerBackVoid(0.0),
  skinMat("Stainless304"),pbMat("Lead"),
  voidMat("Void")
  /*!
    Constructor and defaults
  */
{}

void
OpticsHutGenerator::addHole(const Geometry::Vec3D& HO,
			    const double R)
  /*!
    Add an additional hole to the back wall
    \param HO :: Offset relative to electron beam
    \param R :: Radius [inner]
   */
{
  if (R>Geometry::zeroTol)
    {
      holeOffset.push_back(HO);
      holeRadius.push_back(R);
    }
  return;
}
  
void
OpticsHutGenerator::generateHut(FuncDataBase& Control,
				const std::string& keyName,
				const double length) const
  /*!
    Primary function for setting the variables
    \param Control :: Database to add variables
    \param keyName :: head name for variable
    \param length :: Full length of hut
  */
{
  ELog::RegMethod RegA("OpticsHutGenerator","generateOpticsHut");

  Control.addVariable(keyName+"Length",length);
  
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"OutWidth",outWidth);
  Control.addVariable(keyName+"InnerThick",innerThick);
  Control.addVariable(keyName+"PbWallThick",pbWallThick);
  Control.addVariable(keyName+"PbBackThick",pbBackThick);
  Control.addVariable(keyName+"PbRoofThick",pbRoofThick);
  Control.addVariable(keyName+"OuterThick",outerThick);
  Control.addVariable(keyName+"InnerOutVoid",innerOutVoid);
  Control.addVariable(keyName+"OuterOutVoid",outerOutVoid);
  Control.addVariable(keyName+"FrontVoid",frontVoid);
  Control.addVariable(keyName+"BackVoid",backVoid);
  Control.addVariable(keyName+"OuterBackVoid",outerBackVoid);

  Control.addVariable(keyName+"BackPlateThick",backPlateThick);
  Control.addVariable(keyName+"BackPlateWidth",backPlateWidth);
  Control.addVariable(keyName+"BackPlateHeight",backPlateHeight);
    
  for(size_t i=0;i<holeRadius.size();i++)
    {
      const std::string iStr("Hole"+std::to_string(i)); 
      Control.addVariable(keyName+iStr+"XStep",holeOffset[i].X());
      Control.addVariable(keyName+iStr+"ZStep",holeOffset[i].Z());
      Control.addVariable(keyName+iStr+"Radius",holeRadius[i]);
    }

  Control.addVariable(keyName+"SkinMat",skinMat);
  Control.addVariable(keyName+"PbMat",pbMat);
  Control.addVariable(keyName+"VoidMat",voidMat);
    
  return;

}

}  // NAMESPACE setVariable
