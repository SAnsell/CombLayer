/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   Main/fullBuild.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "SimProcess.h"
#include "SimInput.h"
#include "Simulation.h"
#include "SimPHITS.h"
#include "variableSetup.h"
#include "ImportControl.h"
#include "SourceSelector.h"
#include "makeTS2.h"
#include "chipDataStore.h"
#include "TallySelector.h"
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
  std::map<std::string,double> IterVal;           // Variable to iterate 
  std::string Oname;

  Simulation* SimPtr(0);
  try
    {
      // PROCESS INPUT:
      InputControl::mainVector(argc,argv,Names);
      mainSystem::inputParam IParam;
      createFullInputs(IParam);
      
      const int iteractive(IterVal.empty() ? 0 : 1);   
      
      
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
      
      // Definitions section 
      RNG.seed(static_cast<unsigned int>(IParam.getValue<long int>("random")));
      const std::string rotFlag=ModelSupport::setDefRotation(IParam);
      
      int MCIndex(0);
      const int multi=IParam.getValue<int>("multi");

      ELog::EM<<"FULLBUILD : variable hash: "
	      <<SimPtr->getDataBase().variableHash()
	      <<ELog::endBasic;
	
      while(MCIndex<multi)
        {
	  if (MCIndex)
	    {
	      ELog::EM.setActive(4);    // write error only
	      ELog::FM.setActive(4);    
	      ELog::RN.setActive(0);    
	      if (iteractive)
		mainSystem::incRunTimeVariable
		  (SimPtr->getDataBase(),IterVal);
	    }
	  SimPtr->resetAll();
	  World::createOuterObjects(*SimPtr);
	  moderatorSystem::makeTS2 TS2Obj;
	  TS2Obj.build(SimPtr,IParam);
	  // This for chipObjBuild
	  SDef::sourceSelection(*SimPtr,IParam);

	  //      WeightSystem::addForcedCollision(Sim,40.0);
	  SimPtr->removeComplements();
	  SimPtr->removeDeadSurfaces(0);         

	  ModelSupport::setDefaultPhysics(*SimPtr,IParam);

	  // Sets a line of tallies at different angles
	  //	  TMRSystem::setAllTally(Sim,SInfo);

	  const int renumCellWork=beamTallySelection(*SimPtr,IParam);

	  if (!rotFlag.empty())
	    {
	      ModelSupport::setItemRotate(World::masterTS2Origin(),rotFlag);
	      ModelSupport::setDefRotation(IParam);
	    }
	  SimPtr->masterRotation();
  //	  tallySystem::addHeatBlock(*SimPtr,heatCells);
	  

	  if (IParam.flag("endf"))
	    SimPtr->setENDF7();


	  // NOTE : This flag must be set in tally== beamline standard
	  SimProcess::importanceSim(*SimPtr,IParam);
	  SimProcess::inputPatternSim(*SimPtr,IParam); // energy cut etc
	  if (createVTK(IParam,SimPtr,Oname))
	    {
	      mainSystem::exitDelete(SimPtr);
	      return 0;
	    }

	  if (renumCellWork)
	    tallyRenumberWork(*SimPtr,IParam);
	  tallyModification(*SimPtr,IParam);

	  // Ensure we done loop
	  do
	    {
	      SimProcess::writeIndexSim(*SimPtr,Oname,MCIndex);
	      MCIndex++;
	    }
	  while(!iteractive && MCIndex<multi);
	}
      if (IParam.flag("cinder"))
	SimPtr->writeCinder();

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
  mainSystem::exitDelete(SimPtr);
  ModelSupport::objectRegister::Instance().reset();
  ModelSupport::surfIndex::Instance().reset();
  return exitFlag;
}

