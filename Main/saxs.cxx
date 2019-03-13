/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   Main/saxs.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include <boost/format.hpp>
#include <boost/multi_array.hpp>

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
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Rules.h"
#include "surfIndex.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "MainProcess.h"
#include "MainInputs.h"
#include "SimProcess.h"
#include "groupRange.h"
#include "objectGroups.h"
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
#include "World.h"
#include "SimInput.h"
#include "neutron.h"
#include "World.h"

#include "Beam.h"
#include "AreaBeam.h"
#include "DetGroup.h"
#include "SimMonte.h"

#include "makeSAXS.h"

class SimMonte;
namespace ELog 
{
  ELog::OutputLog<EReport> EM;      
  ELog::OutputLog<FileReport> FM("Spectrum.log");               
  ELog::OutputLog<FileReport> RN("Renumber.txt");   ///< Renumber
  ELog::OutputLog<StreamReport> CellM;
}

MTRand RNG(12345UL);

int 
main(int argc,char* argv[])
{
  int exitFlag(0);                // Value on exit
  // For output stream
  ELog::RegMethod RControl("saxs","main");
  mainSystem::activateLogging(RControl);
  
  // For output stream
  std::vector<std::string> Names;  
  std::map<std::string,std::string> Values;  
  std::string Oname;

  InputControl::mainVector(argc,argv,Names);
  mainSystem::inputParam IParam;
  createInputs(IParam);
  const int iteractive(0);   

  Simulation* SimPtr=createSimulation(IParam,Names,Oname);
  SimMonte* MSim=dynamic_cast<SimMonte*>(SimPtr);
  if (!SimPtr) return -1;
  try
    {
      // PROCESS INPUT:
      InputControl::mainVector(argc,argv,Names);
      mainSystem::inputParam IParam;
      createInputs(IParam);
      
      SimPtr=createSimulation(IParam,Names,Oname);
      if (!SimPtr) return -1;

      // The big variable setting
      setVariable::SAXSModel(SimPtr->getDataBase());

      InputModifications(SimPtr,IParam,Names);
      mainSystem::setMaterialsDataBase(IParam);

      saxsSystem::makeSAXS dObj; 
      World::createOuterObjects(*SimPtr);
      dObj.build(*SimPtr,IParam);

      mainSystem::buildFullSimulation(SimPtr,IParam,Oname);
      exitFlag=SimProcess::processExitChecks(*SimPtr,IParam);

      ModelSupport::calcVolumes(SimPtr,IParam);
      SimPtr->objectGroups::write("ObjectRegister.txt");
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

  if (MSim)
    {
      Transport::AreaBeam A;
      A.setWidth(0.4);
      A.setHeight(1.0);
      A.setStart(-5.0);
      A.setCent(Geometry::Vec3D(0,0,0));
      A.setWavelength(0.7);
      
      const size_t NPS(IParam.getValue<size_t>("nps"));
      const int MS(IParam.getValue<int>("MS"));
      MSim->setMS(MS);
      MSim->setBeam(A);
      MSim->runMonte(NPS);
      
      const std::string DFile=
	IParam.getValue<std::string>("detFile");
      
      MSim->writeDetectors(DFile,NPS);
    }
  else
    {
      ELog::EM<<"No simulation done : use -Monte to enable"
	      <<ELog::endDiag;
    }
  // EXIT

  delete SimPtr; 
  ModelSupport::surfIndex::Instance().reset();
  return exitFlag;
  
  //   System.setDetector(200,200,30,
  // 		     Geometry::Vec3D(0,875.0,0),
  // 		     Geometry::Vec3D(detSize,0,0),
  // 		     Geometry::Vec3D(0,0,detSize),
  // 		     -0.05,-10.0);
  // ELog::EM<<"Using Beam Type:"<<System.getBeam()->className()<<ELog::endCrit;

  // // Commented out to select random track:
  // //  System.setAimZone(&PZ);        
  // System.runMonte(nps);
  // //  System.getDet().write(outFile);
  // // TEST:
  // System.write(primaryFile,lambda);
  // System.writeMCNPX(primaryFile+".i");
  
  // System.writeXML("test.xml");
  return 0;
}




