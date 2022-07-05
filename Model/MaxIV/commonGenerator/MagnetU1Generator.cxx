/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGenerator/MagnetU1Generator.cxx
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

#include "CorrectorMagGenerator.h"
#include "DipoleGenerator.h"
#include "QuadrupoleGenerator.h"
#include "SexupoleGenerator.h"
#include "OctupoleGenerator.h"

#include "MagnetU1Generator.h"

namespace setVariable
{

MagnetU1Generator::MagnetU1Generator() :
  yOffset(130.2),blockYStep(10.5),length(220.0),
  outerVoid(12.0),ringVoid(12.0),topVoid(12.0),
  baseVoid(12.0),baseThick(8.0),wallThick(6.0),
  voidMat("Void"),wallMat("Stainless304")
  /*!
    Constructor and defaults
  */
{}
  
MagnetU1Generator::~MagnetU1Generator() 
 /*!
   Destructor
 */
{}

void
MagnetU1Generator::generateComponents(FuncDataBase& Control,
				      const std::string& keyName) const
{
  setVariable::QuadrupoleGenerator QGen;
  setVariable::SexupoleGenerator SGen;
  setVariable::CorrectorMagGenerator CMGen;
  setVariable::DipoleGenerator DGen;
  

  QGen.generateQuad(Control,keyName+"QFm1",30.0,25.0);
  SGen.generateHex(Control,keyName+"SFm",50.0,7.5);
  QGen.generateQuad(Control,keyName+"QFm2",68.0,25.0);

  CMGen.setMagLength(10.0);
  CMGen.generateMag(Control,keyName+"cMagVA",88.5,1);
  CMGen.generateMag(Control,keyName+"cMagHA",100.5,1);

  SGen.generateHex(Control,keyName+"SD1",110.5,7.5);
  DGen.generateDipole(Control,keyName+"DIPm",122.50,80.0);
  SGen.generateHex(Control,keyName+"SD2",224.0,7.5);
  return;
}

  
void
MagnetU1Generator::generateBlock(FuncDataBase& Control,
				 const std::string& keyName) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("MagnetU1Generator","generateBlock");

  Control.addVariable(keyName+"YStep",yOffset-blockYStep);    
  Control.addVariable(keyName+"BlockYStep",blockYStep);
  Control.addVariable(keyName+"Length",length);

  Control.addVariable(keyName+"OuterVoid",outerVoid);
  Control.addVariable(keyName+"RingVoid",ringVoid);
  Control.addVariable(keyName+"TopVoid",topVoid);
  Control.addVariable(keyName+"BaseVoid",baseVoid);

  Control.addVariable(keyName+"BaseThick",baseThick);
  Control.addVariable(keyName+"WallThick",wallThick);
  
  Control.addVariable(keyName+"VoidMat",voidMat);
  Control.addVariable(keyName+"WallMat",wallMat);

  generateComponents(Control,keyName);
  return;
}

 
  
}  // NAMESPACE setVariable
