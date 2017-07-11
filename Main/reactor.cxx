/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   Main/reactor.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "InputControl.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Transform.h"
#include "Quaternion.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "surfIndex.h"
#include "Object.h"
#include "Qhull.h"
#include "MainProcess.h"
#include "MainInputs.h"
#include "SimProcess.h"
#include "Simulation.h"
#include "SimInput.h"
#include "mainJobs.h"
#include "Volumes.h"
#include "TallySelector.h"
#include "variableSetup.h"
#include "World.h"

#include "fissionConstruct.h"
#include "reactorTallyConstruct.h"
#include "tallyConstructFactory.h" 
#include "tallyConstruct.h" 

#include "makeDelft.h"

MTRand RNG(12345UL);

///\cond STATIC
namespace ELog 
{
  ELog::OutputLog<EReport> EM;
  ELog::OutputLog<FileReport> FM("Spectrum.log");
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
      World::createOuterObjects(*SimPtr);
      RObj.build(*SimPtr,IParam);

      RObj.setSource(*SimPtr,IParam);

      tallySystem::tallyConstructFactory FC;
      tallySystem::tallyConstruct& TC=
	tallySystem::tallyConstruct::Instance(&FC);
      TC.setFission(new tallySystem::reactorTallyConstruct);
      mainSystem::buildFullSimulation(SimPtr,IParam,Oname);


      exitFlag=SimProcess::processExitChecks(*SimPtr,IParam);
      ModelSupport::calcVolumes(SimPtr,IParam);
      ModelSupport::objectRegister::Instance().write("ObjectRegister.txt");
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
  ModelSupport::objectRegister::Instance().reset();
  ModelSupport::surfIndex::Instance().reset();
  return exitFlag;
}
