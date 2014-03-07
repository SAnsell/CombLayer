/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/MainProcess.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <boost/format.hpp>
#include <boost/array.hpp>
#include <boost/multi_array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "InputControl.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "support.h"
#include "masterWrite.h"
#include "objectRegister.h"
#include "Simulation.h"
#include "SimPHITS.h"
#include "neutron.h"
#include "Detector.h"
#include "DetGroup.h"
#include "SimMonte.h"
#include "variableSetup.h"
#include "MainProcess.h"

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

  ELog::FM.setNBasePtr(RControl.getBasePtr());
  ELog::FM.setActive(255);
  ELog::FM.setTypeFlag(0);

  ELog::RN.setNBasePtr(0);
  ELog::RN.setActive(255);
  ELog::RN.setTypeFlag(0);

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
  ELog::RegMethod RegA("mainProcess","setRunTimeVariable");

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
  for(mc=AVMap.begin();mc!=AVMap.end();mc++)
    {
      if (!Control.hasVariable(mc->first))
	{
	  ELog::EM<<"Adding variable name "<<mc->first<<ELog::endDiag;
	  Control.addVariable(mc->first,mc->second);
	}
      else
	{
	  ELog::EM<<"Setting undefined variable "<<mc->second<<ELog::endErr;
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
createInputs(inputParam& IParam)
  /*!
    Sets all the input paramter flags
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainProcess::","createInputs");

  std::vector<std::string> RItems(10,"");

  IParam.regFlag("a","axis");
  IParam.regItem<std::string>("angle","angle");
  IParam.regDefItem<int>("c","cellRange",2,0,0);
  IParam.regItem<double>("C","ECut");
  IParam.regFlag("cinder","cinder");
  IParam.regItem<int>("d","debug");
  IParam.regDefItem<std::string>("dc","doseCalc",1,"InternalDOSE");
  IParam.regFlag("e","endf");
  IParam.regMulti<std::string>("E","exclude",1);
  IParam.regDefItem<double>("electron","electron",1,-1.0);
  IParam.regFlag("help","helf");
  IParam.regMulti<std::string>("i","iterate",1);
  IParam.regItem<std::string>("I","isolate");
  IParam.regDefItemList<std::string>("imp","importance",10,RItems);
  IParam.regDefItem<int>("m","multi",1,1);
  IParam.regFlag("M","mesh");
  IParam.regItem<Geometry::Vec3D>("MA","meshA");
  IParam.regItem<Geometry::Vec3D>("MB","meshB");
  IParam.regItem<size_t>("MN","meshNPS",3);
  IParam.regFlag("md5","md5");
  IParam.regItem<int>("memStack","memStack");
  IParam.regDefItem<int>("n","nps",1,10000);
  IParam.regFlag("p","PHITS");
  IParam.regFlag("Monte","Monte");
  IParam.regDefItem<double>("photon","photon",1,0.001);

  IParam.regDefItemList<std::string>("r","renum",10,RItems);
  IParam.regFlag("sdefVoid","sdefVoid");
  IParam.regDefItem("sdefEnergy","sdefEnergy",1,800.0);
  IParam.regDefItem<std::string>("sdefType","sdefType",1,"");
  IParam.regDefItem<std::string>("physModel","physicsModel",1,"CEM03"); 
  IParam.regDefItem<double>("SA","sdefAngle",1,35.0);
  IParam.regItem<std::string>("SF","sdefFile");
  IParam.regDefItem<int>("SI","sdefIndex",1,1);
  IParam.regDefItem<std::string>("SObj","sdefObj",1,"chipIRGuide");
  IParam.regItem<Geometry::Vec3D>("SP","sdefPos");
  IParam.regItem<double>("SR","sdefRadius");
  IParam.regItem<Geometry::Vec3D>("SV","sdefVec");
  IParam.regItem<double>("SZ","sdefZRot");
  IParam.regDefItem<long int>("s","random",1,375642321L);
  // std::vector<std::string> AItems(15);
  // IParam.regDefItemList<std::string>("T","tally",15,AItems);
  IParam.regMulti<std::string>("T","tally",25,0);
  IParam.regMulti<std::string>("TC","tallyCells",3,2);
  IParam.regMulti<int>("TGrid","TGrid",3,2);
  IParam.regMulti<std::string>("TMod","tallyMod",4,1);
  IParam.regFlag("TW","tallyWeight");
  IParam.regItem<std::string>("TX","Txml",1);
  IParam.regItem<std::string>("targetType","targetType",1);
  IParam.regDefItem<int>("u","units",1,0);
  IParam.regItem<size_t>("validCheck","validCheck",1);
  IParam.regFlag("um","voidUnMask");
  IParam.regItem<double>("volume","volume",4);
  IParam.regDefItem<int>("VN","volNum",1,20000);
  IParam.regFlag("void","void");
  IParam.regFlag("vtk","vtk");
  std::vector<std::string> VItems(15,"");
  IParam.regDefItemList<std::string>("vmat","vmat",15,VItems);
  IParam.regFlag("vcell","vcell");

  IParam.regItem<double>("w","weight");
  IParam.regItem<Geometry::Vec3D>("WP","weightPt");
  IParam.regItem<double>("WTemp","weightTemp",1);
  IParam.regDefItem<std::string>("WType","weightType",1,"basic");

  IParam.regDefItem<std::string>("X","xmlout",1,"Model.xml");
  IParam.regMulti<std::string>("x","xml",1,1);
  
  IParam.setDesc("angle","Orientate to component [name]");
  IParam.setDesc("axis","Rotate to main axis rotation [TS2]");
  IParam.setDesc("c","Cells to protect");
  IParam.setDesc("ECut","Cut energy");
  IParam.setDesc("cinder","Outer Cinder files");
  IParam.setDesc("d","debug flag");
  IParam.setDesc("dc","Dose flag (internalDOSE/DOSE)");
  IParam.setDesc("e","Convert materials to ENDF-VII");
  IParam.setDesc("electron","Add electron physics at Energy");
  IParam.setDesc("E","exclude part of the simualtion [chipir/zoom]");
  IParam.setDesc("help","Help on the diff options for building [only TS1] ");
  IParam.setDesc("i","iterate on variables");
  IParam.setDesc("I","Isolate component");
  IParam.setDesc("imp","Importance regions");
  IParam.setDesc("m","Create multiple files (diff: RNDseed)");
  IParam.setDesc("M","Add mesh tally");
  IParam.setDesc("MA","Lower Point in mesh tally");
  IParam.setDesc("MB","Upper Point in mesh tally");
  IParam.setDesc("MN","Number of points [3]");
  IParam.setDesc("md5","MD5 track of cells");
  IParam.setDesc("memStack","Memstack verbrosity value");
  IParam.setDesc("n","Number of starting particles");
  IParam.setDesc("p","PHITS output");
  IParam.setDesc("Monte","MonteCarlo capable simulation");
  IParam.setDesc("photon","Photon Cut energy");
  IParam.setDesc("r","Renubmer cells");
  IParam.setDesc("s","RND Seed");
  IParam.setDesc("SF","File read source");
  IParam.setDesc("SA","Source Angle [deg]");
  IParam.setDesc("SI","Source Index value [1:2]");
  IParam.setDesc("SObj","Source Initialization Object");
  IParam.setDesc("sdefType","Source Type (TS1/TS2)");
  IParam.setDesc("physModel","Physics Model"); 
  IParam.setDesc("SP","Source start point");
  IParam.setDesc("SV","Sourece direction vector");
  IParam.setDesc("SZ","Source direction: Rotation to +ve Z [deg]");
  IParam.setDesc("T","Tally type [set to -1 to see all help]");
  IParam.setDesc("TC","Tally cells for a f4 cinder tally");
  //  IParam.setDesc("TNum","Tally ");
  IParam.setDesc("TMod","Modify tally [help for description]");
  IParam.setDesc("TGrid","Set a grid on a point tally [tallyN NXpts NZPts]");
  IParam.setDesc("TW","Activate tally pd weight system");
  IParam.setDesc("Txml","Tally xml file");
  IParam.setDesc("targetType","Name of target type");
  IParam.setDesc("u","Units in cm");
  IParam.setDesc("um","Unset void area (from imp=0)");
  IParam.setDesc("void","Adds the void card to the simulation");
  IParam.setDesc("volume","Create volume about point/radius for f4 tally");
  IParam.setDesc("vtk","Write out VTK plot mesh");
  IParam.setDesc("vmat","sections to be written by vmat");
  IParam.setDesc("VN","Number of points in the volume integration");
  IParam.setDesc("validCheck","Run simulation to check for validity");

  IParam.setDesc("w","weightBias");
  IParam.setDesc("WType","Initial model for weights [help for info]");
  IParam.setDesc("WTemp","Temperature correction for weights");
  IParam.setDesc("WP","Weight bias Point");

  IParam.setDesc("x","XML input file");
  IParam.setDesc("X","XML output file");
  return;
}


void
createSiliconInputs(inputParam& IParam)
  /*!
    Create the specialized input for silicon simulation
    \param IParam :: Input parameters
  */
{
  ELog::RegMethod RegA("MainProcess[F]","createSiliconInputs");

  IParam.regItem<double>("b","bias");
  IParam.regDefItem<double>("d","detSize",1,50.0);
  IParam.regDefItem<double>("l","lambda",1,1.0);
  IParam.regDefItem<std::string>("m","material",1,"Poly");
  IParam.regDefItem<std::string>("si","silicon",1,"Silicon");
  IParam.regDefItem<int>("n","nps",1,20);
  IParam.regItem<std::string>("o","output");
  IParam.regDefItem<long int>("s","random",1,37564232L);

  IParam.setDesc("bias","");
  IParam.setDesc("detSize","Width/Height of the area-detector");
  IParam.setDesc("lambda","Wavelength in angstrom");
  IParam.setDesc("material","Moderator material");  
  IParam.setDesc("silicon","Silicon vane material");
  IParam.setDesc("nps","Number of simulated points");
  IParam.setDesc("output","Output file");
  IParam.setDesc("random","");

  return;
}

void 
createSinbadInputs(inputParam& IParam)
  /*!
    Set the specialise inputs for sinbad
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainProcess::","createSinbadInputs");

  createInputs(IParam);
  IParam.setValue("sdefType",std::string("sinbad"));    

  IParam.regDefItem<std::string>("preName","preName",1,"49");

  IParam.setDesc("preName","Experiment type for sinbad configuration");
  return;
}

void 
createDelftInputs(inputParam& IParam)
  /*!
    Set the specialise inputs
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainProcess::","createDelftInputs");

  IParam.regItem<std::string>("coreType","coreType",1);
  IParam.regDefItem<std::string>("modType","modType",1,"Sphere");
  IParam.regMulti<std::string>("kcode","kcode",15,0);
  IParam.regMulti<int>("ksrcMat","ksrcMat",15,0);
  IParam.regMulti<Geometry::Vec3D>("ksrcVec","ksrcVec",15,0);

  IParam.setDesc("kcode","MatN nsourcePart keff skip realRuns");
  IParam.setDesc("ksrcMat","Acceptable material number for ksrc");
  IParam.setDesc("ksrcVec","Positions for ksrc [after matN check]");

  IParam.setDesc("modType","Type of moderator (sphere/tunnel)");

  createInputs(IParam);
  return;
}

void createFullInputs(inputParam& IParam)
  /*!
    Set the specialise inputs for TS2
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainProcess::","createFullInputs");
  createInputs(IParam);

  IParam.regDefItem<std::string>("b","beamstop",1,"");
  IParam.regFlag("bolts","bolts");
  IParam.regDefItem<int>("cf","collFlag",1,7);
  IParam.regItem<std::string>("decFile","decFile");
  IParam.regDefItem<std::string>("decType","decType",1,"standard");
  IParam.regFlag("h","horr");  
  IParam.regFlag("orthoH","orthoH");  
  IParam.regMulti<std::string>("t","target",5,0);
  IParam.regDefItem<int>("zoomShutterGN","zoomShutterGN",1,4);

  IParam.setDesc("b","Add beamstop");
  IParam.setDesc("bolts","Add reflector bolts");
  IParam.setDesc("cf","Collimator Flag");
  IParam.setDesc("decFile","Surface File to replace CH4 in decoupled");
  IParam.setDesc("h","horrizontal for chipIR");
  IParam.setDesc("orthoH","Add ortho/para layer to the Coupled H2");
  IParam.setDesc("t","Use flat target");
  IParam.setDesc("zoomShutterGN","Number of sections between B4C "
		 "in zoom shutter");

  IParam.setValue("targetType",std::string("t2Target"));  
  IParam.setValue("sdefType",std::string("TS2"));  
  return;
}

void createTS1Inputs(inputParam& IParam)
  /*!
    Set the specialise inputs for TS1
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainProcess::","createFullInputs");
  createInputs(IParam);

  IParam.regDefItem<std::string>("CH4PreType","CH4PreType",1,"Wrapper");  
  IParam.setDesc("CH4PreType","Name of CH4 Premoderator");
  IParam.regDefItem<std::string>("CH4ModType","CH4ModType",1,"Basic");  
  IParam.setDesc("CH4ModType","Name of CH4 Moderator");
  IParam.regDefItem<std::string>("H2ModType","H2ModType",1,"Basic");  
  IParam.setDesc("H2ModType","Name of H2 Moderator");

  IParam.setValue("sdefType",std::string("TS1"));  
  IParam.setValue("targetType",std::string("t1PlateTarget"));  

  return;
}

void createBilbauInputs(inputParam& IParam)
  /*!
    Set the specialise inputs for TS2
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainProcess::","createFullInputs");
  createInputs(IParam);

  IParam.setValue("sdefEnergy",50.0);    
  IParam.setValue("sdefType",std::string("TS1"));  
  

  return;
}

void createBNCTInputs(inputParam& IParam)
  /*!
    Set the specialise inputs for Boron capture beamline
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainProcess::","createSNSInputs");
  createInputs(IParam);
  
  IParam.setValue("sdefType",std::string("ess"));  
  return;
}

void createCuInputs(inputParam& IParam)
  /*!
    Set the specialise inputs for TS2
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainProcess::","createFullInputs");
  createInputs(IParam);
  
  IParam.setValue("sdefType",std::string("TS1"));  
  

  return;
}

void createESSInputs(inputParam& IParam)
  /*!
    Set the specialise inputs for TS2
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainProcess::","createESSInputs");
  createInputs(IParam);
  
  //  IParam.setValue("sdefEnergy",2503.0);    
  IParam.setValue("sdefType",std::string("ess"));  
  IParam.setValue("targetType",std::string("Wheel"));  

  IParam.regDefItem<std::string>("lowMod","lowModType",1,std::string("lowMod"));
  IParam.regDefItem<std::string>("topMod","topModType",1,std::string("topMod"));
  IParam.regDefItem<std::string>("lowPipe","lowPipeType",1,std::string("side"));
  IParam.setDesc("lowMod","Type of low moderator to be built");
  IParam.setDesc("topMod","Type of top moderator to be built");
  IParam.setDesc("lowPipe","Type of low moderator pipework");
  return;
}

void createEPBInputs(inputParam& IParam)
  /*!
    Set the specialise inputs for ESS EPB magnets
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainProcess::","createEPBInputs");
  createInputs(IParam);
  
  IParam.setValue("sdefType",std::string("ess"));  
  return;
}

void createSNSInputs(inputParam& IParam)
  /*!
    Set the specialise inputs for SNS
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainProcess::","createSNSInputs");
  createInputs(IParam);
  
  IParam.setValue("sdefType",std::string("ess"));  
  return;
}

void createMuonInputs(inputParam& IParam)
  /*!
    Set the specialise inputs for EPB on TS2
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainProcess::","createMuonInputs");
  createInputs(IParam);

  IParam.setValue("sdefType",std::string("TS1EpbColl")); 
  return;
}

void
createLensInputs(inputParam& IParam)
  /*!
    Sets all the input paramter flags
    \param IParam :: Input Parameters
  */
{
  createInputs(IParam);
  IParam.regMulti<std::string>("TS","surfTally",2,2);
  IParam.regMulti<std::string>("TE","tallyEnergy",1,1);
  IParam.regMulti<std::string>("TT","tallyTime",1,1);

  IParam.setDesc("TE","Tally Energy: Energy string");
  IParam.setDesc("TT","Tally Time: Time string");
  IParam.setDesc("TS","Surface Tally: FL, dist [paired with TE/TT]");
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
  
  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  if (IParam.flag("renum"))
    {
      std::vector<int> rOffset;
      std::vector<int> rRange;
      // int cOffset=IParam.getValue<int>("cellRange",0);
      // int cRange=IParam.getValue<int>("cellRange",1);

      size_t i=0;
      const size_t dataCnt(IParam.dataCnt("renum"));
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
	      xOffset=OR.getCell(Name);
	      xRange=OR.getRange(Name);
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
    \todo Value/AddValues not set but used by setRunTimeVariables
  */
{
  ELog::RegMethod RegA("MainProcess","setVariables");

  std::map<std::string,std::string> Values;  
  std::map<std::string,std::string> AddValues;  
  std::map<std::string,double> IterVal;  


  for(size_t i=0;i<IParam.grpCnt("xml");i++)
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
  else if (IParam.flag("Monte"))
    SimPtr=new SimMonte; 
  else 
    SimPtr=new Simulation;


  SimPtr->setCmdLine(cmdLine.str());        // set full command line

  return SimPtr;
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

}  // NAMESPACE mainSystem

