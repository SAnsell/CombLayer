/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/MainInputs.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "inputParam.h"

namespace mainSystem
{

void
createPHITSInputs(inputParam& IParam)
  /*!
    Create inputs specifically for PHITS
    \param IParam :: Input Parameters
   */
{
  ELog::RegMethod RegA("MainProcess[F]","createPHITSInputs");

  IParam.regItem("icntl","icntl");

  IParam.setDesc("icntl","Control parameter for output");
  return;
}
  
void
createInputs(inputParam& IParam)
  /*!
    Sets all the input paramter flags
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainProcess[F]","createInputs");

  std::vector<std::string> RItems(10,"");

  createPHITSInputs(IParam);
  
  IParam.regMulti("activation","activation",10000,1);

  IParam.regFlag("a","axis");
  IParam.regMulti("angle","angle",10000,1,8);
  IParam.regFlag("basicGeom","basicGeom");
  IParam.regDefItem<int>("c","cellRange",2,0,0);
  IParam.regItem("C","ECut");
  IParam.regDefItem<double>("cutWeight","cutWeight",2,0.5,0.25);
  IParam.regMulti("cutTime","cutTime",100,1);
  IParam.regFlag("fullOR","fullOR");
  IParam.regItem("geomPrecision","geomPrecision");
  IParam.regItem("mode","mode");
  IParam.regMulti("MagStep","MagStep",1000,0);
  IParam.regMulti("MagUnit","MagUnit",1000,0);
  IParam.regMulti("MagField","MagField",1000,0);
  IParam.regItem("defMagnet","defMagnet");
  IParam.regMulti("userWeight","userWeight",1000,0);
  IParam.regMulti("comment","comment",1000,0);
  IParam.regFlag("cinder","cinder");
  IParam.regItem("cellDNF","cellDNF");
  IParam.regItem("cellCNF","cellCNF");
  IParam.regItem("d","debug");
  IParam.regItem("dbcn","dbcn");
  IParam.regItem("defaultConfig","defaultConfig");
  IParam.regDefItem<std::string>("dc","doseCalc",1,"InternalDOSE");
  IParam.regFlag("e","endf");
  IParam.regMulti("eng","engineering",10000,0);
  IParam.regItem("E","exclude");
  IParam.regDefItem<double>("electron","electron",1,-1.0);
  IParam.regItem("event","EVENT");
  IParam.regFlag("help","help");
  IParam.regMulti("i","iterate",10000,1);
  IParam.regItem("I","isolate");
  IParam.regDefItemList<std::string>("imp","importance",10,RItems);
  IParam.regDefItem<int>("m","multi",1,1);
  IParam.regDefItem<std::string>("matDB","materialDatabase",1,
                                 std::string("shielding"));  
  IParam.regItem("matFile","matFile");
  IParam.regItem("maxEnergy","maxEnergy");   // default max energy
  IParam.regFlag("M","mesh");
  IParam.regItem("MA","meshA");
  IParam.regItem("MB","meshB");
  IParam.regItem("MN","meshNPS",3,3);
  IParam.regFlag("md5","md5");
  IParam.regItem("md5Mesh","md5Mesh");
  IParam.regItem("memStack","memStack");
  IParam.regDefItem<int>("n","nps",1,10000);
  IParam.regItem("noVariables","noVariables");
  IParam.regFlag("phits","PHITS");
  IParam.regFlag("fluka","FLUKA");
  IParam.regItem("povray","POVRAY");
  IParam.regDefItem<int>("mcnp","MCNP",1,6);
  IParam.regFlag("Monte","Monte");
  IParam.regFlag("noThermal","noThermal");
  IParam.regMulti("ObjAdd","objectAdd",1000);
  IParam.regMulti("offset","offset",10000,1,8);
  IParam.regDefItem<double>("photon","photon",1,0.001);  // 1keV
  IParam.regDefItem<double>("photonModel","photonModel",1,100.0);
  IParam.regMulti("postOffset","postOffset",10000,1,8);
  IParam.regDefItem<std::string>("print","printTable",1,
				 "10 20 40 50 110 120");  
  IParam.regItem("PTRAC","ptrac");

  IParam.regItem("r","renum");
  IParam.regMulti("report","report",1000,0);
  IParam.regDefItem<std::string>("physModel","physicsModel",1,"CEM03"); 

  IParam.regFlag("sdefVoid","sdefVoid");
  IParam.regMulti("sdefType","sdefType",10,0);
  IParam.regItem("sdefFile","sdefFile");
  IParam.regMulti("sdefSource","sdefSource",1000,0);
  IParam.regMulti("sdefSourceName","sdefSourceName",1000,0);
  IParam.regMulti("sdefMod","sdefMod",1000,0);
  IParam.regMulti("sdefObj","sdefObj",1000,0);
  IParam.regMulti("sdefVec","sdefVec",1000,0);
  IParam.regItem("singleItem","singleItem");  
  IParam.regDefItem<long int>("s","random",1,375642321L);
  // std::vector<std::string> AItems(15);
  // IParam.regDefItemList<std::string>("T","tally",15,AItems);
  IParam.regMulti("T","tally",1000,0);
  IParam.regMulti("OAdd","objectAddition",1000);
  IParam.regMulti("TC","tallyCells",10000,2,3);
  IParam.regMulti("TGrid","TGrid",10000,2,3);
  IParam.regMulti("TMod","tallyMod",1000,1);
  IParam.regFlag("TW","tallyWeight");
  IParam.regItem("TX","Txml",1);
  IParam.regItem("targetType","targetType",1);
  IParam.regDefItem<int>("u","units",1,0);
  IParam.regItem("validCheck","validCheck",1);
  IParam.regItem("validAll","validAll",0);
  IParam.regItem("validFC","validFC",1);
  IParam.regMulti("validLine","validLine",1000);
  IParam.regItem("validPoint","validPoint",1);
  IParam.regFlag("um","voidUnMask");
  IParam.regMulti("volume","volume",4,1);
  IParam.regItem("volCard","volCard");
  IParam.regDefItem<int>("VN","volNum",1,20000);
  IParam.regMulti("volCell","volCells",100,1,100);
    
  IParam.regFlag("void","void");
  IParam.regMulti("voidObject","voidObject",1000);
  IParam.regItem("vtkMesh","vtkMesh",1);
  IParam.regItem("vtk","vtk",0);
  IParam.regItem("vtkType","vtkType",1);
  std::vector<std::string> VItems(15,"");
  IParam.regDefItemList<std::string>("vmat","vmat",15,VItems);

  IParam.regItem("w","weight");
  IParam.regItem("WP","weightPt");
  IParam.regMulti("wExt","wExt",25,0);
  IParam.regMulti("wEXP","wEXP",25,0);
  IParam.regMulti("wECut","wECut",100,0);
  IParam.regMulti("wPWT","wPWT",25,0);
  IParam.regItem("WControl","weightControl",1,10);
  IParam.regItem("WTemp","weightTemp",1);
  IParam.regItem("WEType","weightEnergyType",1,30);
  IParam.regItem("WParticle","weightParticles",1,30);
  IParam.regMulti("WSource","weightSource",30,1);
  IParam.regMulti("WPlane","weightPlane",30,2);
  IParam.regMulti("WTally","weightTally",30,1);
  IParam.regMulti("WObject","weightObject",100,1);
  IParam.regMulti("WRebase","weightRebase",100,1);
  IParam.regMulti("wDXT","weightDxtran",100,1);
  IParam.regMulti("wDD","weightDD",100,1);


  IParam.regMulti("wBIAS","wBIAS",1000,0);
  IParam.regMulti("wFCL","wFCL",1000,0);
  IParam.regMulti("wWWG","wWWG",1000,0);
  IParam.regMulti("wIMP","wIMP",1000,0);
  IParam.regMulti("wEMF","wEMF",1000,0);
  IParam.regMulti("wLAM","wLAM",1000,0);
  IParam.regMulti("wCUT","wCUT",1000,0);
  IParam.regMulti("wMAT","wMAT",1000,0);
  
  IParam.regMulti("wwgE","wwgE",25,0);
  IParam.regMulti("wwgVTK","wwgVTK",30);
  IParam.regMulti("wwgActive","wwgActive",100,1);
  IParam.regMulti("wwgCADIS","wwgCADIS",30,1);
  IParam.regMulti("wwgNorm","wwgNorm",30,0);
  IParam.regMulti("wwgCreate","wwgCreate",100,1);
  IParam.regMulti("wwgEnergy","wwgEnergy",25,0);
  IParam.regMulti("wwgMarkov","wwgMarkov",100,1);
  IParam.regMulti("wwgRPtMesh","wwgRPtMesh",100,0);
  IParam.regMulti("wwgMesh","wwgMesh",25,0);
  IParam.regMulti("wwgSource","wwgSource",30,1);
  IParam.regMulti("wwgPlane","wwgPlane",30,1);

  
  IParam.regDefItem<std::string>("X","xmlout",1,"Model.xml");
  IParam.regMulti("x","xml",10000,1);

  // PHITS Options -- to be updated
  IParam.regMulti("energyCut","energyCut",1000);
  IParam.setDesc("energyCut","PHITS: { particle minEnergy }");

  IParam.regItem("eRange","eRange",1,2);
  IParam.setDesc("eRange",
		 "PHITS: { charged particle track energy limits [low:high] }");

  IParam.regItem("eTrack","eTrack",1,2);
  IParam.setDesc("eTrack",
		 "PHITS: { electron track energy limits [low:high] }");
    
  IParam.setDesc("angle","Orientate to component [name]");
  IParam.setDesc("axis","Rotate to main axis rotation [TS2]");
  IParam.setDesc("basicGeom","Use basic fluka geometry system");
  IParam.setDesc("geomPrecision","Set geometry accuracy  : 1e-6cm * value");
  IParam.setDesc("c","Cells to protect");
  IParam.setDesc("cutWeight","Set the cut weights (wc1/wc2)" );
  IParam.setDesc("ECut","Cut energy");
  IParam.setDesc("fullOR","Write out full X/Y/Z basis in objectRegister.txt");
  IParam.setDesc("cinder","Outer Cinder files");
  IParam.setDesc("d","debug flag");
  IParam.setDesc("dbcn","DBCN card values");
  IParam.setDesc("dc","Dose flag (internalDOSE/DOSE)");
  IParam.setDesc("defaultConfig","Set up a default configuration");
  IParam.setDesc("e","Convert materials to ENDF-VII");
  IParam.setDesc("electron","Add electron physics at Energy");
  IParam.setDesc("engineering","Select engineering detail {components}");
  IParam.setDesc("E","exclude part of the simualtion [e.g. chipir/zoom]");
  IParam.setDesc("event","Event processing : ");
  IParam.setDesc("help","Help on the diff options for building [only TS1] ");
  IParam.setDesc("i","iterate on variables");
  IParam.setDesc("I","Isolate component");
  IParam.setDesc("imp","Importance regions");
  IParam.setDesc("m","Create multiple files (diff: RNDseed)");
  IParam.setDesc("matDB","Set the material database to use "
                 "(shielding or neutronics)");  
  IParam.setDesc("matFile","Set the materials from a file");

  IParam.setDesc("M","Add mesh tally");
  IParam.setDesc("MA","Lower Point in mesh tally");
  IParam.setDesc("MB","Upper Point in mesh tally");
  IParam.setDesc("MN","Number of points [3]");
  IParam.setDesc("md5","MD5 track of cells");
  IParam.setDesc("md5Mesh","Define mesh for MD5/VTK");
  IParam.setDesc("memStack","Memstack verbrosity value");
  IParam.setDesc("n","Number of starting particles");
  IParam.setDesc("noVariables","NO variables to written to file");
  IParam.setDesc("MCNP","MCNP version");
  IParam.setDesc("FLUKA","FLUKA output");
  IParam.setDesc("POVRAY","PovRay output");
  IParam.setDesc("PHITS","PHITS output");
  IParam.setDesc("Monte","MonteCarlo capable simulation");
  IParam.setDesc("MagStep","Set the min/max step size for magnetics");
  IParam.setDesc("MagUnit","Add a magnetic type");
  IParam.setDesc("MagField","Set the various units");
  IParam.setDesc("defMagnet","Do not use default magnet variables");
  IParam.setDesc("noThermal","No thermal cross-section in materials def");
  IParam.setDesc("offset","Displace to component [name]");
  IParam.setDesc("ObjAdd","Add a component (cell)");
  IParam.setDesc("photon","Photon Cut energy");
  IParam.setDesc("photonModel","Photon Model Energy [min]");
  IParam.setDesc("r","Renubmer cells");
  IParam.setDesc("report","Report a position/axis (show info on points etc)");
  IParam.setDesc("s","RND Seed");
  IParam.setDesc("sdefFile","File(s) for source");
  IParam.setDesc("sdefObj","Source Initialization Object");
  IParam.setDesc("sdefType","Source Type (TS1/TS2)");
  IParam.setDesc("sdefVoid","Remove sdef card [to use source.F]");
  IParam.setDesc("sdefSource","Extra data for source.F");
  IParam.setDesc("singleItem","Single item for singleItem method");
  
  IParam.setDesc("physModel","Physics Model"); 
  IParam.setDesc("T","Tally type [set to -1 to see all help]");
  IParam.setDesc("TC","Tally cells for a f4 cinder tally");
  //  IParam.setDesc("TNum","Tally ");
  IParam.setDesc("userWeight","Set userWeight flag for FLUKA: [value]");
  IParam.setDesc("TMod","Modify tally [help for description]");
  IParam.setDesc("OAdd","Add a component (cell)");
  IParam.setDesc("TGrid","Set a grid on a point tally [tallyN NXpts NZPts]");
  IParam.setDesc("TW","Activate tally pd weight system");
  IParam.setDesc("Txml","Tally xml file");
  IParam.setDesc("targetType","Name of target type");
  IParam.setDesc("u","Units in cm");
  IParam.setDesc("um","Unset spherical void area (from imp=0)");
  IParam.setDesc("void","Adds the void card to the simulation");
  IParam.setDesc("voidObject","Sets material of FC-object by name");
  IParam.setDesc("volume","Create volume about point/radius for f4 tally");
  IParam.setDesc("volCells","Cells [object/range]");
  IParam.setDesc("volCard","set/delete the vol card");
  IParam.setDesc("vtk","Write out VTK plot mesh");
  IParam.setDesc("vtkMesh","Define mesh for MD5/VTK");
  IParam.setDesc("vmat","Material sections to be written by vtk output");
  IParam.setDesc("VN","Number of points in the volume integration");
  IParam.setDesc("validCheck","Run simulation to check for validity");
  IParam.setDesc("validPoint","Point to start valid check from");

  IParam.setDesc("w","weightBias");
  IParam.setDesc("wExt","Extraction biasisng [see: -wExt help]");
  IParam.setDesc("wDXT","Dxtran sphere addition [set -wDXT help] ");
  IParam.setDesc("wDD","Dxtran Diagnostic [set -wDXT help] ");
  IParam.setDesc("wWWG","Weight WindowGenerator Mesh  ");
  IParam.setDesc("wwgCADIS","Single step evolve for the calculate for WWG/WWCell  ");
  IParam.setDesc("wwgMarkov","Evolve the calculate for WWG/WWCell  ");
  IParam.setDesc("wIMP","set imp: particle impValue object(s)  ");
  IParam.setDesc("wFCL","Forced Collision ");
  IParam.setDesc("wPWT","Photon Bias [set -wPWT help]");
  IParam.setDesc("WEType","Initial model for weights [help for info]");
  IParam.setDesc("WTemp","Temperature correction for weights");
  IParam.setDesc("WRebase","Rebase the weights based on a cell");
  IParam.setDesc("WObject","Reconstruct weights base on cells");
  IParam.setDesc("WP","Weight bias Point");
  IParam.setDesc("weightControl","Sets: energyCut scaleFactor minWeight");
  IParam.setDesc("wwgNorm"," normalization step : "
		 "[weightRange - lowRange - highRange - powerRange]");
  
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
  ELog::RegMethod RegA("MainInputs[F]","createSiliconInputs");

  IParam.regItem("b","bias");
  IParam.regDefItem<double>("d","detSize",1,50.0);
  IParam.regDefItem<double>("l","lambda",1,1.0);
  IParam.regDefItem<std::string>("m","material",1,"Poly");
  IParam.regDefItem<std::string>("si","silicon",1,"Silicon");
  IParam.regDefItem<int>("n","nps",1,20);
  IParam.regItem("o","output");

  IParam.setDesc("bias","");
  IParam.setDesc("detSize","Width/Height of the area-detector");
  IParam.setDesc("lambda","Wavelength in angstrom");
  IParam.setDesc("material","Moderator material");  
  IParam.setDesc("silicon","Silicon vane material");
  IParam.setDesc("nps","Number of simulated points");
  IParam.setDesc("output","Output file");

  return;
}

void 
createSinbadInputs(inputParam& IParam)
  /*!
    Set the specialise inputs for sinbad
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainInputs[F]","createSinbadInputs");

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
  ELog::RegMethod RegA("MainInputs[F]","createDelftInputs");

  createInputs(IParam);

  IParam.regItem("coreType","coreType",1);
  IParam.regDefItem<std::string>("modType","modType",1,"Sphere");
  IParam.regDefItem<std::string>("refExtra","refExtra",1,"None");
  IParam.regDefItem<std::string>("buildType","buildType",1,"Full");

  IParam.regMulti("kcode","kcode",1000);
  IParam.regMulti("ksrcMat","ksrcMat",1000);
  IParam.regMulti("ksrcVec","ksrcVec",1000);

  IParam.regItem("FuelXML","FuelXML",1);
  IParam.regItem("fuelXML","fuelXML",1);

  IParam.setDesc("coreType","Selection of Delft cores");
  IParam.setDesc("kcode","MatN nsourcePart keff skip realRuns");
  IParam.setDesc("ksrcMat","Acceptable material number for ksrc");
  IParam.setDesc("ksrcVec","Positions for ksrc [after matN check]");
  IParam.setDesc("buildType","Single/Full -- build moderator only");
  IParam.setDesc("modType","Type of moderator (sphere/tunnel)");
  IParam.setDesc("refExtra","Type of extra Be around moderators");
  IParam.setDesc("FuelXML","Write Fuel config to XMLfile");
  IParam.setDesc("fuelXML","Read the Fuel config from an XMLfile");

  IParam.setValue("sdefType",std::string("kcode"));
  IParam.setValue("sdefType",std::string("kcode"),1);
  IParam.setValue("sdefType",std::string("kcode"),2);
  IParam.setValue("sdefType",std::string("kcode"),3);
  
  return;
}

void createFullInputs(inputParam& IParam)
  /*!
    Set the specialise inputs for TS2
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainInputs[F]","createFullInputs");
  createInputs(IParam);

  IParam.regDefItem<std::string>("b","beamstop",1,"");
  IParam.regFlag("bolts","bolts");
  IParam.regDefItem<int>("cf","collFlag",1,7);
  IParam.regItem("decFile","decFile",1,1);
  IParam.regDefItem<std::string>("decType","decType",1,"standard");
  IParam.regFlag("h","horr");  
  IParam.regFlag("orthoH","orthoH");  
  IParam.regMulti("t","target",5);
  IParam.regDefItem<int>("zoomShutterGN","zoomShutterGN",1,4);

  IParam.setDesc("b","Add beamstop");
  IParam.setDesc("bolts","Add reflector bolts");
  IParam.setDesc("cf","Collimator Flag");
  IParam.setDesc("decFile","Surface File to replace CH4 in decoupled");
  IParam.setDesc("h","horrizontal for chipIR");
  IParam.setDesc("orthoH","Add ortho/para layer to the Coupled H2");
  IParam.setDesc("target","Use flat target");
  IParam.setDesc("zoomShutterGN","Number of sections between B4C "
		 "in zoom shutter");

  IParam.setValue("targetType",std::string("t2Target"));  
  IParam.setValue("sdefType",std::string("TS2"));  
  return;
}

void
createFilterInputs(inputParam& IParam)
  /*!
    Create Photon-neutron source input
    \param IParam :: Initial input
  */
{
  ELog::RegMethod RegA("MainInputs[F]","createFilterInputs");
  createInputs(IParam);
  

  IParam.setValue("sdefType",std::string("Beam"));
  IParam.setFlag("voidUnMask");  
  return;
}

void
createGammaInputs(inputParam& IParam)
  /*!
    Create Gamma expt model
    \param IParam :: Initial input
  */
{
  ELog::RegMethod RegA("MainInputs[F]","createGammaInputs");
  createInputs(IParam);

  IParam.setValue("sdefType",std::string("Gamma"));  
  return;
}

void
createPhotonInputs(inputParam& IParam)
  /*!
    Create Photon-neutron source input
    \param IParam :: Initial input
  */
{
  ELog::RegMethod RegA("MainInputs[F]","createPhotonInputs");
  createInputs(IParam);

  IParam.setValue("sdefType",std::string("Laser"));
  IParam.setFlag("voidUnMask");  
  return;
}

void
createTS1Inputs(inputParam& IParam)
  /*!
    Set the specialise inputs for TS1
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainInputs[F]","createTS1Inputs");
  createInputs(IParam);

  IParam.regDefItem<std::string>("CH4PreType","CH4PreType",1,"Wrapper");  
  IParam.setDesc("CH4PreType","Name of CH4 Premoderator");
  IParam.regDefItem<std::string>("CH4ModType","CH4ModType",1,"Basic");  
  IParam.setDesc("CH4ModType","Name of CH4 Moderator");
  IParam.regDefItem<std::string>("WaterModType","WaterModType",1,"Triangle");  
  IParam.setDesc("WaterModType","Name of Water [top] Moderator");
  IParam.regDefItem<std::string>("H2ModType","H2ModType",1,"Basic");  
  IParam.setDesc("H2ModType","Name of H2 Moderator");

  IParam.regFlag("BeRods","BeRods");  
  IParam.setDesc("BeRods","Divide the Be into rods");

  IParam.setValue("sdefType",std::string("TS1"));  
  IParam.setValue("targetType",std::string("t1PlateTarget"));  

  return;
}

void
createBilbauInputs(inputParam& IParam)
  /*!
    Set the specialise inputs for TS2
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainInputs[F]","createBilbauInputs");
  createInputs(IParam);

  //  IParam.setValue("sdefEnergy",50.0);    
  IParam.setValue("sdefType",std::string("TS1"));  
  

  return;
}

void
createBNCTInputs(inputParam& IParam)
  /*!
    Set the specialise inputs for Boron capture beamline
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainInputs[F]","createSNSInputs");
  createInputs(IParam);
  
  IParam.setValue("sdefType",std::string("ess"));  
  return;
}

void
createD4CInputs(inputParam& IParam)
  /*!
    Set the specialise inputs for D4C beamline
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainInputs[F]","createD4CInputs");

  createInputs(IParam);

  IParam.regDefItem<std::string>("df","detFile",1,"test");
  IParam.setDesc("detFile","Head name of output file");
  IParam.regDefItem<int>("MS","multiScat",1,0);
  IParam.setDesc("multiScat","Consider only 1 collision ");

  IParam.setValue("sdefType",std::string("D4C"));  
  //  IParam.setFlag("Monte");
  return;
}

void
createTS3ExptInputs(inputParam& IParam)
  /*!
    Set the specialise inputs for TS3 Model
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainInputs[F]","createD4CInputs");
  createInputs(IParam);

  IParam.setValue("sdefType",std::string("TS3Expt"));
  return;
}

void
createCuInputs(inputParam& IParam)
  /*!
    Set the specialise inputs for TS2
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainInputs[F]","createCutInputs");
  createInputs(IParam);
  
  IParam.setValue("sdefType",std::string("TS1"));    
  return;
}

void
createPipeInputs(inputParam& IParam)
  /*!
    Set the specialise inputs for TS2
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainInputs[F]","createPipeInputs");
  createInputs(IParam);
  
  IParam.setValue("sdefType",std::string("Point"));    
  return;
}

void
createLinacInputs(inputParam& IParam)
  /*!
    Set the specialise inputs for the ESS-Linac
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainInputs[F]","createLinacInputs");
  createInputs(IParam);

  IParam.setValue("sdefType",std::string("essLinac"));
  
  return;
}
  

void
createSingleItemInputs(inputParam& IParam)
  /*!
    Set the specialise inputs for single test item
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainInputs[F]","createSingleItemInputs");
  createInputs(IParam);
  return;
}

  
void
createEPBInputs(inputParam& IParam)
  /*!
    Set the specialise inputs for ESS EPB magnets
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainInputs[F]","createEPBInputs");
  createInputs(IParam);
  
  IParam.setValue("sdefType",std::string("ess"));  
  return;
}

void
createSNSInputs(inputParam& IParam)
  /*!
    Set the specialise inputs for SNS
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainInputs[F]","createSNSInputs");
  createInputs(IParam);
  
  IParam.setValue("sdefType",std::string("ess"));  
  return;
}

void
createMuonInputs(inputParam& IParam)
  /*!
    Set the specialise inputs for EPB on TS2
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainInputs[F]","createMuonInputs");
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
  ELog::RegMethod RegA("MainInputs[F]","createLensInputs");
  
  createInputs(IParam);
  IParam.regMulti("TS","surfTally",1000,2,2);
  IParam.regMulti("TE","tallyEnergy",1000,1,1);
  IParam.regMulti("TT","tallyTime",10000,1,1);

  IParam.setDesc("TE","Tally Energy: Energy string");
  IParam.setDesc("TT","Tally Time: Time string");
  IParam.setDesc("TS","Surface Tally: FL, dist [paired with TE/TT]");
  return;
}

} // NAMESPACE mainSystem
