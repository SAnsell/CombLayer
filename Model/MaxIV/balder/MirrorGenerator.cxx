/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructVar/MirrorGenerator.cxx
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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "CFFlanges.h"
#include "MirrorGenerator.h"

namespace setVariable
{

MirrorGenerator::MirrorGenerator() :
  radius(0.0),length(80.0),thick(1.0),width(5.0),
  baseThick(2.0),baseExtra(1.0),
  mirrMat("Silicon300K"),baseMat("Copper")
  /*!
    Constructor and defaults
  */
{}

MirrorGenerator::~MirrorGenerator() 
 /*!
   Destructor
 */
{}

void
MirrorGenerator::setPlate(const double L,const double T,
			  const double W)

  /*!
    Set the mirror plate
    \param L :: Length of mirror [Y]
    \param T :: Thickness [Z] of mirror plate
    \param W :: Widht [X]  mirror plate
   */
{
  length=L;
  thick=T;
  width=W;
  return;
}


void
MirrorGenerator::setRadius(const double R)

  /*!
    Set the surface radius
    \param R :: surface radius

   */
{
  radius=R;
  return;
}

void
MirrorGenerator::setMaterial(const std::string& MMat,
			     const std::string& BMat)
  /*!
    Set the materials
    \param MMat :: Mirror Material
    \param BMat :: Base Material
  */
{
  mirrMat=MMat;
  baseMat=BMat;
  return;
}

				  
void
MirrorGenerator::generateMirror(FuncDataBase& Control,
				const std::string& keyName,
				const double yStep,
				const double zStep,
				const double theta,
				const double phi) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: Step along beam centre
    \param zStep :: Vertical lift from beamCentre
    \param theta :: theta angle
    \param phi :: phi angle
  */
{
  ELog::RegMethod RegA("MirrorGenerator","generatorMount");
  
  Control.addVariable(keyName+"YStep",yStep);
  Control.addVariable(keyName+"ZStep",zStep);
  Control.addVariable(keyName+"Theta",theta);
  Control.addVariable(keyName+"Phi",phi);

  Control.addVariable(keyName+"Radius",radius);
  Control.addVariable(keyName+"Thick",thick);
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Width",width);

  Control.addVariable(keyName+"BaseThick",baseThick);
  Control.addVariable(keyName+"BaseExtra",baseExtra);

  
  Control.addVariable(keyName+"MirrorMat",mirrMat);
  Control.addVariable(keyName+"BaseMat",baseMat);
       
  return;

}

  
}  // NAMESPACE setVariable
