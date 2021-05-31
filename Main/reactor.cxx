/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   Main/reactor.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include <memory>

#include "Exception.h"
#include "MersenneTwister.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "InputControl.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "surfIndex.h"
#include "MainProcess.h"
#include "MainInputs.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimInput.h"
#include "Volumes.h"
#include "variableSetup.h"

#include "makeDelft.h"

MTRand RNG(12345UL);

///\cond STATIC
namespace ELog 
{
  ELog::OutputLog<EReport> EM;
  ELog::OutputLog<FileReport> RN("Renumber.txt");   ///< Renumber
  ELog::OutputLog<StreamReport> CellM;
}
///\endcond STATIC

int 
main(int argc,char* argv[])
{  
  ELog::RegMethod RControl("","main");
  int exitFlag(0);
  mainSystem::activateLogging(RControl);

  std::string Oname;
  std::vector<std::string> Names;  
  
  Simulation* SimPtr(0);
  try
    {
      // PROCESS INPUT:
      InputControl::mainVector(argc,argv,Names);
      mainSystem::inputParam IParam;
      createDelftInputs(IParam);
      
      // Read XML/Variable
      SimPtr=createSimulation(IParam,Names,Oname);
      if (!SimPtr) return -1;
      
      // The big variable setting
      setVariable::DelftModel(SimPtr->getDataBase());
      setVariable::DelftCoreType(IParam,SimPtr->getDataBase());
      InputModifications(SimPtr,IParam,Names);
      mainSystem::setMaterialsDataBase(IParam);
	
      delftSystem::makeDelft RObj;
      RObj.build(*SimPtr,IParam);

      //      RObj.setSource(*SimPtr,IParam);

      mainSystem::buildFullSimulation(SimPtr,IParam,Oname);


      exitFlag=SimProcess::processExitChecks(*SimPtr,IParam);
      ModelSupport::calcVolumes(SimPtr,IParam);
      SimPtr->objectGroups::write("ObjectRegister.txt");
    }
  catch (ColErr::ExitAbort& EA)
    {
      exitFlag=-2;
    }
  catch (ColErr::ExBase& A)
    {
      ELog::EM<<"\nEXCEPTION FAILURE :: "
	      <<A.what()<<ELog::endCrit;
      exitFlag= -1;
    }
  catch (...)
    {
      ELog::EM<<"GENERAL EXCEPTION"<<ELog::endCrit;
      exitFlag= -3;
    }

  delete SimPtr;
  ModelSupport::surfIndex::Instance().reset();
  return exitFlag;
}
