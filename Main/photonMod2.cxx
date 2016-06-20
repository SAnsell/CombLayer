/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   Main/photonMod2.cxx
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
#include "Rules.h"
#include "surfIndex.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "MainProcess.h"
#include "SimProcess.h"
#include "Simulation.h" 
#include "SimPHITS.h"
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "mainJobs.h"
#include "Volumes.h"
#include "DefPhysics.h"
#include "variableSetup.h"
#include "ImportControl.h"
#include "SourceCreate.h"
#include "SourceSelector.h"
#include "TallySelector.h"
#include "tallyConstructFactory.h"
#include "World.h"
#include "SimInput.h"

#include "makePhoton2.h"

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
  int exitFlag(0);                // Value on exit
  ELog::RegMethod RControl("","main");
  mainSystem::activateLogging(RControl);

  std::string Oname;
  std::vector<std::string> Names;  

  Simulation* SimPtr(0);
  try
    {
      // PROCESS INPUT:
      InputControl::mainVector(argc,argv,Names);
      mainSystem::inputParam IParam;
      createPhotonInputs(IParam);

      SimPtr=createSimulation(IParam,Names,Oname);
      if (!SimPtr) return -1;

      // The big variable setting
      setVariable::PhotonVariables(SimPtr->getDataBase());
      InputModifications(SimPtr,IParam,Names);
      mainSystem::setMaterialsDataBase(IParam);

      // Definitions section 
      int MCIndex(0);
      const int multi=IParam.getValue<int>("multi");
      SimPtr->resetAll();
            

      photonSystem::makePhoton2 LObj;
      World::createOuterObjects(*SimPtr);
      LObj.build(SimPtr,IParam);
      
      SDef::sourceSelection(*SimPtr,IParam);
      
      SimPtr->removeComplements();
      SimPtr->removeDeadSurfaces(0);         
      ModelSupport::setDefaultPhysics(*SimPtr,IParam);

      ModelSupport::setDefRotation(IParam);
      SimPtr->masterRotation();

      const int renumCellWork=tallySelection(*SimPtr,IParam);
      if (createVTK(IParam,SimPtr,Oname))
        {
          delete SimPtr;
          ModelSupport::objectRegister::Instance().reset();
          ModelSupport::surfIndex::Instance().reset();
          return 0;
        }

      SimProcess::importanceSim(*SimPtr,IParam);
      SimProcess::inputProcessForSim(*SimPtr,IParam); // energy cut etc
      if (renumCellWork)
        tallyRenumberWork(*SimPtr,IParam);
      tallyModification(*SimPtr,IParam);

      // Ensure we done loop
      ELog::EM<<"PHOTONMOD : variable hash: "
              <<SimPtr->getDataBase().variableHash()
              <<ELog::endBasic;
      do
        {
          SimProcess::writeIndexSim(*SimPtr,Oname,MCIndex);
          MCIndex++;
        }
      while(MCIndex<multi);
    
      exitFlag=SimProcess::processExitChecks(*SimPtr,IParam);
      ModelSupport::calcVolumes(SimPtr,IParam);
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
      ELog::EM<<"EXCEPTION FAILURE :: "
	      <<A.what()<<ELog::endCrit;
      exitFlag= -1;
    }
  delete SimPtr;
  ModelSupport::objectRegister::Instance().reset();
  ModelSupport::surfIndex::Instance().reset();
  return exitFlag;
}
