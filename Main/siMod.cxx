/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   Main/siMod.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <boost/shared_ptr.hpp>
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
#include "version.h"
#include "InputControl.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Tensor.h"
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
#include "KGroup.h"
#include "LSwitchCard.h"
#include "Source.h"
#include "KCode.h"
#include "PhysicsCards.h"
#include "DefPhysics.h"
#include "BasicWWE.h"
#include "MainProcess.h"
#include "insertInfo.h"
#include "insertBaseInfo.h"
#include "InsertComp.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "ReadFunctions.h"
#include "Simulation.h"
#include "SimPHITS.h"
#include "variableSetup.h"
#include "weightManager.h"
#include "ImportControl.h"
#include "SourceCreate.h"
#include "SourceSelector.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LinearComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"

#include "siModerator.h"
#include "candleStick.h"
#include "LensSource.h"
#include "FlightLine.h"
#include "FlightCluster.h"
#include "ProtonFlight.h"
#include "layers.h"
#include "outerConstruct.h"
#include "World.h"
#include "LensTally.h"
#include "makeLens.h"

// #include "Component.h"
// #include "ObjComponent.h"
// #include "CompStore.h"
//#include "ObjStore.h"
// #include "InstrumentGeom.h"
#include "surfaceFactory.h"
#include "neutron.h"
#include "Detector.h"
// #include "Beam.h"
// #include "OutZone.h"

namespace ELog 
{
  ELog::OutputLog<EReport> EM;                     
}

MTRand RNG(12345UL);

int 
main(int argc,char* argv[])
{
  int exitFlag(0);                // Value on exit
  // For output stream
  ELog::RegMethod RControl("siMod","main");
  mainSystem::activateLogging(RControl);

  // For output stream
  std::vector<std::string> Names;  
  std::map<std::string,std::string> Values;  
  std::string Oname;

  InputControl::mainVector(argc,argv,Names);
  mainSystem::inputParam IParam;
  createSiliconInputs(IParam);

  Simulation* SimPtr=createSimulation(IParam,Names,Oname);
  if (!SimPtr) return -1;

  try
    {
      lensSystem::makeLens lensObj;
      World::createOuterObjects(*SimPtr);
      lensObj.build(SimPtr,IParam);
      
      SimPtr->removeComplements();
      SimPtr->removeDeadSurfaces(0);         
      
      SimPtr->removeOppositeSurfaces();
      // outer void to zero
      // RENUMBER:
      mainSystem::renumberCells(*SimPtr,IParam);
      
      SimProcess::writeIndexSim(*SimPtr,Oname,0);

      // ACTUAL SIM:
      
      
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

  delete SimPtr; 
  ModelSupport::objectRegister::Instance().reset();
  ModelSupport::surfIndex::Instance().reset();
  masterRotate::Instance().reset();
  return exitFlag;
  
  //   System.setDetector(200,200,30,
  // 		     Geometry::Vec3D(0,875.0,0),
  // 		     Geometry::Vec3D(detSize,0,0),
  // 		     Geometry::Vec3D(0,0,detSize),
  // 		     -0.05,-10.0);
  // ELog::EM<<"Using Beam Type:"<<System.getBeam()->className()<<ELog::endCrit;
  // System.getBeam()->setWavelength(lambda);
  // System.getBeam()->setBias(yBias);

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




