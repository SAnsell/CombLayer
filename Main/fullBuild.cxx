/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   Main/fullBuild.cxx
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
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>
#include <array>

#include "Exception.h"
#include "MersenneTwister.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "version.h"
#include "InputControl.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "DefPhysics.h"
#include "MainProcess.h"
#include "MainInputs.h"
#include "SimProcess.h"
#include "SimInput.h"
#include "Simulation.h"
#include "SimPHITS.h"
#include "variableSetup.h"
#include "ImportControl.h"
#include "makeTS2.h"
#include "chipDataStore.h"
#include "mainJobs.h"
#include "World.h"
#include "Volumes.h"

#include "MemStack.h"

// Random number
MTRand RNG(12345UL);

namespace ELog 
{
  ELog::OutputLog<EReport> EM;                      ///< Main Error log
  ELog::OutputLog<FileReport> FM("ChipDatum.pts");  ///< C[x] datum points
  ELog::OutputLog<FileReport> RN("Renumber.txt");   ///< Renumber
  ELog::OutputLog<StreamReport> CellM;              ///< Cell modifiers
}

int 
main(int argc,char* argv[])
{
  int exitFlag(0);                // Value on exit
  // For output stream
  ELog::RegMethod RControl("","main");
  mainSystem::activateLogging(RControl);

  ELog::FM<<"Version == "<<version::Instance().getVersion()+1
	  <<ELog::endDiag;

  std::vector<std::string> Names;  
  std::string Oname;

  Simulation* SimPtr(0);
  try
    {
      // PROCESS INPUT:
      InputControl::mainVector(argc,argv,Names);
      mainSystem::inputParam IParam;
      createFullInputs(IParam);
            
      // Read XML/Variable and set IParam
      SimPtr=createSimulation(IParam,Names,Oname);
      if (!SimPtr) return -1;
      
      TS2InputModifications(SimPtr,IParam,Names);
      
      if (IParam.flag("units"))
	chipIRDatum::chipDataStore::Instance().setUnits(chipIRDatum::cm);
      
      // MemStack
      if (IParam.flag("memStack"))
	{
	  ELog::MemStack::Instance().
	    setVFlag(IParam.getValue<int>("memStack"));
	}
      
      World::createOuterObjects(*SimPtr);
      moderatorSystem::makeTS2 TS2Obj;
      TS2Obj.build(SimPtr,IParam);
      
      mainSystem::buildFullSimulation(SimPtr,IParam,Oname);
      ELog::EM<<"FULLBUILD : variable hash: "
	      <<SimPtr->getDataBase().variableHash()
	      <<ELog::endBasic;

      exitFlag=SimProcess::processExitChecks(*SimPtr,IParam);
      ModelSupport::calcVolumes(SimPtr,IParam);
      chipIRDatum::chipDataStore::Instance().writeMasterTable("chipIR.table");
      ModelSupport::objectRegister::Instance().write("ObjectRegister.txt");
    }
  catch (ColErr::ExitAbort& EA)
    {
      if (!EA.pathFlag())
	ELog::EM<<"Exiting from "<<EA.what()<<ELog::endCrit;
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

  mainSystem::exitDelete(SimPtr);
  ModelSupport::objectRegister::Instance().reset();
  ModelSupport::surfIndex::Instance().reset();
  return exitFlag;
}
