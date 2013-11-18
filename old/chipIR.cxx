/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/chipIR.cxx
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
#include <iterator>
#include <boost/array.hpp>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>

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
#include "LSwitchCard.h"
#include "PhysImp.h"
#include "KGroup.h"
#include "Source.h"
#include "PhysicsCards.h"
#include "AlterSurfBase.h"
#include "AlterSurfTS2.h"
#include "RemoveCell.h"
#include "TS2remove.h"
#include "TMRCreate.h"
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
#include "SourceCreate.h"
#include "SourceSelector.h"
#include "MainTally.h"
#include "ChipTally.h"
#include "VCell.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "LinearComp.h"
#include "TS2target.h"
#include "TS2flatTarget.h"
#include "shutterBlock.h"
#include "GeneralShutter.h"
#include "BulkInsert.h"
#include "ChipIRFilter.h"
#include "ChipIRGuide.h"
#include "ChipIRShutter.h"
#include "ChipIRInsert.h"
#include "ChipIRSource.h"
#include "InnerWall.h"
#include "HoleUnit.h"
#include "PreCollimator.h"
#include "Collimator.h"
#include "beamBlock.h"
#include "BeamStop.h"
#include "Table.h"
#include "Hutch.h"
#include "zoomInsertBlock.h"
#include "ZoomWeights.h"
#include "makeChipIR.h"
#include "makeZoom.h"
#include "chipDataStore.h"
#include "TallySelector.h"
#include "mainJobs.h"
#include "Volumes.h"

#include "VoidVessel.h"
#include "BulkShield.h"

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
  // For output stream
  ELog::RegMethod RControl("","main");
  mainSystem::activateLogging(RControl);

  ELog::FM<<"Version == "<<version::Instance().getVersion()+1
	  <<ELog::endDiag;

  std::vector<std::string> Names;  
  std::map<std::string,double> IterVal;           // Variable to iterate 

  // PROCESS INPUT:
  InputControl::mainVector(argc,argv,Names);
  // Get a copy of the command used to run the program
  std::stringstream cmdLine;
  copy(Names.begin(),Names.end(),
       std::ostream_iterator<std::string>(cmdLine," "));
  
  mainSystem::inputParam IParam;
  createInputs(IParam);

  // NOTE THE REVERSE ORDER:
  std::string Oname("-"),Fname("-");
  if (Names.size()>=2)
    {
      Oname=Names.back();   // Output file name
      Names.pop_back();
      Fname=Names.back();         // Master file name
      Names.pop_back();
    }
  if (Fname[0]=='-' || Oname[0]=='-')
    {
      IParam.writeDescription(ELog::EM.Estream());
      ELog::EM<<ELog::endDiag;
      return -1;
    }
  
  IParam.processMainInput(Names);
  // Units
  if (IParam.flag("units"))
    chipIRDatum::chipDataStore::Instance().setUnits(chipIRDatum::cm);

  // DEBUG
  if (IParam.flag("debug"))
    ELog::EM.setActive(IParam.getValue<int>("debug"));


  //  const int isoZoomFlag= IParam.compValue("I",std::string("zoom"));

  const int iteractive(IterVal.empty() ? 0 : 1);    // Do we need to get new information etc	
  Simulation* SimPtr=(IParam.flag("PHITS")) ? new SimPHITS : new Simulation;
  SimPtr->setCmdLine(cmdLine.str());        // set full command line

  RNG.seed(static_cast<unsigned int>(IParam.getValue<long int>("random")));

  //The big variable setting
  setVariable_TS2layout(SimPtr->getDataBase());
  setVariable_TS2model(SimPtr->getDataBase());
  mainSystem::setVariables(*SimPtr,IParam,Names);

  SimPtr->registerAlterSurface(new AlterSurfTS2(SimPtr->getDataBase()));
  SimPtr->registerRemoveCell(new TS2remove());   // database/cells get set later
  // Definitions section 

  if (IParam.flag("axis"))
    {
      masterRotate& MR = masterRotate::Instance();
      // Move X to Z:
      MR.addRotation(Geometry::Vec3D(0,1,0),
		     Geometry::Vec3D(0,0,0),
		     90.0);
      //Move XY to -X-Y 
      MR.addRotation(Geometry::Vec3D(0,0,1),
		     Geometry::Vec3D(0,0,0),
		     -90.0);
      // //Move ChipIR hutch to horrizontal
      if (IParam.flag("horr"))
	MR.addRotation(Geometry::Vec3D(0,0,1),
		       Geometry::Vec3D(0,0,0),
		       42.85);

      MR.addMirror(Geometry::Plane
		   (1,0,Geometry::Vec3D(0,0,0),
		    Geometry::Vec3D(1,0,0)));

      if (IParam.compValue("I",std::string("chipIR")))
	MR.addDisplace(Geometry::Vec3D(-1175.0,0,0));
    }

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
	      if (iteractive)
		mainSystem::incRunTimeVariable
		  (SimPtr->getDataBase(),IterVal);
	    }
	  shutterSystem::VoidVessel VObj("void");
	  shutterSystem::BulkShield BulkObj("bulk");
	  TMRSystem::TS2target TarObj("t2Target");
	  TMRSystem::TS2flatTarget TarFlatObj("t2Target");

	  hutchSystem::makeChipIR chipObj;
	  zoomSystem::makeZoom zoomObj;

	  SimPtr->resetAll();
	  SimPtr->readMaster(Fname);

	  if (!IParam.flag("target"))
	    {
	      ELog::EM<<"Creating Domed Target:"<<ELog::endCrit;
	      TarObj.setRefPlates(186,190);
	      TarObj.createAll(*SimPtr);
	    }
	  else
	    {
	      ELog::EM<<"Creating FLAT target:"<<ELog::endCrit;
	      TarFlatObj.createAll(*SimPtr);
	    }

	  TMRSystem::setVirtualContainers(*SimPtr);
	  //      TMRSystem::addSapphire(Sim);

	  TMRSystem::makeNGroove(*SimPtr);
	  TMRSystem::makeBGroove(*SimPtr);
	  TMRSystem::addIradBeamline(*SimPtr); 

	  VObj.createAll(*SimPtr);
	  BulkObj.createAll(*SimPtr,VObj);

	  // chipguide/chiphutch
	  chipObj.build(SimPtr,IParam,BulkObj);
	  zoomObj.build(SimPtr,IParam,BulkObj);
	  // This for chipObjBuild
	  SDef::sourceSelection(*SimPtr,IParam,&BulkObj,
				&chipObj.getGuide(),
				&chipObj.getHutch());

	  //	  TMRSystem::removeTallyWindows(*SimPtr);
	  SimPtr->removeComplements();
	  SimPtr->removeDeadCells();            // Generic
	  SimPtr->removeDeadSurfaces(0);         
	  SimPtr->processCellsImp();
	  SimPtr->getPC().setCells("imp",1,0);            // Set a zero cell

	  SimPtr->getPC().setNPS(IParam.getValue<int>("nps"));
	  SimPtr->getPC().setRND(IParam.getValue<long int>("random"));	

	  // Sets a line of tallies at different angles
	  //	  TMRSystem::setAllTally(Sim,SInfo);
	  tallySelection(*SimPtr,IParam,
			 BulkObj,chipObj.getGuide(),
			 chipObj.getHutch());

	  SimPtr->masterRotation();
	  if (createVTK(IParam,SimPtr,Oname))
	    {
	      delete SimPtr;
	      return 0;
	    }

	  TMRSystem::setWeights(*SimPtr);
	  //	  tallySystem::addHeatBlock(*SimPtr,heatCells);


	  if (IParam.flag("endf"))
	    SimPtr->setENDF7();

	  createMeshTally(IParam,SimPtr);

	  if (!IParam.flag("voidUnMask") && !IParam.flag("mesh") &&
	      IParam.getValue<int>("tally")!=5)
	    {
	      WeightSystem::weightManager::Instance().maskCell(74123);
	      SimPtr->getPC().setCells("imp",74123,0);      // outer void 	
	    }
	  // outer void to zero
	  // RENUMBER:
	  mainSystem::renumberCells(*SimPtr,IParam);
	  // WEIGHTS:
	  if (IParam.flag("weight"))
	    {
	      SimPtr->createObjSurfMap();
	      SimPtr->calcAllVertex();

	      Geometry::Vec3D AimPoint;
	      if (IParam.flag("weightPt"))
		AimPoint=IParam.getValue<Geometry::Vec3D>("weightPt");
	      else
		tallySystem::getFarPoint(*SimPtr,AimPoint);

	      zoomSystem::setPointWeights(*SimPtr,AimPoint,
					  IParam.getValue<double>("weight"));
	      if (IParam.flag("tallyWeight"))
		tallySystem::addPointPD(*SimPtr);
	    }

	  if (IParam.flag("cinder"))
	    SimPtr->setForCinder();
	  
	  // Cut energy tallies:
	  if (IParam.flag("ECut"))
	    SimPtr->setEnergy(IParam.getValue<double>("ECut"));

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
      ModelSupport::calcVolumes(SimPtr,IParam);

      chipIRDatum::chipDataStore::Instance().writeMasterTable("chipIR.table");
      ModelSupport::objectRegister::Instance().write("ObjectRegister.txt");
    }
  catch (ColErr::ExitAbort& EA)
    {
      delete SimPtr;
      ELog::EM<<"Exiting from "<<EA.what()<<ELog::endCrit;
      return -2;
    }
  catch (ColErr::ExBase& A)
    {
      delete SimPtr;
      ELog::EM<<"EXCEPTION FAILURE :: "<<A.what()<<ELog::endCrit;
      return -1;
    }
  delete SimPtr; 
  return 0;
}
