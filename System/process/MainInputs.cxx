/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/MainInputs.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "InputControl.h"
#include "inputParam.h"
#include "MainInputs.h"

namespace mainSystem
{

void
createInputs(inputParam& IParam)
  /*!
    Sets all the input paramter flags
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("MainProcess[F]","createInputs");

  std::vector<std::string> RItems(10,"");
  
  IParam.regMulti("activation","activation",10000,1);

  IParam.regFlag("a","axis");
  IParam.regMulti("angle","angle",10000,1,8);
  IParam.regDefItem<int>("c","cellRange",2,0,0);
  IParam.regItem("C","ECut");
  IParam.regDefItem<double>("cutWeight","cutWeight",2,0.5,0.25);
  IParam.regMulti("cutTime","cutTime",100,1);
  IParam.regItem("mode","mode");
  IParam.regFlag("cinder","cinder");
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
  IParam.regFlag("M","mesh");
  IParam.regItem("MA","meshA");
  IParam.regItem("MB","meshB");
  IParam.regItem("MN","meshNPS",3,3);
  IParam.regFlag("md5","md5");
  IParam.regItem("memStack","memStack");
  IParam.regDefItem<int>("n","nps",1,10000);
  IParam.regFlag("p","PHITS");
  IParam.regFlag("fluka","FLUKA");
  IParam.regItem("povray","PovRay");
  IParam.regDefItem<int>("mcnp","MCNP",1,6);
  IParam.regFlag("Monte","Monte");
  IParam.regMulti("ObjAdd","objectAdd",1000);
  IParam.regMulti("offset","offset",10000,1,8);
  IParam.regDefItem<double>("photon","photon",1,0.001);  // 1keV
  IParam.regDefItem<double>("photonModel","photonModel",1,100.0);
  IParam.regDefItem<std::string>("print","printTable",1,
				 "10 20 40 50 110 120");  
  IParam.regItem("PTRAC","ptrac");
  IParam.regDefItemList<std::string>("r","renum",10,RItems);
  IParam.regMulti("R","report",1000,0);
  IParam.regFlag("sdefVoid","sdefVoid");
  IParam.regDefItem<std::string>("sdefType","sdefType",1,"");
  IParam.regDefItem<std::string>("physModel","physicsModel",1,"CEM03"); 
  IParam.regDefItem<double>("SA","sdefAngle",1,35.0);
  IParam.regItem("sdefFile","sdefFile");
  IParam.regDefItem<int>("SI","sdefIndex",1,1);

  std::vector<std::string> SItems(3,"");
  IParam.regDefItemList<std::string>("SObj","sdefObj",3,SItems);
  
  IParam.regItem("SP","sdefPos");
  IParam.regItem("SR","sdefRadius");
  IParam.regItem("SV","sdefVec");
  IParam.regItem("SZ","sdefZRot");
  IParam.regDefItem<long int>("s","random",1,375642321L);
  // std::vector<std::string> AItems(15);
  // IParam.regDefItemList<std::string>("T","tally",15,AItems);
  IParam.regMulti("T","tally",1000,0);
  IParam.regMulti("TAdd","tallyAdd",1000);
  IParam.regMulti("TC","tallyCells",10000,2,3);
  IParam.regMulti("TGrid","TGrid",10000,2,3);
  IParam.regMulti("TMod","tallyMod",8,1);
  IParam.regFlag("TW","tallyWeight");
  IParam.regItem("TX","Txml",1);
  IParam.regItem("targetType","targetType",1);
  IParam.regDefItem<int>("u","units",1,0);
  IParam.regItem("validCheck","validCheck",1);
  IParam.regMulti("validLine","validLine",1000);
  IParam.regItem("validPoint","validPoint",1);
  IParam.regFlag("um","voidUnMask");
  IParam.regMulti("volume","volume",4,1);
  IParam.regItem("volCard","volCard");
  IParam.regDefItem<int>("VN","volNum",1,20000);
  IParam.regMulti("volCell","volCells",100,1,100);
    
  IParam.regFlag("void","void");
  IParam.regFlag("vtk","vtk");
  IParam.regFlag("vcell","vcell");
  std::vector<std::string> VItems(15,"");
  IParam.regDefItemList<std::string>("vmat","vmat",15,VItems);

  IParam.regItem("w","weight");
  IParam.regItem("WP","weightPt");
  IParam.regMulti("wExt","wExt",25,0);
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


  IParam.regMulti("wFCL","wFCL",25,0);
  IParam.regMulti("wWWG","wWWG",25,0);
  IParam.regMulti("wIMP","wIMP",25,0);
    
  IParam.regMulti("wwgE","wwgE",25,0);
  IParam.regItem("wwgVTK","wwgVTK",1,10);
  IParam.regItem("wwgNorm","wwgNorm",0,30);
  IParam.regMulti("wwgCalc","wwgCalc",100,1);
  IParam.regItem("wwgCADIS","wwgCADIS",0,30);
  IParam.regMulti("wwgMarkov","wwgMarkov",100,1);
  IParam.regItem("wwgRPtMesh","wwgRPtMesh",1,125);
  IParam.regItem("wwgXMesh","wwgXMesh",3,125);
  IParam.regItem("wwgYMesh","wwgYMesh",3,125);
  IParam.regItem("wwgZMesh","wwgZMesh",3,125);  
  
  IParam.regDefItem<std::string>("X","xmlout",1,"Model.xml");
  IParam.regMulti("x","xml",10000,1);

  IParam.setDesc("angle","Orientate to component [name]");
  IParam.setDesc("axis","Rotate to main axis rotation [TS2]");
  IParam.setDesc("c","Cells to protect");
  IParam.setDesc("cutWeight","Set the cut weights (wc1/wc2)" );
  IParam.setDesc("ECut","Cut energy");
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
  IParam.setDesc("memStack","Memstack verbrosity value");
  IParam.setDesc("n","Number of starting particles");
  IParam.setDesc("MCNP","MCNP version");
  IParam.setDesc("FLUKA","FLUKA output");
  IParam.setDesc("PovRay","PovRay output");
  IParam.setDesc("PHITS","PHITS output");
  IParam.setDesc("Monte","MonteCarlo capable simulation");
  IParam.setDesc("offset","Displace to component [name]");
  IParam.setDesc("ObjAdd","Add a component (cell)");
  IParam.setDesc("photon","Photon Cut energy");
  IParam.setDesc("photonModel","Photon Model Energy [min]");
  IParam.setDesc("r","Renubmer cells");
  IParam.setDesc("report","Report a position/axis (show info on points etc)");
  IParam.setDesc("s","RND Seed");
  IParam.setDesc("sdefFile","File(s) for source");
  IParam.setDesc("SA","Source Angle [deg]");
  IParam.setDesc("SI","Source Index value [1:2]");
  IParam.setDesc("SObj","Source Initialization Object");
  IParam.setDesc("sdefType","Source Type (TS1/TS2)");
  IParam.setDesc("sdefVoid","Remove sdef card [to use source.F]");
  IParam.setDesc("physModel","Physics Model"); 
  IParam.setDesc("SP","Source start point");
  IParam.setDesc("SV","Sourece direction vector");
  IParam.setDesc("SZ","Source direction: Rotation to +ve Z [deg]");
  IParam.setDesc("T","Tally type [set to -1 to see all help]");
  IParam.setDesc("TC","Tally cells for a f4 cinder tally");
  //  IParam.setDesc("TNum","Tally ");
  IParam.setDesc("TMod","Modify tally [help for description]");
  IParam.setDesc("TAdd","Add a component (cell)");
  IParam.setDesc("TGrid","Set a grid on a point tally [tallyN NXpts NZPts]");
  IParam.setDesc("TW","Activate tally pd weight system");
  IParam.setDesc("Txml","Tally xml file");
  IParam.setDesc("targetType","Name of target type");
  IParam.setDesc("u","Units in cm");
  IParam.setDesc("um","Unset spherical void area (from imp=0)");
  IParam.setDesc("void","Adds the void card to the simulation");
  IParam.setDesc("volume","Create volume about point/radius for f4 tally");
  IParam.setDesc("volCells","Cells [object/range]");
  IParam.setDesc("volCard","set/delete the vol card");
  IParam.setDesc("vtk","Write out VTK plot mesh");
  IParam.setDesc("vcell","Use cell id rather than material");
  IParam.setDesc("vmat","Material sections to be written by vtk output");
  IParam.setDesc("VN","Number of points in the volume integration");
  IParam.setDesc("validCheck","Run simulation to check for validity");
  IParam.setDesc("validPoint","Point to start valid check from");

  IParam.setDesc("w","weightBias");
  IParam.setDesc("wExt","Extraction biasisng [see: -wExt help]");
  IParam.setDesc("wDXT","Dxtran sphere addition [set -wDXT help] ");
  IParam.setDesc("wDD","Dxtran Diagnostic [set -wDXT help] ");
  IParam.setDesc("wWWG","Weight WindowGenerator Mesh  ");
  IParam.setDesc("wwgCalc","Single step evolve for the calculate for WWG/WWCell  ");
  IParam.setDesc("wwgMarkov","Evolve the calculate for WWG/WWCell  ");
  IParam.setDesc("wIMP","set imp partile imp object(s)  ");
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
  ELog::RegMethod RegA("MainProcess[F]","createSiliconInputs");

  IParam.regItem("b","bias");
  IParam.regDefItem<double>("d","detSize",1,50.0);
  IParam.regDefItem<double>("l","lambda",1,1.0);
  IParam.regDefItem<std::string>("m","material",1,"Poly");
  IParam.regDefItem<std::string>("si","silicon",1,"Silicon");
  IParam.regDefItem<int>("n","nps",1,20);
  IParam.regItem("o","output");
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
  ELog::RegMethod RegA("MainProcess::","createFilterInputs");
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
  ELog::RegMethod RegA("MainProcess::","createGammaInputs");
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
  ELog::RegMethod RegA("MainProcess::","createPhotonInputs");
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
  ELog::RegMethod RegA("MainProcess::","createTS1Inputs");
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
  ELog::RegMethod RegA("MainProcess::","createBilbauInputs");
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
  ELog::RegMethod RegA("MainProcess::","createSNSInputs");
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
  ELog::RegMethod RegA("MainProcess::","createD4CInputs");
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
  ELog::RegMethod RegA("MainProcess::","createD4CInputs");
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
  ELog::RegMethod RegA("MainProcess::","createCutInputs");
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
  ELog::RegMethod RegA("MainProcess::","createPipeInputs");
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
  ELog::RegMethod RegA("MainProcess::","createLinacInputs");
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
  ELog::RegMethod RegA("MainProcess::","createSingleItemInputs");
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
  ELog::RegMethod RegA("MainProcess::","createEPBInputs");
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
  ELog::RegMethod RegA("MainProcess::","createSNSInputs");
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
  IParam.regMulti("TS","surfTally",1000,2,2);
  IParam.regMulti("TE","tallyEnergy",1000,1,1);
  IParam.regMulti("TT","tallyTime",10000,1,1);

  IParam.setDesc("TE","Tally Energy: Energy string");
  IParam.setDesc("TT","Tally Time: Time string");
  IParam.setDesc("TS","Surface Tally: FL, dist [paired with TE/TT]");
  return;
}

} // NAMESPACE mainSystem
