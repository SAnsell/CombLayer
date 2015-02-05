/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   Main/reactor.cxx
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
#include "TallyCreate.h"
#include "Transform.h"
#include "Quaternion.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "BasicWWE.h"
#include "MainProcess.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "Simulation.h"
#include "SimPHITS.h"
#include "mainJobs.h"
#include "Volumes.h"
#include "DefPhysics.h"
#include "TallySelector.h"
#include "variableSetup.h"
#include "World.h"

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

  std::vector<std::string> Names;  

  // PROCESS INPUT:
  InputControl::mainVector(argc,argv,Names);

  mainSystem::inputParam IParam;
  createDelftInputs(IParam);

  // NOTE THE REVERSE ORDER:
  std::string Oname;

  // Read XML/Variable
  Simulation* SimPtr=createSimulation(IParam,Names,Oname);
  if (!SimPtr) return -1;
  
  // The big variable setting
  setVariable::DelftModel(SimPtr->getDataBase());
  setVariable::DelftCoreType(IParam,SimPtr->getDataBase());
  InputModifications(SimPtr,IParam,Names);
  
  // Definitions section 
  int MCIndex(0);
  const int multi=IParam.getValue<int>("multi");

  try
    {
      while(MCIndex<multi)
	{
	  if (MCIndex)
	    {
	      ELog::EM.setActive(4);    // write error only
	      ELog::FM.setActive(4);    
	      ELog::RN.setActive(0);    
	    }

	  SimPtr->resetAll();

	  delftSystem::makeDelft RObj(IParam.getValue<std::string>("modType"));
	  World::createOuterObjects(*SimPtr);
	  RObj.build(SimPtr,IParam);
	  ModelSupport::setDefaultPhysics(*SimPtr,IParam);
	  RObj.setSource(SimPtr,IParam);

	  const int renumCellWork=
	    reactorTallySelection(*SimPtr,IParam);
	  SimPtr->removeComplements();
	  SimPtr->removeDeadSurfaces(0);         

	  if (createVTK(IParam,SimPtr,Oname))
	    {
	      delete SimPtr;
	      ModelSupport::objectRegister::Instance().reset();
	      return 0;
	    }
	  if (IParam.flag("endf"))
	    SimPtr->setENDF7();


	  // outer void to zero
	  // RENUMBER:x
	  //	  WeightSystem::simulationImp(*SimPtr,IParam);
	  WeightSystem::simulationWeights(*SimPtr,IParam);
	  mainSystem::renumberCells(*SimPtr,IParam);

	  if (IParam.flag("cinder"))
	    SimPtr->setForCinder();

	  // Cut energy tallies:
	  if (IParam.flag("ECut"))
	    SimPtr->setEnergy(IParam.getValue<double>("ECut"));
	  if (renumCellWork)
	    tallyRenumberWork(*SimPtr,IParam);
	  tallyModification(*SimPtr,IParam);

	  // Ensure we done loop
	  do
	    {
	      SimProcess::writeIndexSim(*SimPtr,Oname,MCIndex);
	      MCIndex++;
	    }
	  while(MCIndex<multi);
	}

      if (IParam.flag("cinder"))
	SimPtr->writeCinder();
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
  delete SimPtr;
  ModelSupport::objectRegister::Instance().reset();
  return exitFlag;
}
