/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/tUpgrade.cxx
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
#include <string>
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <algorithm>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
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
#include "MainTally.h"
#include "ChipTally.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LinearComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "shutterBlock.h"
#include "GeneralShutter.h"
#include "BulkInsert.h"
#include "ChipIRFilter.h"
#include "ChipIRGuide.h"
#include "ChipIRInsert.h"
#include "ChipIRSource.h"
#include "InnerWall.h"
#include "HoleUnit.h"
#include "PreCollimator.h"
#include "Collimator.h"
#include "ColBox.h"
#include "beamBlock.h"
#include "BeamStop.h"
#include "Table.h"
#include "Hutch.h"
#include "zoomInsertBlock.h"
#include "PointWeights.h"
#include "FBBlock.h"
#include "makeChipIR.h"
#include "makeZoom.h"
#include "chipDataStore.h"
#include "TallySelector.h"
#include "mainJobs.h"
#include "Volumes.h"

#include "VoidVessel.h"
#include "BulkShield.h"
#include "Groove.h"
#include "Hydrogen.h"
#include "VacVessel.h"
#include "FlightLine.h"
#include "PreMod.h"
#include "HWrapper.h"
#include "Decoupled.h"
#include "RefCutOut.h"
#include "Bucket.h"
#include "CoolPad.h"
#include "Reflector.h"
#include "World.h"

#include "TS1build.h"
#include "SpecialSurf.h"

MTRand RNG(12345UL);

namespace ELog 
{
  ELog::OutputLog<EReport> EM;
  ELog::OutputLog<FileReport> FM("Spectrum.log");
  ELog::OutputLog<FileReport> RN("Renumber.txt");   ///< Renumber
  ELog::OutputLog<StreamReport> CellM;
}

void addFlightLines(Simulation&);           ///< Adds the flight lines to the objects
void setVirtualContainers(Simulation&);                   ///< Sets the virtual containers
void addDecoupler(Simulation&);
void modifyWaterModerator(Simulation&);
std::string getSlice(const int,const int,const int,const int,const int);

int 
main(int argc,char* argv[])
{
  ELog::RegMethod RControl("","main");
  mainSystem::activateLogging(RControl);

  std::vector<std::string> Names;  
  std::map<std::string,std::string> Values;  
  std::map<std::string,std::string> AddValues;  
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
  std::string Oname("-");
  std::string Fname("-");
  if (Names.size()<2)
    {
      Fname=Names.back();   // Input file name
      Oname=Names.back();   // Output file name
      Names.pop_back();
      Names.pop_back();
    }
  if (Oname[0]=='-' || Fname[0]=='-')
    {
      IParam.writeDescription(ELog::EM.Estream());
      ELog::EM<<ELog::endDiag; 
      return -1;
    }

  IParam.processMainInput(Names);
  // DEBUG
  if (IParam.flag("debug"))
    ELog::EM.setActive(IParam.getValue<unsigned int>("debug"));

  const int iteractive(IterVal.empty() ? 0 : 1);   // Do we need to get new information etc  
  Simulation* SimPtr=(IParam.flag("PHITS")) ? new SimPHITS : new Simulation;
  SimPtr->setCmdLine(cmdLine.str());        // set full command line

  setVariable::TS1layout(SimPtr->getDataBase());
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
	      
	      if (iteractive)
		mainSystem::incRunTimeVariable
		  (SimPtr->getDataBase(),IterVal);
	    }

	  SimPtr->resetAll();
	  SimPtr->readMaster(Fname);

	  addFlightLines(*SimPtr);
	  setVirtualContainers(*SimPtr);
	  addDecoupler(*SimPtr);
      

	  SimPtr->getPC().setCells("imp",1,0);	         // World cell
	  SimPtr->getPC().setCells("imp",70212,0);	 // ??
	  // Methane Face (Far Hydrogen)
	  tallySystem::addF5Tally(*SimPtr,15,10001,5001,5002,5005,5006,-1000.0);        
	  // Methane Face (Near Hydrogen)
	  tallySystem::addF5Tally(*SimPtr,25,10002,5011,5012,5005,5006,1000.0);        
	  // Hydrogen
	  tallySystem::addF5Tally(*SimPtr,35,20002,5005,5006,21003,21004,-1000.0);      
	  tallySystem::setF5Angle(*SimPtr,35,Geometry::Vec3D(-1,0,0),-1000.0,-15.0);
	  // Water Face (non-merlin side)
	  tallySystem::addF5Tally(*SimPtr,45,41001,6002,6001,6006,6005,-1000.0);         
	  // Water Face (merlin side)
	  tallySystem::addF5Tally(*SimPtr,55,41002,6012,6011,6006,6005,1000.0);         
	  // Merlin Water:
	  tallySystem::addF5Tally(*SimPtr,65,51001,6022,6021,6005,6006,-1000.0);      

	  SimPtr->removeComplements();
	  SimPtr->removeDeadCells();            // Generic
	  SimPtr->removeDeadSurfaces(0);         

	  SimPtr->removeOppositeSurfaces();

	  ModelSupport::setDefaultPhysics(*SimPtr,IParam);

	  if (createVTK(IParam,SimPtr,Oname))
	    {
	      delete SimPtr;
	      return 0;
	    }
	  if (IParam.flag("endf"))
	    SimPtr->setENDF7();
	  createMeshTally(IParam,SimPtr);

	  // outer void to zero
	  // RENUMBER:
	  mainSystem::renumberCells(*SimPtr,IParam);

	  // WEIGHTS:
	  if (IParam.flag("weight") || IParam.flag("tallyWeight"))
	    SimPtr->calcAllVertex();

	  if (IParam.flag("weight"))
	    {
	      Geometry::Vec3D AimPoint;
	      if (IParam.flag("weightPt"))
		AimPoint=IParam.getValue<Geometry::Vec3D>("weightPt");
	      else 
		tallySystem::getFarPoint(*SimPtr,AimPoint);

	      WeightSystem::setPointWeights(*SimPtr,AimPoint,
					    IParam.getValue<double>("weight"));
	    }

	  if (IParam.flag("tallyWeight"))
	    {
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
      ModelSupport::objectRegister::Instance().write("ObjectRegister.txt");
    }
  catch (ColErr::ExBase& A)
    {
      ELog::EM<<"EXCEPTION FAILURE :: "<<A.what()<<ELog::endErr;
      return -1;
    }
  delete SimPtr;
  return 0;
}

void
setVirtualContainers(Simulation& System)
  /*!
    Create virtual contains within the system
    \param System :: Simulation model
  */
{
  System.makeVirtual(1000);        // Container for the methane moderator
  System.makeVirtual(2000);        // Container for the hydrogen moderator
  System.makeVirtual(7000);        // Container for the water
  System.makeVirtual(8000);        // Container for the merlin moderator
  System.makeVirtual(9000);        // Container for the merlin moderator
  return;
}

void
addFlightLines(Simulation& System)
  /*!
    Add flight lines 
    \param System :: Simulation object
   */
{
  ELog::RegMethod RegA("tUpgrade","addFlightLines");
  FuncDataBase& Control=System.getDataBase();

  Geometry::Vec3D Nvec;
  Geometry::Vec3D Diff;
  Geometry::Vec3D Mid;
  double S,hSep;
  std::vector<Geometry::Vec3D> OutA;
  std::vector<Geometry::Vec3D> OutB;

  // WATER flight
  Control.Parse("watAngleA");
  const double WA=Control.Eval();
  Control.Parse("watAngleB");
  const double WB=Control.Eval();
  Control.Parse("watAngleC");
  const double WC=Control.Eval();
  Control.Parse("watAngleD");
  const double WD=Control.Eval();
  // MERLIN
  Control.Parse("merlinAngleA");
  const double OA=Control.Eval();
  Control.Parse("merlinAngleB");
  const double OB=Control.Eval();

  // MERLIN TOP WATER
  Nvec=Geometry::Vec3D(0.0,cos(OA*M_PI/180.0),-sin(OA*M_PI/180.0));
  // Now get Intercept points on outside edge and creap in

  if (!ModelSupport::calcVertex(50001,50004,212,OutA,0) ||
      !ModelSupport::calcVertex(50001,50003,212,OutB,0) )
    {
      ELog::EM<<"Error finding intercept for 50001 50003/4 212"<<ELog::endErr;
      exit(1);
    }
  Diff=(OutB[0]-OutA[0])/2.0;
  S=Diff.abs();
  Mid=(OutA[0]+OutB[0])/2.0;
  hSep=Control.EvalVar<double>("merlinViewHorr")/2.0;
  
  ModelSupport::addPlane(6021,Nvec,Mid-Diff*(hSep/S));
  Nvec=Geometry::Vec3D(0.0,cos(OB*M_PI/180.0),-sin(OB*M_PI/180.0));
  ModelSupport::addPlane(6022,Nvec,Mid+Diff*(hSep/S));

  // TOP WATER
  Nvec=Geometry::Vec3D(0.0,cos(WA*M_PI/180.0),-sin(WA*M_PI/180.0));
  // Now get Intercept points on outside edge and creap in
  if (!ModelSupport::calcVertex(40001,40003,212,OutA,0) ||
      !ModelSupport::calcVertex(40001,40004,212,OutB,0) )
    {
      ELog::EM<<"Error finding intercept for 40001 40003/4 212"<<ELog::endErr;
      exit(1);
    }
  Diff=(OutB[0]-OutA[0])/2.0;
  S=Diff.abs();
  Mid=(OutA[0]+OutB[0])/2.0;
  hSep=Control.EvalVar<double>("waterViewHorr")/2.0;
  
  ModelSupport::addPlane(6001,Nvec,Mid-Diff*(hSep/S));
  Nvec=Geometry::Vec3D(0.0,cos(WB*M_PI/180.0),-sin(WB*M_PI/180.0));
  ModelSupport::addPlane(6002,Nvec,Mid+Diff*(hSep/S));
  // BASE WATER
  Nvec=Geometry::Vec3D(0.0,cos(WC*M_PI/180.0),-sin(WC*M_PI/180.0));
  // Now get Intercept points on outside edge and creap in
  if (!ModelSupport::calcVertex(40002,40003,212,OutA,0) ||
      !ModelSupport::calcVertex(40002,40004,212,OutB,0) )
    {
      ELog::EM<<"Error finding intercept for 40002 40003/4 212"<<ELog::endErr;
      exit(1);
    }
  Diff=(OutB[0]-OutA[0])/2.0;
  S=Diff.abs();
  Mid=(OutA[0]+OutB[0])/2.0;
  hSep=Control.EvalVar<double>("waterViewHorr")/2.0;
  
  ModelSupport::addPlane(6011,Nvec,Mid-Diff*(hSep/S));
  Nvec=Geometry::Vec3D(0.0,cos(WD*M_PI/180.0),-sin(WD*M_PI/180.0));
  ModelSupport::addPlane(6012,Nvec,Mid+Diff*(hSep/S));

  // METHANE flight
  Control.Parse("methAngleHSide");
  const double MA=Control.Eval();
  Control.Parse("methAngleFar");
  const double MM=Control.Eval();
  Nvec=Geometry::Vec3D(0.0,cos(MA*M_PI/180.0),-sin(MA*M_PI/180.0));

  // Now get Intercept points on outside edge and creap in
  if (!ModelSupport::calcVertex(10001,10003,202,OutA,0) ||
      !ModelSupport::calcVertex(10001,10004,202,OutB,0) )
    {
      ELog::EM<<"Error finding intercept for 10001 10003/4 202"<<ELog::endErr;
      exit(1);
    }
  Diff=(OutB[0]-OutA[0])/2.0;
  S=Diff.abs();
  Mid=(OutA[0]+OutB[0])/2.0;
  hSep=Control.EvalVar<double>("methViewHorr")/2.0;
  
  ModelSupport::addPlane(5001,Nvec,Mid-Diff*(hSep/S));

//  ModelSupport::addPlane(5001,Nvec,10001,10003,202);


  Nvec=Geometry::Vec3D(0.0,cos(MM*M_PI/180.0),-sin(MM*M_PI/180.0));
  ModelSupport::addPlane(5002,Nvec,Mid+Diff*(hSep/S));
//  ModelSupport::addPlane(5002,Nvec,10001,10004,202);
  
  //
  // Methane other Flight
  //
  // METHANE flight
  const double HA=Control.EvalVar<double>("HMAngleHSide");
  const double HM=Control.EvalVar<double>("HMAngleFarSide");

  if (!ModelSupport::calcVertex(10002,10003,202,OutA,0) ||
      !ModelSupport::calcVertex(10002,10004,202,OutB,0) )
    {
      ELog::EM<<"Error finding intercept for 10002 10003/4 202"<<ELog::endErr;
      exit(1);
    }

  Diff=(OutB[0]-OutA[0])/2.0;
  S=Diff.abs();
  Mid=(OutA[0]+OutB[0])/2.0;

  // Create Flight Edge:
  Nvec=Geometry::Vec3D(0.0,cos(HA*M_PI/180.0),-sin(HA*M_PI/180.0));  
  ModelSupport::addPlane(5011,Nvec,Mid-Diff*(hSep/S));
  Nvec=Geometry::Vec3D(0.0,cos(HM*M_PI/180.0),-sin(HM*M_PI/180.0));
  ModelSupport::addPlane(5012,Nvec,Mid+Diff*(hSep/S));

  TS1Build::addMethaneFlightLines(System);
 
  // Hydrogen flight

  Control.Parse("HAngleSide");
  const double HX=Control.Eval();
  Nvec=Geometry::Vec3D(0.0,cos(HX*M_PI/180.0),-sin(HX*M_PI/180.0));
  ModelSupport::addPlane(5021,Nvec,20002,20003,202);

  return;
}


void 
modifyWaterModerator(Simulation& System)
  /*!
    Modify the poisoning in 8101
    \param System :: Simulation model
    \param Control :: FuncDataBase
  */
{
  ELog::RegMethod RegA("tUpgrade","modifyWaterModerator");  
  FuncDataBase& Control=System.getDataBase();

  // Hull for Water moderator (inner)
  MonteCarlo::Qhull* WMod=System.findQhull(8101);
  const int pmat=Control.EvalVar<int>("watpoisonmat");
  const double Prho=System.getMaterial(pmat).getAtomDensity();

  WMod->addSurfString("(40011 : 40012)");

  MonteCarlo::Qhull PoisonLayer;  
  std::stringstream cx;

  cx<<"8103 "<<pmat<<" "<<Prho<<" ";
  PoisonLayer.setObject(cx.str());
  System.addCell(8103,PoisonLayer);
  return;
}

void
addDecoupler(Simulation& System)
  /*!
    ADDS TO CELL :: 70001 
    \param System :: Simulation model
  */
{
  ELog::RegMethod RegA("tUpgrade","addDecoupler");  
  FuncDataBase& Control=System.getDataBase();
  

  const int nvane=static_cast<int>(Control.EvalVar<double>("nvane"));
  const int nslice=static_cast<int>(Control.EvalVar<double>("nslice"));
  if (nvane<=0)  // No work
    return;
  const double vaneT=Control.EvalVar<double>("vaneThick");
  const int vmat=static_cast<int>(Control.EvalVar<double>("vanemat"));
  const double Vrho=System.getMaterial(vmat).getAtomDensity();
//  const int almat=static_cast<int>(Control.EvalVar<double>("almat"));
//  const double Alrho=System.getMaterial(almat).getAtomDensity();

  // Get  Edges
  Control.Parse("radiusInner+3*cladding+2*twater+radiusOuter+steel");
  const double YA=-Control.Eval();
  Control.Parse("-(refsize+refedge)+refyshift");
  const double YB=Control.Eval();

  const double XA= -YA;
  Control.Parse("refsize+refedge+refyshift");
  const double XB=Control.Eval();
  const double StepY=(YB-YA)/(nvane+1);
  const double StepX=(XB-XA)/(nvane+1);
    
  int cellN=7001;
  int planeN=7001;
  int sliceN=8001;

  Geometry::Vec3D Nvec(0,1,0);
  Geometry::Vec3D Zvec(0,0,1);
  Geometry::Vec3D PVec(0,0,0);
  MonteCarlo::Qhull* BasePtr=System.findQhull(70001);
  MonteCarlo::Qhull* BasePtrPlus=System.findQhull(70032);
  if (BasePtr==0 || BasePtrPlus==0)
    {
      ELog::EM<<"Not BASE CELL "<<70001<<ELog::endErr;
      exit(1);
    }
  std::vector<Geometry::Vec3D> OutA;
  std::vector<Geometry::Vec3D> OutB;
  Geometry::Vec3D VStep;
  std::ostringstream cx;
  // 70001: LEFT
  for(int i=0;i<nvane;i++)
    {
      PVec[1]=YA+(i+1)*StepY;
      ModelSupport::addPlane(planeN,Nvec,PVec);
      PVec[1]-=vaneT;
      ModelSupport::addPlane(planeN+1,Nvec,PVec);
      // ADD SLICES (Z-planes)
      ModelSupport::calcVertex(102,planeN,211,OutA,0);
      ModelSupport::calcVertex(108,planeN,211,OutB,0);
      VStep=(OutB[0]-OutA[0])/(nslice+1.0);
      ELog::EM<<"Vane == "<<VStep<<ELog::endErr;
      for(int j=1;j<nslice;j++)
	ModelSupport::addPlane(sliceN+j,Zvec,OutA[0]+VStep*j);

      MonteCarlo::Qhull A;
      cx.str("");
      cx<<cellN<<" "<<vmat<<" "<<Vrho<<" "
	  <<"101 103  -107 202 -211 ";
      cx<<-planeN<<" "<<planeN+1;
      cx<<getSlice(nslice,i % 2,-102,-108,sliceN);
      
      A.setObject(cx.str());
      while(!System.addCell(cellN,A))
	cellN++;

      MonteCarlo::Qhull Aplus;  // 70032 object
      cx.str("");
      cx<<cellN<<" "<<vmat<<" "<<Vrho<<" "
	<<"101 -102 103 -107 -108 -201 110 ";
      cx<<-planeN<<" "<<planeN+1;
      Aplus.setObject(cx.str());
      while(!System.addCell(cellN,Aplus))
	cellN++;

      cx.str("");
      cx<<" ( "<<planeN<<" : "<<-(planeN+1)<<") ";
      BasePtr->addSurfString(cx.str());
      BasePtrPlus->addSurfString(cx.str());
      planeN+=2;
    }

  // 70001: RIGHT
  for(int i=0;i<nvane;i++)
    {
      PVec[1]=XA+(i+1)*StepX;
      ModelSupport::addPlane(planeN,Nvec,PVec);
      PVec[1]-=vaneT;
      ModelSupport::addPlane(planeN+1,Nvec,PVec);
      MonteCarlo::Qhull A;
      cx.str("");
      cx<<cellN<<" "<<vmat<<" "<<Vrho<<" "
	<<"-104 -105 -106 -107 202 -211 ";
      cx<<-planeN<<" "<<planeN+1;
       A.setObject(cx.str());
      while(!System.addCell(cellN,A))
	cellN++;

      MonteCarlo::Qhull Aplus;  // 70032 object
      cx.str("");
      cx<<cellN<<" "<<vmat<<" "<<Vrho<<" "
	<<"-104 -105 -106 -107 -201 110 ";
      cx<<-planeN<<" "<<planeN+1;
      Aplus.setObject(cx.str());
      while(!System.addCell(cellN,Aplus))
	cellN++;

      cx.str("");
      cx<<" ( "<<planeN<<" : "<<-(planeN+1)<<") ";
      BasePtr->addSurfString(cx.str());
      BasePtrPlus->addSurfString(cx.str());
      planeN+=2;
    }

  return;
}

std::string
getSlice(const int Index,const int F,const int A,const int B,const int NSlice)
  /*!
    Start of a function to write out a slice component
    \param Index :: Index of the slice
    \param F :: Flag to decide flip side
    \param A :: Initial surface
    \param B :: Final surface
    \param NSlice :: Number of slices
  */
{
  std::ostringstream cx;
  if (Index<1)
    {
      cx<<" "<<A<<" "<<B;
      return cx.str();
    }

//  int first;

  cx<<" "<<((!F) ? A : NSlice);
  int i;
  for(i=F;i<Index-1;i+=2)
    cx<<" ( "<<-(NSlice+i)<<" : "<<NSlice+i+1<<" )";
  if (i==Index)
    cx<<" "<<B;
  else
    cx<<" "<<-(NSlice+i);

  return  cx.str();
}
