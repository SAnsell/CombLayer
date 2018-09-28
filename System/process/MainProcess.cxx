/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/MainProcess.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <list>
#include <map>
#include <string>
#include <iterator>
#include <memory>

#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "InputControl.h"
#include "inputParam.h"
#include "support.h"
#include "masterWrite.h"
#include "objectRegister.h"
#include "surfIndex.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimPHITS.h"
#include "SimFLUKA.h"
#include "SimPOVRay.h"
#include "SimMCNP.h"
#include "neutron.h"
#include "Detector.h"
#include "DetGroup.h"
#include "SimMonte.h"
#include "variableSetup.h"
#include "defaultConfig.h"
#include "DBModify.h"
#include "SimProcess.h"
#include "DefPhysics.h"
#include "TallySelector.h"
#include "TallyBuilder.h"
#include "flukaTallyBuilder.h"
#include "flukaTallySelector.h"
#include "ReportSelector.h"
#include "mainJobs.h"
#include "SimInput.h"
#include "inputSupport.h"
#include "SourceCreate.h"
#include "SourceSelector.h"
#include "flukaDefPhysics.h"
#include "flukaSourceSelector.h"
#include "ObjectAddition.h"
#include "World.h"

#include "MainProcess.h"

#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"

namespace mainSystem
{

void
activateLogging(ELog::RegMethod& RControl)
  /*!
    Sets up the Main flags for logging
    \param RControl :: Any RegMethod 
  */
{  // Set up output information:
  ELog::EM.setNBasePtr(RControl.getBasePtr());
  ELog::EM.setTypeFlag(0);
  //  ELog::EM.setActive(255 ^ ELog::debug); // No debug
  ELog::EM.setActive(255);
  ELog::EM.setDebug(ELog::debug);
  ELog::EM.setAction(ELog::error);       // Exit on Error
  ELog::EM.setColour();

  ELog::RN.setNBasePtr(0);
  ELog::RN.setActive(255);
  ELog::RN.setTypeFlag(0);
  ELog::RN.setLocFlag(0);

  masterWrite::Instance().setSigFig(9);
  masterWrite::Instance().setZero(1e-14);
  return;
}

void
getVariables(std::vector<std::string>& Names,
	     std::map<std::string,std::string>& AddValues,
	     std::map<std::string,std::string>& Values,
	     std::map<std::string,double>& IterVal)
  /*!
    Populate the variables for cmdline setup
    \param Names :: Initial set of values from ARGV
    \param AddValues :: -va options [new variables]
    \param Values :: -v options [existing variables]
    \param IterVal :: Iteration variables 
   */
{
  ELog::RegMethod RegA("MainProcess","getVariables");

  std::string varName;
  std::string varExpr;
  // Added values
  while(InputControl::flagVExtract(Names,"va","addvalue",varName,varExpr) ||
	InputControl::flagVExtract(Names,"V","addvalue",varName,varExpr))
    {
      AddValues[varName]=varExpr;
    }
  while(InputControl::flagVExtract(Names,"vx","delvalue",varName))
    {
      AddValues[varName]="DELETE";
    }
  while(InputControl::flagVExtract(Names,"v","value",varName,varExpr))
    {
      Values[varName]=varExpr;
    }

  double iValue;
  while(InputControl::flagVExtract(Names,"i","iterate",varName,iValue))
    {
      IterVal[varName]=iValue;
    }
  return;
}

void
setRunTimeVariable(FuncDataBase& Control,
		   const std::map<std::string,std::string>& VMap,
		   const std::map<std::string,std::string>& AVMap)
  /*!
    Set runtime variables 
    \param Control :: Function Data base to update
    \param VMap :: Map of variable + values
    \param AVMap :: Map of variable to add + values
   */
{
  ELog::RegMethod RegA("MainProcess[F]","setRunTimeVariable");
  
  std::map<std::string,std::string>::const_iterator mc;
  for(mc=VMap.begin();mc!=VMap.end();mc++)
    {
      if (!Control.hasVariable(mc->first))
        {
	  ELog::EM<<"Failure to find variable name "<<mc->first<<ELog::endCrit;
	  throw ColErr::ExitAbort(mc->first+" not found");
	}
      Control.setVariable(mc->first,mc->second);
    }
  // add/delete variables:
  for(mc=AVMap.begin();mc!=AVMap.end();mc++)
    {
      if (!Control.hasVariable(mc->first))
	{
	  ELog::EM<<"Adding variable name["<<mc->first<<"] "
                  <<mc->second<<ELog::endDiag;
	  Control.addVariable(mc->first,mc->second);
	}
      else if (mc->second=="DELETE")
        {
	  ELog::EM<<"Removing variable name["<<mc->first<<"] "<<ELog::endDiag;
          Control.removeVariable(mc->first);
        }
      else
        {
	  ELog::EM<<"Setting pre-defined variable["<<mc->first<<"] "
                  <<mc->second<<ELog::endDiag;
	  Control.setVariable(mc->first,mc->second);
	}
    }
  return;
}

void
incRunTimeVariable(FuncDataBase& Control,
		   const std::map<std::string,double>& VMap)
  /*!
    Increase runtime variables 
    \param Control :: Function Data base to update
    \param VMap :: Map of variable + values
   */
{
  ELog::RegMethod RegA("MainProcess","incRunTimeVariable");

  std::map<std::string,double>::const_iterator mc;

  for(mc=VMap.begin();mc!=VMap.end();mc++)
    {
      if (!Control.hasVariable(mc->first))
        {
	  ELog::EM<<"Failure to find variable name "<<mc->first<<ELog::endErr;
	  throw ColErr::ExitAbort(RegA.getFull());
	}
      const double Prev=Control.EvalVar<double>(mc->first);
      Control.setVariable(mc->first,Prev+mc->second);
    }
  return;
}

void
renumberCells(Simulation& System,const inputParam& IParam)
  /*!
    Renumber all the cells/surfaces
    \param System :: simulation to renumber
    \param IParam :: Parameters to use.
  */
{
  ELog::RegMethod RegA("MainProcess","renumberCells");

  if (!IParam.flag("renum") && !IParam.flag("cellRange"))
    return;
  
  if (IParam.flag("renum"))
    {
      std::vector<int> rOffset;
      std::vector<int> rRange;
      // int cOffset=IParam.getValue<int>("cellRange",0);
      // int cRange=IParam.getValue<int>("cellRange",1);

      size_t i=0;
      const size_t dataCnt(IParam.itemCnt("renum"));
      while(i<dataCnt)
	{
	  const std::string& Name=
	    IParam.getValue<std::string>("renum",i);
	  const std::string& Range=
	    (i+1<dataCnt) ? IParam.getValue<std::string>("renum",i+1) : "";

	  int xOffset(0);
	  int xRange(10000);	  
	  i+=2;
	  if (!StrFunc::convert(Name,xOffset) || 
	      !StrFunc::convert(Range,xRange))
	    {
	      xOffset=System.getFirstCell(Name);
	      xRange=System.getLastCell(Name)-xOffset;			
	      i--;              // using names
	    }
	  
	  if (xOffset>0)
	    {
	      rOffset.push_back(xOffset);
	      rRange.push_back(xRange);
	    }
	  else if (!Name.empty())
	    ELog::EM<<"Failed to understand protected unit : "
		    <<Name<<ELog::endErr;
	  else
	    i+=dataCnt;
	}

      System.validateObjSurfMap();
      System.renumberSurfaces(rOffset,rRange);
      System.renumberCells(rOffset,rRange);

      return;
    }

  return;
}

void
setVariables(Simulation& System,const inputParam& IParam,
	     std::vector<std::string>& Names)
  /*!
    Set all the variables via external means
    \param System :: Simulation
    \param IParam :: Parameter set
    \param Names :: Vector of Options and values
    \todo Value/AddValues not set bu-t used by setRunTimeVariables
  */
{
  ELog::RegMethod RegA("MainProcess","setVariables");

  std::map<std::string,std::string> Values;  
  std::map<std::string,std::string> AddValues;  
  std::map<std::string,double> IterVal;  


  for(size_t i=0;i<IParam.setCnt("xml");i++)
    {
      const std::string FileName=IParam.getValue<std::string>("xml",i);
      System.getDataBase().processXML(FileName);
    }

  mainSystem::getVariables(Names,AddValues,Values,IterVal);
  mainSystem::setRunTimeVariable(System.getDataBase(),Values,AddValues);

  if (IParam.flag("xmlout")) 
    {
      ELog::EM<<"Xout == "<<IParam.getValue<std::string>("xmlout")
	      <<ELog::endCrit;
      System.getDataBase().writeXML(IParam.getValue<std::string>("xmlout"));
    }

  if (IParam.flag("engineering"))
    {
      const size_t NP=IParam.itemCnt("engineering",0);
      FuncDataBase& Control=System.getDataBase();
      for(size_t i=0;i<NP;i++)
	{
	  const std::string KN=IParam.getValue<std::string>("engineering",i);
	  Control.addVariable(KN+"EngineeringActive",1);
	}
      if (!NP)
	Control.addVariable("EngineeringActive",1);
    }
  

  return;
}

int
extractName(std::vector<std::string>& Names,std::string& Out)
  /*!
    Extract a name from teh back of a list
    \param Names :: list of options
    \param Out :: Output value
    \return true if valid extraction
   */
{
  ELog::RegMethod RegA("MainProcess","extractName");
  if (!Names.empty())
    {
      Out=Names.back();
      if (Out[0]!='-')
	return 1;
    }
  return 0;
}

Simulation*
createSimulation(inputParam& IParam,
		 std::vector<std::string>& Names,
		 std::string& Oname)
  /*!
    Creates the simulation and populates the variables
    \param IParam :: Input Parameter 
    \param Names :: Vector of inputs
    \param Oname :: output file name
    \return Simulation type
   */
{
  ELog::RegMethod RegA("MainProcess","createSimulation");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  // Get a copy of the command used to run the program
  std::stringstream cmdLine;

  copy(Names.begin(),Names.end(),
       std::ostream_iterator<std::string>(cmdLine," "));
  Oname=InputControl::getFileName(Names);

  if (Oname[0]=='-')
    {
      IParam.writeDescription(ELog::EM.Estream());
      ELog::EM<<ELog::endDiag; 
      return 0;
    }
  // DEBUG
  if (IParam.flag("debug"))
    ELog::EM.setActive(IParam.getValue<size_t>("debug"));


  IParam.processMainInput(Names);

  Simulation* SimPtr;
  if (IParam.flag("PHITS"))
    SimPtr=new SimPHITS;
  else if (IParam.flag("FLUKA"))
    SimPtr=new SimFLUKA;
  else if (IParam.flag("PovRay"))
    SimPtr=new SimPOVRay;
  else if (IParam.flag("Monte"))
    SimPtr=new SimMonte; 
  else
    {
      SimMCNP* SMCPtr=new SimMCNP;
      SMCPtr->setMCNPversion(IParam.getValue<int>("mcnp"));
      SimPtr=SMCPtr;
    }
  OR.setObjectGroup(*SimPtr);
  buildWorld(*SimPtr);
  
  // DNF split the cells
  SimPtr->setCellDNF(IParam.getDefValue<size_t>(0,"cellDNF"));
  // CNF split the cells
  SimPtr->setCellCNF(IParam.getDefValue<size_t>(0,"cellCNF"));

  SimPtr->setCmdLine(cmdLine.str());        // set full command line
  
  return SimPtr;
}

void
buildWorld(objectGroups& OGrp)
{
  std::shared_ptr<attachSystem::FixedComp> worldPtr=
    std::make_shared<attachSystem::FixedComp>(World::masterOrigin());

  OGrp.addObject(worldPtr);
  return;
}


void
InputModifications(Simulation* SimPtr,inputParam& IParam,
		   std::vector<std::string>& Names)
  /*!
    General initial build processing of variables/input
    \param SimPtr :: Simulation Ptr [Not null]
    \param IParam :: initial Paramters
    \param Names :: Command line names
   */
{
  ELog::RegMethod RegA("MainProcess","InputModifications");

  mainSystem::setVariables(*SimPtr,IParam,Names);
  if (!Names.empty()) 
    ELog::EM<<"Unable to understand values "<<Names[0]<<ELog::endErr;

  return;
}

void
setMaterialsDataBase(const inputParam& IParam)
  /*!
    Set the different data base option for mateirals
    \param IParam :: Input param
  */
{
  ELog::RegMethod RegA("MainProcess","setMaterialsDataBase");

  const std::string materials=IParam.getValue<std::string>("matDB");
  
  // Add extra materials to the DBMaterials
  if (materials=="neutronics")
    ModelSupport::addESSMaterial();
  else if (materials=="shielding")
    ModelSupport::cloneESSMaterial();
  else if (materials=="basic")
    ModelSupport::cloneBasicMaterial();
  else if (materials=="help")
    {
      ELog::EM<<"Materials database setups:\n"
	" -- basic [System that works with standard MCNP distribution\n"
	"      (WARNING -- basic results are very approximate -- WARNING)\n"
	" -- shielding [S.Ansell original naming]\n"
	" -- neutronics [ESS Target division naming]"<<ELog::endDiag;
      throw ColErr::ExitAbort("help");
    }	
  else
    throw ColErr::InContainerError<std::string>
      (materials,"Materials Data Base type");

  if (IParam.flag("matFile"))
    {
      const size_t NCnt(IParam.itemCnt("matFile",0));
      for(size_t i=0;i<NCnt;i++)
	{
	  const std::string matFile=IParam.getValue<std::string>("matFile",i);
	  ModelSupport::processMaterialFile(matFile);
	}
    }
  return;
}
  
void
exitDelete(Simulation* SimPtr)
 /*!
   Final deletion including singletons
   \param SimPtr :: Simulation to delete
 */
{
  delete SimPtr;
  ModelSupport::surfIndex::Instance().reset();
  return;
}


void
buildFullSimFLUKA(SimFLUKA* SimFLUKAPtr,
		 const mainSystem::inputParam& IParam,
		 const std::string& OName)
  /*!
    Carry out the construction of the geometry
    and wieght/tallies
    \param SimFLUKAPtr :: Simulation point
    \param IParam :: input pararmeter
    \param OName :: output file name
   */
{
  ELog::RegMethod RegA("MainProcess[F]","buildFullSimFLUKA");

  // Definitions section 
  int MCIndex(0);
  const int multi=IParam.getValue<int>("multi");
  if (IParam.flag("noVariables"))
    SimFLUKAPtr->setNoVariables();

  ModelSupport::setDefaultPhysics(*SimFLUKAPtr,IParam);

  flukaSystem::tallySelection(*SimFLUKAPtr,IParam);
  //
  SimFLUKAPtr->processActiveMaterials();
  SimProcess::importanceSim(*SimFLUKAPtr,IParam);
  
  //  SimProcess::inputProcessForSim(*SimMCPtr,IParam); // energy cut etc
  tallyModification(*SimFLUKAPtr,IParam);

  SDef::flukaSourceSelection(*SimFLUKAPtr,IParam);
  //  SimFLUKAPtr->masterSourceRotation();
  // Ensure we done loop

  do
    {
      SimProcess::writeIndexSimFLUKA(*SimFLUKAPtr,OName,MCIndex);
      MCIndex++;
    }
  while(MCIndex<multi);

  return;
}

void
buildFullSimPHITS(SimPHITS* SimPHITSPtr,
		 const mainSystem::inputParam& IParam,
		 const std::string& OName)
  /*!
    Carry out the construction of the geometry
    and wieght/tallies
    \param SimFLUKAPtr :: Simulation point
    \param IParam :: input pararmeter
    \param OName :: output file name
   */
{
  ELog::RegMethod RegA("MainProcess[F]","buildFullSimPHITS");


  ModelSupport::setDefaultPhysics(*SimPHITSPtr,IParam);
  SimPHITSPtr->prepareWrite();
  
  // tallySystem::tallySelection(*SimPHITSPtr,IParam);
  SimProcess::importanceSim(*SimPHITSPtr,IParam);

  SimProcess::inputProcessForSim(*SimPHITSPtr,IParam); // energy cut etc
  //  tallyModification(*SimPHITSPtr,IParam);

  SDef::sourceSelection(*SimPHITSPtr,IParam);
  //  SimPHITSPtr->masterSourceRotation();
  // Ensure we done loop
  
  int MCIndex(0);
  const int multi=IParam.getValue<int>("multi");
  do
    {
      SimProcess::writeIndexSimPHITS(*SimPHITSPtr,OName,MCIndex);
      MCIndex++;
    }
  while(MCIndex<multi);

  return;
}

void
buildFullSimMCNP(SimMCNP* SimMCPtr,
		 const mainSystem::inputParam& IParam,
		 const std::string& OName)
  /*!
    Carry out the construction of the geometry
    and wieght/tallies
    \param SimPtr :: Simulation point
    \param IParam :: input pararmeter
    \param OName :: output file name
   */
{
  // Definitions section 
  int MCIndex(0);
  const int multi=IParam.getValue<int>("multi");

  
  ModelSupport::setDefaultPhysics(*SimMCPtr,IParam);
  SimMCPtr->prepareWrite();

  // From tallybuilder
  tallySystem::tallySelection(*SimMCPtr,IParam);
   //

  SimProcess::importanceSim(*SimMCPtr,IParam);

  SimProcess::inputProcessForSim(*SimMCPtr,IParam); // energy cut etc
  tallyModification(*SimMCPtr,IParam);

  SDef::sourceSelection(*SimMCPtr,IParam);
  //  SimMCPtr->masterSourceRotation();
  // Ensure we done loop
  do
    {
      SimProcess::writeIndexSim(*SimMCPtr,OName,MCIndex);
      MCIndex++;
    }
  while(MCIndex<multi);

  return;
}

void
buildFullSimPOVRay(SimPOVRay* SimPOVRayPtr,
		   const mainSystem::inputParam& IParam,
		   const std::string& OName)
/*!
    Carry out the construction of the geometry
    and wieght/tallies
    \param SimPOVRayPtr :: Simulation point
    \param IParam :: input pararmeter
    \param OName :: output file name
   */
{
  ELog::RegMethod RegA("MainProcess[F]","buildFullSimFLUKA");
  // Definitions section 

  // if (IParam.flag("noVariables"))
  //   SimPOVRayPtr->setNoVariables();

  SimPOVRayPtr->prepareWrite();
  SimPOVRayPtr->write(OName+".x");

  return;
}

void
buildFullSimulation(Simulation* SimPtr,
                    const mainSystem::inputParam& IParam,
                    const std::string& OName)
  /*!
    Carry out the construction of the geometry
    and wieght/tallies
    \param SimPtr :: Simulation point
    \param IParam :: input pararmeter
    \param OName :: output file name
   */
{
  ELog::RegMethod RegA("MainProcess[F]","buildFullSimulation");

  ModelSupport::objectAddition(*SimPtr,IParam);
  
  SimPtr->removeComplements();
  SimPtr->removeDeadSurfaces(0);
  
  ModelSupport::setDefRotation(*SimPtr,IParam);
  SimPtr->masterRotation();

  reportSelection(*SimPtr,IParam);
  SimPtr->createObjSurfMap();
  //  SimPtr->createObjSurfMap();
  
  if (createVTK(IParam,SimPtr,OName))
    return;

  //  UGLY CASTS to be removed
  SimMCNP* SimMCPtr=dynamic_cast<SimMCNP*>(SimPtr);
  if (SimMCPtr)
    {
      buildFullSimMCNP(SimMCPtr,IParam,OName);
      return;
    }
  
  SimFLUKA* SimFLUKAPtr=dynamic_cast<SimFLUKA*>(SimPtr);
  if (SimFLUKAPtr)
    {      
      buildFullSimFLUKA(SimFLUKAPtr,IParam,OName);
      return;
    }

  SimPOVRay* SimPOVPtr=dynamic_cast<SimPOVRay*>(SimPtr);
  if (SimPOVPtr)
    {      
      buildFullSimPOVRay(SimPOVPtr,IParam,OName);
      return;
    }

  SimPHITS* SimPHITSPtr=dynamic_cast<SimPHITS*>(SimPtr);
  if (SimPHITSPtr)
    {      
      buildFullSimPHITS(SimPHITSPtr,IParam,OName);
      return;
    }

  return;
}



                      
}  // NAMESPACE mainSystem

