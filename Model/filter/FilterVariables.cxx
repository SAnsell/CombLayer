/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photon/PhotonVariables.cxx
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
#include <set>
#include <map>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "variableSetup.h"

namespace setVariable
{

void
FilterVariables(FuncDataBase& Control)
  /*!
    Function to set the control variables and constants
    -- This version is for Photon Moderator
    \param Control :: Function data base to add constants too
  */
{
  ELog::RegMethod RegA("FilterVariables[F]","FilterVariables");

  Control.addVariable("beamSourceParticleType",1);     

  Control.addVariable("beamSourceXStep",0.0);       
  Control.addVariable("beamSourceYStep",-5.0);       
  Control.addVariable("beamSourceZStep",0.0);
  Control.addVariable("beamSourceZAngle",0.0);       
  Control.addVariable("beamSourceRadius",3.0);       
  Control.addVariable("beamSourceASpread",0.0);       
  Control.addVariable("beamSourceEStart",3.0);       
  Control.addVariable("beamSourceNE",10);       
  Control.addVariable("beamSourceEEnd",80.0);     
  //  Control.addVariable("beamSourceEnergy","0.7 0.8 1.0 2.0 4.0 8.0 10.0");
  //  Control.addVariable("beamSourceEProb"," 0.0 2e9 1e9 8e8 4e8 2e8 1e8");
  Control.addVariable("beamSourceEFile","Spectrum.out");

  Control.addVariable("SiPrimaryXStep",0.0);
  Control.addVariable("SiPrimaryYStep",0.0);
  Control.addVariable("SiPrimaryZStep",0.0);
  Control.addVariable("SiPrimaryXYangle",0.0);
  Control.addVariable("SiPrimaryZangle",0.0);
  Control.addVariable("SiPrimaryNLayers",1);
  Control.addVariable("SiPrimaryOuterRadius",5.0);
  Control.addVariable("SiPrimaryNUnits",1);
  Control.addVariable("SiPrimary0Thick",10.0);
  Control.addVariable("SiPrimaryMat0","Silicon80K");
  Control.addVariable("SiPrimaryTemp0",20.0);


  Control.addVariable("SiSecondXStep",0.0);
  Control.addVariable("SiSecondYStep",400.0);
  Control.addVariable("SiSecondZStep",0.0);
  Control.addVariable("SiSecondXYangle",0.0);
  Control.addVariable("SiSecondZangle",0.0);
  Control.addVariable("SiSecondNLayers",1);
  Control.addVariable("SiSecondOuterRadius",5.0);
  Control.addVariable("SiSecondNUnits",1);
  Control.addVariable("SiSecond0Thick",10.0);
  Control.addVariable("SiSecondMat0","Silicon80K");
  Control.addVariable("SiSecondTemp0",20.0);
  
  Control.addVariable("LeadXStep",0.0);
  Control.addVariable("LeadYStep",100.0);
  Control.addVariable("LeadZStep",0.0);
  Control.addVariable("LeadXYangle",0.0);
  Control.addVariable("LeadZangle",0.0);
  Control.addVariable("LeadNLayers",1);
  Control.addVariable("LeadOuterRadius",5.0);
  Control.addVariable("LeadNUnits",1);
  Control.addVariable("Lead0Thick",5.0);
  Control.addVariable("LeadMat0","Void");
  Control.addVariable("LeadTemp0",80.0);
    
  Control.addVariable("SiThirdXStep",0.0);
  Control.addVariable("SiThirdYStep",400.0);
  Control.addVariable("SiThirdZStep",0.0);
  Control.addVariable("SiThirdXYangle",0.0);
  Control.addVariable("SiThirdZangle",0.0);
  Control.addVariable("SiThirdNLayers",1);
  Control.addVariable("SiThirdOuterRadius",5.0);
  Control.addVariable("SiThirdNUnits",1);
  Control.addVariable("SiThird0Thick",10.0);
  Control.addVariable("SiThirdMat0","Silicon80K");
  Control.addVariable("SiThirdTemp0",80.0);
  
  return;
}

}  // NAMESPACE setVariable
