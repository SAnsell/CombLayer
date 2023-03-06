/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGenerator/M1DetailGenerator.cxx
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

#include "M1DetailGenerator.h"

namespace setVariable
{

M1DetailGenerator::M1DetailGenerator() :
  mWidth(6.0),mHeight(6.0),        
  mLength(37.0),mSlotXStep(4.5),     
  mSlotWidth(1.0),mSlotDepth(0.90),     
  mPipeXStep(2.17),mPipeYStep(2.0),
  mPipeZStep(1.8),mPipeSideRadius(0.225),
  mPipeBaseLen(2.1),mPipeBaseRadius(0.25),
  mPipeOuterLen(1.5),mPipeOuterRadius(0.30),
  
  mirrorMat("Silicon300K"),waterMat("H2O"),
  pipeMat("Stainless304"),outerMat("Copper"),
  voidMat("Void")
  /*!
    Constructor and defaults
  */
{}


M1DetailGenerator::~M1DetailGenerator() 
 /*!
   Destructor
 */
{}


void
M1DetailGenerator::makeCrystal(FuncDataBase& Control,
				const std::string& cryName,
				const double zStep,
				const double theta) const
  /*!
    Build the variables for the general crystal.
    The crystals are built generated in the centre of
    the viewed face.
    \param Control :: Database
    \param cryName :: extra name for crystal
    \param zStep :: displacement from MLM vessel white beam centre
    \param theta :: Angle to this crystal to the beam
  */
{
  ELog::RegMethod RegA("M1DetailGenerator","makeCrystal");

  Control.addVariable(cryName+"ZStep",zStep);

  Control.addVariable(cryName+"Theta",theta);
  Control.addVariable(cryName+"Phi",0.0);
  
  Control.addVariable(cryName+"Width",mWidth);
  Control.addVariable(cryName+"Height",mHeight);
  Control.addVariable(cryName+"Length",mLength);

  Control.addVariable(cryName+"SlotXStep",mSlotXStep);
  Control.addVariable(cryName+"SlotWidth",mSlotWidth);
  Control.addVariable(cryName+"SlotDepth",mSlotDepth);

  Control.addVariable(cryName+"PipeXStep",mPipeXStep);
  Control.addVariable(cryName+"PipeYStep",mPipeYStep);
  Control.addVariable(cryName+"PipeZStep",mPipeZStep);
  
  Control.addVariable(cryName+"PipeSideRadius",mPipeSideRadius);
  Control.addVariable(cryName+"PipeBaseLen",mPipeBaseLen);
  Control.addVariable(cryName+"PipeBaseRadius",mPipeBaseRadius);
  Control.addVariable(cryName+"PipeOuterLen",mPipeOuterLen);
  Control.addVariable(cryName+"PipeOuterRadius",mPipeOuterRadius);

  Control.addVariable(cryName+"MirrorMat",mirrorMat);
  Control.addVariable(cryName+"WaterMat",waterMat);
  Control.addVariable(cryName+"PipeMat",pipeMat);
  Control.addVariable(cryName+"OuterMat",outerMat);
  Control.addVariable(cryName+"VoidMat",voidMat);

  return;
}


void
M1DetailGenerator::generateMirror(FuncDataBase& Control,
				  const std::string& keyName,
				  const double theta,
				  const double zStep) const
/*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: y-offset 
    
  */
{
  ELog::RegMethod RegA("M1DetailGenerator","generateMirror");

  // guess of separation
  makeCrystal(Control,keyName+"Mirror",theta,zStep);
  
  return;

}
  
}  // NAMESPACE setVariable
