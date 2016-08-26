/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   Main/simple.cxx
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
#include <vector>
#include <set>
#include <map>
#include <list>
#include <string>
#include <algorithm>
#include <memory>
#include <array>
#include <boost/format.hpp>

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
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "TallyCreate.h"
#include "MeshCreate.h"
#include "Transform.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "masterWrite.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "ModeCard.h"
#include "PhysCard.h"
#include "PhysImp.h"
#include "LSwitchCard.h"
#include "Source.h"
#include "KCode.h"
#include "PhysicsCards.h"
#include "DefPhysics.h"
#include "BasicWWE.h"
#include "MainProcess.h"
#include "MainInputs.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "ReadFunctions.h"
#include "Simulation.h"
#include "SimPHITS.h"
#include "variableSetup.h"
#include "weightManager.h"
#include "SourceCreate.h"
#include "SourceSelector.h"
#include "mainJobs.h"
#include "InputControl.h"
#include "Volumes.h"
#include "PointWeights.h"

MTRand RNG(12345UL);

namespace ELog 
{
  ELog::OutputLog<EReport> EM;
  ELog::OutputLog<FileReport> FM("Spectrum.log");
  ELog::OutputLog<FileReport> RN("Renumber.txt");   ///< Renumber
  ELog::OutputLog<StreamReport> CellM;
}

int 
main(int argc,char* argv[])
{
  int exitFlag(0);                // Value on exit
  ELog::RegMethod RControl("","main");
  mainSystem::activateLogging(RControl);
  std::string Oname;
  std::string Fname;
  std::vector<std::string> Names;

  Simulation* SimPtr(0);
  try
    {
      // PROCESS INPUT:
      InputControl::mainVector(argc,argv,Names);
      mainSystem::inputParam IParam;
      createInputs(IParam);
      Simulation* SimPtr=createSimulation(IParam,Names,Oname);
      
      // The big variable setting
      setVariable::EssVariables(SimPtr->getDataBase());
      mainSystem::setDefUnits(SimPtr->getDataBase(),IParam);
      InputModifications(SimPtr,IParam,Names);
      
      // Definitions section 
      int MCIndex(0);
      const int multi=IParam.getValue<int>("multi");
      while(MCIndex<multi)
	{
	  if (MCIndex)
	    {
	      ELog::EM.setActive(4);    // write error only
	      ELog::FM.setActive(4);    
	      ELog::RN.setActive(0);    
	    }
	  SimPtr->resetAll();
	  
	  SimPtr->readMaster(Fname);
	  SDef::sourceSelection(*SimPtr,IParam);
	  
	  SimPtr->removeComplements();
	  SimPtr->removeDeadSurfaces(0);         
	  ModelSupport::setDefaultPhysics(*SimPtr,IParam);
	  
	  const int renumCellWork=tallySelection(*SimPtr,IParam);
	  SimPtr->masterRotation();
	  
	  if (createVTK(IParam,SimPtr,Oname))
	    {
	      delete SimPtr;
	      ModelSupport::objectRegister::Instance().reset();
	      ModelSupport::surfIndex::Instance().reset();
	      return 0;
	    }
	  
	  if (IParam.flag("endf"))
	    SimPtr->setENDF7();
	  
	  SimProcess::importanceSim(*SimPtr,IParam);
	  SimProcess::inputProcessForSim(*SimPtr,IParam); // energy cut etc
	  
	  if (renumCellWork)
	    tallyRenumberWork(*SimPtr,IParam);
	  tallyModification(*SimPtr,IParam);
	  
	  // Ensure we done loop
	  SimProcess::writeIndexSim(*SimPtr,Oname,MCIndex);
	  MCIndex++;
	}
      
      exitFlag=SimProcess::processExitChecks(*SimPtr,IParam);
      ModelSupport::calcVolumes(SimPtr,IParam);
      ModelSupport::objectRegister::Instance().write("ObjectRegister.txt");
    }
  catch (ColErr::ExitAbort& EA)
    {
      if (!EA.pathFlag())
	ELog::EM<<"Exiting from "<<EA.what()<<ELog::endCrit;
      exitFlag= -2;
    }
  catch (ColErr::ExBase& A)
    {
      ELog::EM<<"\nEXCEPTION FAILURE :: "
	      <<A.what()<<ELog::endCrit;
      exitFlag = -1;
    }
  delete SimPtr; 
  ModelSupport::objectRegister::Instance().reset();
  ModelSupport::surfIndex::Instance().reset();
  masterRotate::Instance().reset();
  return exitFlag;
  
}
