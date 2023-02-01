/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGenerator/MLMDetailGenerator.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"

#include "MLMDetailGenerator.h"

namespace setVariable
{

MLMDetailGenerator::MLMDetailGenerator() :
  gap(0.4),
  width(4.0),height(4.0),length(18.0),
  topSlotXStep(1.2),topSlotWidth(1.0),topSlotLength(16.0),
  topSlotDepth(0.2),
  mirrorMat("Silicon300K"),baseMat("Copper")
  /*!
    Constructor and defaults
  */
{}


MLMDetailGenerator::~MLMDetailGenerator() 
 /*!
   Destructor
 */
{}


void
MLMDetailGenerator::makeCrystal(FuncDataBase& Control,
				const std::string& cryName,
				const bool rotFlag,
				const double xStep,
				const double yStep,
				const double theta) const
/*!
    Build the variables for the general crystal.
    The crystals are built generated in the centre of
    the viewed face.
    \param Control :: Database
    \param cryName :: extra name for crystal
    \param rotFlag :: Crystal rotated 180 around Z
    \param xStep :: displacement from MLM vessel white beam centre
    \param yStep :: displacement from MLM vessel white beam centre
    \param theta :: Angle to this crystal to the beam
  */
{
  ELog::RegMethod RegA("MLMDetailGenerator","makeCrystal");

  const double rotAngle(rotFlag ? 180.0 : 0.0);
  Control.addVariable(cryName+"xStep",xStep);
  Control.addVariable(cryName+"YStep",yStep);
  Control.addVariable(cryName+"ZAngle",rotAngle+theta);

  Control.addVariable(cryName+"Width",width);
  Control.addVariable(cryName+"Height",height);
  Control.addVariable(cryName+"Length",length);

  Control.addVariable(cryName+"TopSlotXStep",topSlotXStep);
  Control.addVariable(cryName+"TopSlotWidth",topSlotWidth);
  Control.addVariable(cryName+"TopSlotDepth",topSlotDepth);
  Control.addVariable(cryName+"TopSlotLength",topSlotLength);

  Control.addVariable(cryName+"MirrorMat",mirrorMat);
  
  return;
}
  
void
MLMDetailGenerator::generateMono(FuncDataBase& Control,
				 const std::string& keyName,
				 const double thetaA,
				 const double thetaB) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: y-offset 
    
  */
{
  ELog::RegMethod RegA("MLMDetailGenerator","generateBox");

  // guess of separation
  const double xstalYStep(gap/tan(2.0*M_PI*thetaA/180.0));

  makeCrystal(Control,keyName+"XstalA",1,0.0,-xstalYStep/2.0,thetaA);
  makeCrystal(Control,keyName+"XstalB",0,gap,xstalYStep/2.0,thetaB); 
  
  
  return;

}
  
}  // NAMESPACE setVariable
