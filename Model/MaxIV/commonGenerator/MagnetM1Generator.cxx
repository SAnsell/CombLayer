/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGenerator/MagnetM1Generator.cxx
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

#include "EPCombineGenerator.h"
#include "DipoleGenerator.h"
#include "QuadrupoleGenerator.h"
#include "OctupoleGenerator.h"

#include "MagnetM1Generator.h"

namespace setVariable
{

MagnetM1Generator::MagnetM1Generator() :
  yOffset(234.6),blockYStep(10.5),length(220.0),
  outerVoid(12.0),ringVoid(12.0),topVoid(12.0),
  baseVoid(12.0),baseThick(8.0),wallThick(6.0),
  
  // entry pipe
  entryLength(109.5),
  entryFlangeRadius(CF50::flangeRadius),
  entryFlangeLength(CF50::flangeLength),
  
  // half pipe
  photonRadius(1.10),
  pipeWallThick(0.1),electronRadius(1910.0),
  electronAngle(1.5),

  // full pipe
  fullLength(205.0),

  // halfpipe
  voidMat("Void"),wallMat("Stainless304")
  /*!
    Constructor and defaults
  */
{}
  
MagnetM1Generator::~MagnetM1Generator() 
 /*!
   Destructor
 */
{}

void
MagnetM1Generator::generateEntryPipe(FuncDataBase& Control,
				     const std::string& pipeName) const
  /*!
    Entry Pipe
    \param Control :: dataBase
    \param pipeName :: Prename of entry pipe
   */
{
  ELog::RegMethod RegA("MagnetM1Generator","generateEntryPipe");

  Control.addVariable(pipeName+"YStep",yOffset-blockYStep);
  Control.addVariable(pipeName+"Radius",photonRadius);
  Control.addVariable(pipeName+"Length",entryLength);
  Control.addVariable(pipeName+"WallThick",pipeWallThick);
  Control.addVariable(pipeName+"FlangeRadius",entryFlangeRadius);
  Control.addVariable(pipeName+"FlangeLength",entryFlangeLength);
  
  Control.addVariable(pipeName+"VoidMat",voidMat);
  Control.addVariable(pipeName+"WallMat",wallMat);
  return;
}

void
MagnetM1Generator::generateHalfPipe(FuncDataBase& Control,
				    const std::string& pipeName) const
  /*!
    Half Pipe
    \param Control :: dataBase
    \param pipeName :: Prename of half pipe
   */
{
  ELog::RegMethod RegA("MagnetM1Generator","generateHalfPipe");

  Control.addVariable(pipeName+"PhotonRadius",photonRadius);
  Control.addVariable(pipeName+"WallThick",pipeWallThick);
  Control.addVariable(pipeName+"ElectronRadius",electronRadius);
  Control.addVariable(pipeName+"ElectronAngle",electronAngle);
  Control.addVariable(pipeName+"FullLength",fullLength-entryLength);
  
  Control.addVariable(pipeName+"VoidMat",voidMat);
  Control.addVariable(pipeName+"WallMat",wallMat);
  return;
}


  
  
void
MagnetM1Generator::generateBlock(FuncDataBase& Control,
				 const std::string& keyName) const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("MagnetM1Generator","generateBlock");

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

  generateEntryPipe(Control,keyName+"EntryPipe");
  generateHalfPipe(Control,keyName+"HalfElectron");
  // setVariable::PreDipoleGenerator PBGen;
  // PBGen.generatePipe(Control,keyName+"PreDipole");

  setVariable::EPCombineGenerator EPCGen;
  EPCGen.generatePipe(Control,keyName+"EPCombine");

  setVariable::DipoleGenerator DGen;
  setVariable::OctupoleGenerator OGen;
  setVariable::QuadrupoleGenerator QGen;

  OGen.generateOcto(Control,keyName+"OXX",23.9,10.0); // 18.9+5

  // length is lengh + coil extra [2cm]
  QGen.generateQuad(Control,keyName+"QFend",46.0,23.0);  //33.5+12.5

  OGen.generateOcto(Control,keyName+"OXY",65.9,10.0);
  // extra +5 cm
  QGen.generateQuad(Control,keyName+"QDend",90.30,23.0);
  // +5 cm
  DGen.generateDipole(Control,keyName+"DIPm",118.50,60.0);

  OGen.setPoleAngle(0.0);
  OGen.setRadius(2.8,3.4);
  OGen.generateOcto(Control,keyName+"OYY",190.0,10.0);
  
  return;
}

  
}  // NAMESPACE setVariable
