/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   build/ts2VarModify.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "support.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "InputControl.h"
#include "objectRegister.h"
#include "Simulation.h"
#include "SimPHITS.h"
#include "variableSetup.h"
#include "MainProcess.h"

namespace mainSystem
{

void
TS2InputModifications(Simulation* SimPtr,inputParam& IParam,
		      std::vector<std::string>& Names)
  /*!
    TS2 specific initial build processing of variables/input
    \param SimPtr :: Simulation Ptr [Not null]
    \param IParam :: initial Paramters
    \param Names :: Command line names
   */
{
  ELog::RegMethod RegA("MainProcess","TS2InputModifications");

  //The big variable setting
  setVariable::TS2layout(SimPtr->getDataBase());

  if (!IParam.flag("exclude") ||
      !IParam.compValue("E",std::string("chipIR")))
    setVariable::ChipVariables(SimPtr->getDataBase());

  if (!IParam.flag("exclude") ||
      !IParam.compNoCaseValue("E",std::string("imat")))
    setVariable::IMatVariables(SimPtr->getDataBase());

  if (!IParam.flag("exclude") ||
      !IParam.compNoCaseValue("E",std::string("zoom")))
    setVariable::ZoomVariables(IParam,SimPtr->getDataBase());
    
  if (IParam.flag("bolts"))
    setVariable::RefBolts(SimPtr->getDataBase());

  mainSystem::setVariables(*SimPtr,IParam,Names);
  if (!Names.empty()) 
    ELog::EM<<"Unable to understand values "<<Names[0]<<ELog::endErr;

  return;
}

} // namespace
