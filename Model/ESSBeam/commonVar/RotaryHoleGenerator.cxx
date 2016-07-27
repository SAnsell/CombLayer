/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/RotaryHoleGenerator.cxx
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
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "ContainedComp.h"
#include "FixedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "HoleShape.h"
#include "RotaryHoleGenerator.h"

namespace setVariable
{

RotaryHoleGenerator::RotaryHoleGenerator() :
  mainRadius(30.0),mainThick(10.0),
  innerWall(1.0),innerMat("Void"),
  defMat("Inconnel"),posIndex(0)
  /*!
    Constructor and defaults
  */
{}

RotaryHoleGenerator::RotaryHoleGenerator(const RotaryHoleGenerator& A) : 
  mainRadius(A.mainRadius),mainThick(A.mainThick),
  innerWall(A.innerWall),innerMat(A.innerMat),defMat(A.defMat),
  posIndex(A.posIndex),HData(A.HData)
  /*!
    Copy constructor
    \param A :: RotaryHoleGenerator to copy
  */
{}

RotaryHoleGenerator&
RotaryHoleGenerator::operator=(const RotaryHoleGenerator& A)
  /*!
    Assignment operator
    \param A :: RotaryHoleGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      mainRadius=A.mainRadius;
      mainThick=A.mainThick;
      innerWall=A.innerWall;
      innerMat=A.innerMat;
      defMat=A.defMat;
      posIndex=A.posIndex;
      HData=A.HData;
    }
  return *this;
}

RotaryHoleGenerator::~RotaryHoleGenerator() 
 /*!
   Destructor
 */
{}

void
RotaryHoleGenerator::addHole(const std::string& type,
			     const double Rad,const double xRad,
			     const double aCent,const double rStep)
/*!
  Add a simple hole
  \param type :: type of hole []
  \param Rad :: radius
  \param xRad :: xRadius [if used]
  \param aCent :: Angular centre ??
  \param rStep :: distance from centre
*/
{
  ELog::RegMethod RegA("RotaryHoleGenerator","addHole");
  holeInfo A;
  A.shape=constructSystem::HoleShape::shapeIndex(type);
  A.radius=Rad;
  A.xradius=xRad;
  A.angCent=aCent;
  A.radStep=rStep;
  HData.push_back(A);
  return;
}

void
RotaryHoleGenerator::setPosition(const size_t PI)
  /*!
    Set the active hole position
    \param PI :: Position index
  */
{
  ELog::RegMethod RegA("RotaryHoleGenerator","setPosition");

  if (PI>=HData.size())
    throw ColErr::IndexError<size_t>(PI,HData.size(),"PositionIndex/HData");
  posIndex=PI;
  return;
}
  
  
void
RotaryHoleGenerator::generatePinHole(FuncDataBase& Control,
				     const std::string& keyName,
				     const double yStep,
				     const double rotStep,
				     const double primAngle) const


  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: Y step of collimator
    \param rotStep :: Z step to rotation centre [ZDepth]
    \param primAngle :: primary angle
  */
{
  ELog::RegMethod RegA("RotaryHoleGenerator","generatorPinHole");

  Control.addVariable(keyName+"YStep",yStep);
  Control.addVariable(keyName+"RotDepth",rotStep);
  Control.addVariable(keyName+"Radius",mainRadius);
  Control.addVariable(keyName+"Thick",mainThick);
  
  Control.addVariable(keyName+"InnerWall",innerWall);
  Control.addVariable(keyName+"InnerWallMat",innerMat);

  Control.addVariable(keyName+"NLayers",0); // ??

  Control.addVariable(keyName+"DefMat",defMat);

  // collimator holes:
  Control.addVariable(keyName+"NHole",HData.size());

  Control.addVariable(keyName+"HoleIndex",posIndex);
  Control.addVariable(keyName+"HoleAngOff",primAngle);
  for(size_t index=0;index<HData.size();index++)
    {
      const std::string holeName=
	keyName+"Hole"+StrFunc::makeString(index);
      const holeInfo& HI(HData[index]);
      Control.addVariable(holeName+"Shape",HI.shape);
      Control.addVariable(holeName+"Radius",HI.radius);
      Control.addVariable(holeName+"XRadius",HI.xradius);

      Control.addVariable(holeName+"AngleCentre",HI.angCent);
      Control.addVariable(holeName+"RadialStep",HI.radStep);
    }      

  return;

}

}  // NAMESPACE setVariable
