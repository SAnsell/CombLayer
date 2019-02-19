/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/PipeShieldGenerator.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "PipeShieldGenerator.h"

namespace setVariable
{

PipeShieldGenerator::PipeShieldGenerator() :
  height(60.0),width(60.0),length(7.0),clearGap(0.2),
  wallThick(0.5),wingThick(7.0),
  mat("Lead"),wallMat("Stainless304"),
  wingMat("Stainless304")
  /*!
    Constructor and defaults
  */
{}

PipeShieldGenerator::PipeShieldGenerator(const PipeShieldGenerator& A) : 
  height(A.height),width(A.width),length(A.length),
  clearGap(A.clearGap),wallThick(A.wallThick),wingThick(A.wingThick),
  mat(A.mat),wallMat(A.wallMat),wingMat(A.wingMat)
  /*!
    Copy constructor
    \param A :: PipeShieldGenerator to copy
  */
{}

PipeShieldGenerator&
PipeShieldGenerator::operator=(const PipeShieldGenerator& A)
  /*!
    Assignment operator
    \param A :: PipeShieldGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      height=A.height;
      width=A.width;
      length=A.length;
      clearGap=A.clearGap;
      wallThick=A.wallThick;
      wingThick=A.wingThick;
      mat=A.mat;
      wallMat=A.wallMat;
      wingMat=A.wingMat;
    }
  return *this;
}

PipeShieldGenerator::~PipeShieldGenerator() 
 /*!
   Destructor
 */
{}

void
PipeShieldGenerator::setPlate(const double W,const double H,
			      const double L)

  /*!
    Set the shield plate
    \param W :: Width [X]  shield plate
    \param H :: Height [Z] of shield plate
    \param L :: Length of shield [Y]
   */
{
  length=L;
  width=W;
  height=H;
  return;
}

void
PipeShieldGenerator::setMaterial(const std::string& MainM,
			     const std::string& WallM,
			     const std::string& WingM)
  /*!
    Set the materials
    \param MainM :: Main shield material
    \param WallM :: Wall shield material
    \param WingM :: Wing Material
  */
{
  mat=MainM;
  wallMat=WallM;
  wingMat=WingM;
  return;
}

				  
void
PipeShieldGenerator::generateShield(FuncDataBase& Control,
				const std::string& keyName,
				const double yStep,
				const double wingLength) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
    \param wingLength :: Length of wings
  */
{
  ELog::RegMethod RegA("PipeShieldGenerator","generateShield");
  
  Control.addVariable(keyName+"YStep",yStep);

  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Width",width);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"WallThick",wallThick);
  Control.addVariable(keyName+"ClearGap",clearGap);

  Control.addVariable(keyName+"WingThick",wingThick);
  Control.addVariable(keyName+"WingLength",wingLength);


  Control.addVariable(keyName+"WallMat",wallMat);
  Control.addVariable(keyName+"Mat",mat);
  Control.addVariable(keyName+"WingMat",wingMat);
  
  return;

}

  
}  // NAMESPACE setVariable
