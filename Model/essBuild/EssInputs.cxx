/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/EssInputs.cxx
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
#include <vector>
#include <set>
#include <list>
#include <map>
#include <string>
#include <iterator>
#include <memory>


#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "InputControl.h"
#include "inputParam.h"
#include "MainInputs.h"

namespace mainSystem
{
void
createESSInputs(inputParam& IParam)
  /*!
    Set the specialise inputs for the ESS
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainProcess::","createESSInputs");
  createInputs(IParam);
  
  //  IParam.setValue("sdefEnergy",2503.0);    
  IParam.setValue("sdefType",std::string("ess"));  
  IParam.setValue("targetType",std::string("Bilbao"));
  
  IParam.regDefItem<std::string>("lowMod","lowModType",1,std::string("None"));
  IParam.regDefItem<std::string>("topMod","topModType",1,std::string("Butterfly"));
  IParam.regDefItem<std::string>("lowPipe","lowPipeType",1,std::string("side"));
  IParam.regDefItem<std::string>("topPipe","topPipeType",1,std::string("side"));
  IParam.regDefItem<std::string>("iradLine","iradLineType",1,
                                 std::string("void"));
  
  IParam.regMulti("bunkerChicane","bunkerChicane",1000,1);
  IParam.regMulti("bunkerFeed","bunkerFeed",1000,1);
  IParam.regMulti("bunkerPillars","bunkerPillars",1000,1);
  IParam.regMulti("bunkerQuake","bunkerQuake",1000,1);
  IParam.regMulti("iradObj","iradObject",1000,3);
  
  IParam.regDefItem<std::string>("bunker","bunkerType",1,std::string("null"));
  IParam.regMulti("beamlines","beamlines",1000);
  IParam.regDefItem<int>("nF5", "nF5", 1,0);


  IParam.setDesc("bunkerFeed","Creates feedthroughs in bunker");
  IParam.setDesc("beamlines","Creates beamlines on the main model");
  IParam.setDesc("lowMod","Type of low moderator to be built");
  IParam.setDesc("topMod","Type of top moderator to be built");
  IParam.setDesc("lowPipe","Type of low moderator pipework");
  IParam.setDesc("topPipe","Type of top moderator pipework");
  IParam.setDesc("iradLine","Build an irradiation line [void for none]");
  IParam.setDesc("iradObj","Build an irradiation object [void for none]");
  IParam.setDesc("beamlines","Build beamlines [void for none]");
  IParam.setDesc("bunker","Build bunker [void for none [A-D]");
  IParam.setDesc("nF5","Number of F5 collimators to build. \n"
		 "  -- The collimators will be named as F5, F15, etc.\n"
		 "  -- The corresponding variables must exist.");

  return;
}

} // NAMESPACE Main Inputs
