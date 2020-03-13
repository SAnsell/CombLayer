/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/flukaImpConstructor.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include <cmath>
#include <complex> 
#include <vector>
#include <list>
#include <set>
#include <map> 
#include <string>
#include <algorithm>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "support.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimFLUKA.h"
#include "objectRegister.h"
#include "inputParam.h"
#include "cellValueSet.h"
#include "pairValueSet.h"
#include "flukaGenParticle.h"
#include "flukaPhysics.h"
#include "flukaProcess.h"
#include "flukaImpConstructor.h"

namespace flukaSystem
{


void
flukaImpConstructor::insertPair(flukaPhysics& PC,
				const size_t cellSize,
				const std::string& pName,
				const int cellName,
				const std::string& keyName,
				const std::vector<std::string>& VV) const
 /*!
   Process the actual insert 
   \param PC :: Physics card to insert into 
   \param cellSize :: Size of additional parameters
   \param pName :: particle name
   \param cellName :: cell name/material name to apply to
   \param keyName :: component keyname
   \param VV :: variables
 */
{
  ELog::RegMethod RegA("flukaImpConstructor","insertPair");

  switch (cellSize)
    {
    case 0:
      //      PC.setFlag(keyName,pName);
      break;
    case 1:
      PC.setLAMPair(keyName,pName,cellName,VV[1],"");
      break;
    case 2:
      PC.setLAMPair(keyName,pName,cellName,VV[1],VV[2]);
      break;
    case 3:
      //      PC.setTHR(keyName,pName,VV[1],VV[2],VV[3]);
      break;
    }
  return;
}

void
flukaImpConstructor::insertParticle(flukaPhysics& PC,
				    const size_t cellSize,
				    const std::string& pName,
				    const std::string& keyName,
				    const std::vector<std::string>& VV) const
 /*!
   Process the actual insert 
   \param PC :: Physics to insert to
   \param cellSize :: cell Unit size
   \param pName :: particle name
   \param keyName :: component keyname
   \param VV :: variables
 */
{
  ELog::RegMethod RegA("flukaImpConstructor","insertParticle");

  if (VV.size()<cellSize)
    ELog::EM<<"IN PARTICAL"<<VV.size()<<" "<<cellSize<<ELog::endErr;
  switch (cellSize)
    {
    case 0:
      PC.setFlag(keyName,pName);
      break;
    case 1:
      PC.setIMP(keyName,pName,VV[1]);
      break;
    case 2:
      PC.setEMF(keyName,pName,VV[1],VV[2]);
      break;
    case 3:
      PC.setTHR(keyName,pName,VV[1],VV[2],VV[3]);
      break;
    }
  return;
}

void
flukaImpConstructor::insertCell(flukaPhysics& PC,
				const size_t cellSize,
				const std::set<int>& activeCell,
				const std::string& keyName,
				const std::vector<std::string>& VV) const
 /*!
   Process the actual insert of cells/materials along with
   values into the phyics object
   \param PC :: Physcis to insert to
   \param cellSize :: cell Unit size
   \param activeCell :: Cells this applies to
   \param keyName :: component keyname
   \param VV :: variables
 */
{
  ELog::RegMethod RegA("flukaImpConstructor","insertCell");

  switch (cellSize)
    {
    case 0:
      for(const int MN : activeCell)
	PC.setFlag(keyName,MN);
      break;
    case 1:
      for(const int MN : activeCell)
	PC.setIMP(keyName,MN,VV[1]);
      break;
    case 2:
      for(const int MN : activeCell)
	PC.setEMF(keyName,MN,VV[1],VV[2]);
      break;
    case 3:
      for(const int MN : activeCell)
	PC.setTHR(keyName,MN,VV[1],VV[2],VV[3]);
      break;
    }
  return;
}				    

void
flukaImpConstructor::processGeneral(SimFLUKA& System,
				    const mainSystem::inputParam& IParam,
				    const size_t setIndex,
				    const std::string& keyName,
				    const impTYPE& cutTuple) const
  /*!
    Handler for constructor of physics cards
    \param System :: Fluke Phays
    \param IParam :: input stream
    \param setIndex :: index value for item
    \param keyName :: wTYPE card
    \param cutTuple :: system for cellStr construction
  */
{
  ELog::RegMethod RegA("flukaImpConstructor","processGeneral(IParam)");

  const size_t cellSize(std::get<0>(cutTuple));
  const int materialFlag(std::get<1>(cutTuple));
  const std::string cardName(std::get<2>(cutTuple));

  std::vector<std::string> VVList(cellSize+1);
  if (materialFlag!=-100)
    {
      VVList[0]=IParam.getValueError<std::string>
	(keyName,setIndex,1,"No cell/material/particle for "+keyName);

      for(size_t i=0;i<cellSize;i++)
	VVList[i+1]=IParam.getValueError<std::string>
	  (keyName,setIndex,2+i,
	   "No value["+std::to_string(i+1)+"] for "+keyName+":"+cardName);      
    }
  
  processGeneral(System,VVList,keyName,cellSize,materialFlag,cardName);
  return;
}

void
flukaImpConstructor::processGeneral(SimFLUKA& System,
				    const std::vector<std::string>& VVList,
				    const std::string& keyName,
				    const impTYPE& cutTuple) const
  /*!
    Handler for constructor of physics cards
    \param System :: Fluke Phays
    \param VVlist :: string array
    \param keyName :: wTYPE card
    \param cutTuple :: cellSize/mat/card tuple
  */
{
  ELog::RegMethod RegA("flukaImpConstructor","processGeneral");

  const size_t cellSize(std::get<0>(cutTuple));
  const int materialFlag(std::get<1>(cutTuple));
  const std::string cardName(std::get<2>(cutTuple));

  processGeneral(System,VVList,keyName,cellSize,materialFlag,cardName);
  return;
}

  
void
flukaImpConstructor::processGeneral(SimFLUKA& System,
				    const std::vector<std::string>& VVList,
				    const std::string& keyName,
				    const size_t cellSize,
				    const int materialFlag,
				    const std::string& cardName) const
  /*!
    Handler for constructor of physics cards
    \param System :: Fluke Phays
    \param VVlist :: string array
    \param keyName :: wTYPE card
    \param cellSize :: number of extra cells 
    \param materialFlag :: material/region/particle [-1/0/1] (-100 for none)
    \param cardName :: card name for flukaPhysics to write
  */
{
  ELog::RegMethod RegA("flukaImpConstructor","processGeneral");

  flukaPhysics& PC = *System.getPhysics();
  const std::string cellM((materialFlag==-100) ?  "None" : VVList[0]);

  if (materialFlag>=0)
    {
      // gets set of cells/materials [0:cells/1 materials]
      const std::set<int> activeCell=
	getActiveUnit(System,materialFlag,cellM);
	
      if (activeCell.empty())
	throw ColErr::InContainerError<std::string>(cellM,"Empty cell:");
      insertCell(PC,cellSize,activeCell,cardName,VVList);
    }
  else if (materialFlag==-1) // particile
    {
      const flukaGenParticle& FG=flukaGenParticle::Instance();
      const std::string& pName=FG.nameToFLUKA(cellM);
      insertParticle(PC,cellSize,pName,cardName,VVList);
    }
  else  // -100 : Generic
    {
      insertParticle(PC,cellSize,"generic",cardName,VVList);
    }
  return;
}

void
flukaImpConstructor::processBIAS(SimFLUKA& System,
				 const mainSystem::inputParam& IParam,
				 const size_t setIndex)
  /*!
    Set BIAS for particles and stuff
    \param PC :: PhysicsCards
    \param IParam :: input stream
    \param setIndex :: index for the importance set
  */
{
  ELog::RegMethod RegA("flukaImpConstructor","processBIAS");

  // cell/mat : tag name 
  typedef std::tuple<size_t,int,std::string> biasTYPE;
  static const std::map<std::string,biasTYPE> IBias
    ({
      { "bias",biasTYPE(2,0,"bias") },       // region(0)
      { "user",biasTYPE(2,0,"bias-user") }   // region(0) 
    });

  const std::string type=IParam.getValueError<std::string>
    ("wBIAS",setIndex,0,"No type for wBIAS ");

  if (type=="help" || type=="Help")
    return writeBIASHelp(ELog::EM.Estream(),&ELog::endBasic);

  const std::string biasParticles=
    IParam.getValueError<std::string>("wBIAS",setIndex,1,"No type for wBIAS");
  ELog::EM<<"biasParticles: "<< biasParticles  <<ELog::endDiag;
  
  std::map<std::string,biasTYPE>::const_iterator mc=IBias.find(type);
  if (mc==IBias.end())
    throw ColErr::InContainerError<std::string>(type,"wBIAS type unknown");

  std::vector<std::string> VVList(4);
  VVList[0]=biasParticles;
  for(size_t i=0;i<3;i++)
    VVList[i+1]=IParam.getValueError<std::string>
      ("wBIAS",setIndex,2+i,
       "No value["+std::to_string(i+1)+"] for wBias");      

  processGeneral(System,VVList,"wBIAS",mc->second);
  return;
}

void
flukaImpConstructor::processCUT(SimFLUKA& System,
				const mainSystem::inputParam& IParam,
				const size_t setIndex)
  /*!
    Set individual IMP based on Iparam
    \param PC :: PhysicsCards
    \param IParam :: input stream
    \param setIndex :: index for the importance set
  */
{
  ELog::RegMethod RegA("flukaImpConstructor","processCUT");

  // cell/mat : tag name 
  typedef std::tuple<size_t,int,std::string> cutTYPE;
  static const std::map<std::string,cutTYPE> ICut
    ({
      { "partthr",cutTYPE(1,-1,"partthr") }   // particle
    });

  const std::string type=IParam.getValueError<std::string>
    ("wCUT",setIndex,0,"No type for wCUT ");

  if (type=="help" || type=="Help")
    return writeCUTHelp(ELog::EM.Estream(),&ELog::endBasic);

  std::map<std::string,cutTYPE>::const_iterator mc=ICut.find(type);
  if (mc==ICut.end())
    throw ColErr::InContainerError<std::string>(type,"imp type unknown");

  processGeneral(System,IParam,setIndex,"wCUT",mc->second);
  return;
}
 
void
flukaImpConstructor::processMAT(SimFLUKA& System,
				const mainSystem::inputParam& IParam,
				const size_t setIndex)
 /*!
    Set individual wMAT based on IParam
    This is to set mat-prop mainly for correct to density 
    so that we correctly loose the correct amount of dE/dx 
    but can enhance the bremstauhlung collision etc.

    \param System :: Fluka Simulation
    \param IParam :: input stream
    \param setIndex :: index for the importance set
  */
{
  ELog::RegMethod RegA("flukaImpConstructor","processMat");

  // cell/mat : tag name 
  typedef std::tuple<size_t,int,std::string> impTYPE;
  static const std::map<std::string,impTYPE> IMap
    ({
      { "gas",impTYPE(1,1,"gas") },   // material : set gas
      { "rho",impTYPE(1,1,"rho") }
    });

  const std::string type=IParam.getValueError<std::string>
    ("wMAT",setIndex,0,"No type for wMAT ");

  if (type=="help" || type=="Help")
    return writeMATHelp(ELog::EM.Estream(),&ELog::endBasic);

  std::map<std::string,impTYPE>::const_iterator mc=IMap.find(type);
  if (mc==IMap.end())
    throw ColErr::InContainerError<std::string>(type,"wMat imp type unknown");

  processGeneral(System,IParam,setIndex,"wMAT",mc->second);
  return;
}

void
flukaImpConstructor::processEXP(SimFLUKA& System,
				const mainSystem::inputParam& IParam,
				const size_t setIndex)
  /*!
    Set individual EXP based on Iparam
    \param System :: Fluka simulation
    \param IParam :: input stream
    \param setIndex :: index for the importance set
  */
{
  ELog::RegMethod RegA("flukaImpConstructor","processEXP");

  // cell/mat : tag name 
  typedef std::tuple<size_t,int,std::string> impTYPE;
  static const std::map<std::string,impTYPE> IMap
    ({
      { "exp",impTYPE(1,0,"exptrans") },      // cell:
      { "particle",impTYPE(1,0,"exppart") }   // cell: 
    });

  const std::string type=IParam.getValueError<std::string>
    ("wEXP",setIndex,0,"No type for wEXP ");
    
  if (type=="help" || type=="Help")
    return writeEXPHelp(ELog::EM.Estream(),&ELog::endBasic);

  std::map<std::string,impTYPE>::const_iterator mc=IMap.find(type);
  if (mc==IMap.end())
    throw ColErr::InContainerError<std::string>(type,"exp type unknown");

  processGeneral(System,IParam,setIndex,"wEXT",mc->second);
  return;
}


void
flukaImpConstructor::processLAM(SimFLUKA& System,
				const mainSystem::inputParam& IParam,
				const size_t setIndex)
/*!
    Set individual LAM based on IParam
    \param System :: Simulation Fluka
    \param IParam :: input stream
    \param setIndex :: index for the importance set
  */
{
  ELog::RegMethod RegA("flukaImpConstructor","processLAM");

  flukaPhysics& PC= *System.getPhysics();
  // cell/mat : tag name 
  typedef std::tuple<size_t,int,std::string> lamTYPE;
  static const std::map<std::string,lamTYPE> IMap
    ({
      { "length",lamTYPE(1,-1,"lamlength") },    // particle
      { "primlen",lamTYPE(1,-1,"lamprimary") }   // particle
    });

  const std::string type=IParam.getValueError<std::string>
    ("wLAM",setIndex,0,"No type for wLAM ");

  if (type=="help" || type=="Help")
    return writeLAMHelp(ELog::EM.Estream(),&ELog::endBasic);
  
  std::map<std::string,lamTYPE>::const_iterator mc=IMap.find(type);
  if (mc==IMap.end())
    throw ColErr::InContainerError<std::string>(type,"wLAM type unknown");

  const std::string partName=IParam.getValueError<std::string>
    ("wLAM",setIndex,1,"No particle for wLAM:partName");

  const std::string cellM=IParam.getValueError<std::string>
    ("wLAM",setIndex,2,"No material for wLAM:cellM");

  const size_t cellSize(std::get<0>(mc->second));
  //  const int materialFlag(std::get<1>(mc->second));
  const std::string cardName(std::get<2>(mc->second));

  std::vector<std::string> VVList(cellSize+1);
  for(size_t i=0;i<cellSize;i++)
    VVList[i+1]=IParam.getValueError<std::string>
      ("wLAM",setIndex,3+i,
       "No value["+std::to_string(i+3)+"] for wLAM");      

  const std::set<int> activeMat=getActiveUnit(System,1,cellM);
  if (activeMat.empty())
    throw ColErr::InContainerError<std::string>(cellM,"Empty Materials:");

  VVList[0]=partName;
  for(const int CN : activeMat)
    if (CN)
      insertPair(PC,cellSize,partName,CN,cardName,VVList);

  return;
}

void
flukaImpConstructor::processEMF(SimFLUKA& System,
				const mainSystem::inputParam& IParam,
				const size_t setIndex)
  /*!
    Set individual EMFCUT based on IParam
    \param System :: Simulation for fluka
    \param IParam :: input stream
    \param setIndex :: index for the importance set
  */
{

  ELog::RegMethod RegA("flukaImpConstructor","processEMF");

  // impTYPE==> NValue : particle[-1]/cell[0]/mat[1] : tag name 
  static const std::map<std::string,impTYPE> EMap
    ({
      { "cut",impTYPE(2,0,"emfcut") },   // cell: S2 : -GeV : GeV
      { "emfcut",impTYPE(2,0,"emfcut") },   // cell: S2 : -GeV : GeV
      { "emfray",impTYPE(0,0,"emfray") },   // cell: [type 4]
      { "elecnucl",impTYPE(1,1,"elecnucl") },     // mat
      { "mupair",impTYPE(1,1,"mupair") },        // mat
      { "prodcut",impTYPE(2,1,"prodcut") }, 
      { "elpothr",impTYPE(3,1,"elpothr") },  //
      { "photthr",impTYPE(3,1,"photthr") },  // comp/photoeelec/gamma-pair
      { "pho2thr",impTYPE(2,1,"pho2thr") },  // photo-nuclear
      { "pairbrem",impTYPE(2,1,"pairbrem") }, // mat   GeV : GeV

      { "photonuc",impTYPE(0,1,"photonuc") },      // none
      { "muphoton",impTYPE(0,1,"muphoton") },      // none
      { "emffluo",impTYPE(1,1,"emffluo") },        // mat
      { "mulsopt",impTYPE(3,1,"mulsopt") },        // mat
      { "lpb",impTYPE(2,0,"lpb") },        // regions
      { "lambbrem",impTYPE(2,1,"lambbrem") },      // mat
      { "lambemf",impTYPE(2,1,"lambemf") },        // mat

      { "bias",impTYPE(2,0,"bias") },                  // cell
      { "bias-user",impTYPE(2,0,"bias-user") },        // cell

      { "evaporation",impTYPE(0,-100,"evaporation") },      // none
      { "evap-noheavy",impTYPE(0,-100,"evap-noheavy") },      // none
      { "coalescence",impTYPE(0,-100,"coalescence") },       // none
      { "ionsplit",impTYPE(0,-100,"ionsplit") }             // none

    });
  
  // must have size
  const std::string type=IParam.getValueError<std::string>
    ("wEMF",setIndex,0,"No type for wEMF ");

  if (type=="help" || type=="Help")
    return writeEMFHelp(ELog::EM.Estream(),&ELog::endBasic);

  std::map<std::string,impTYPE>::const_iterator mc=EMap.find(type);
  if (mc==EMap.end())
    throw ColErr::InContainerError<std::string>(type,"wEMF type unknown");

  ELog::EM<<"Process " <<mc->first<<ELog::endDiag;
  processGeneral(System,IParam,setIndex,"wEMF",mc->second);
  return;
}

void
flukaImpConstructor::writeCUTHelp(std::ostream& OX,
				  ENDL endDL) const
  /*!
    Write out the help
    \param OX :: Output stream
    \param endDL :: End of line type
  */
{
  OX<<"wCUT help :: \n";
  OX<<"wCUT  :: \n"
    " -- type : particle : values \n\n";

  OX<<"    partthr - energy-cut PARTICLE \n"
    << (*endDL);
  return;
}

void
flukaImpConstructor::writeIMPHelp(std::ostream& OX,
				  ENDL endDL) const
  /*!
    Write out the help
    \param OX :: Output stream
    \param endDL :: End of line type
  */
{
  OX<<"wIMP help :: \n";

  OX<<"-wIMP type value[double] object/range/cell  -- ::\n\n";
  
  OX<<"  particle : optional [default all]\n"
      "    -- all hadron electron low \n"
      "  value : value for importance \n"
      "  object : object name  \n"
      "         : object name:cellname\n"
      "         : cell number range\n"
      "         : cell number\n"
    "         : all\n"
    << (*endDL);
  return;
}

void
flukaImpConstructor::writeMATHelp(std::ostream& OX,
				  ENDL endDL) const
  /*!
    Write out the help
    \param OX :: Output stream
    \param endDL :: End of line type
  */
{
  OX<<"wMAT help :: \n";

  OX<<"-wMAT type value[double] materials  -- ::\n\n";
  
  OX<<"  gas : \n"
      "    -- material pressure[bar] \n"
      "  rho :  \n"
      "    -- material density-factor \n"
    << (*endDL);
  return;
}

void
flukaImpConstructor::writeEXPHelp(std::ostream& OX,
				  ENDL endDL) const
  /*!
    Write out the help
    \param OX :: Output stream
    \param endDL :: End of line type
  */
{
  OX<<"wEXP help :: \n";

  OX<<"-wEXP particle -- ::\n\n";
  OX<<"  exp : "
      "    value objectName" 
      "        objectAll : object name  \n"
      "             : object name:cellname\n"
      "             : cell number range\n"
      "             : cell number\n"
      "             : all\n"
    << (*endDL);
  return;
}

void
flukaImpConstructor::writeLAMHelp(std::ostream& OX,
				  ENDL endDL) const
  /*!
    Write out the help for wLAM
    \param OX :: Output stream
    \param endDL :: End of line type
  */
{
  OX<<"wLAM help :: \n";

  OX<<"-wLAM cell particle  -- ::\n\n";
  OX<<"  exp : "
      "    value objectName" 
      "        objectAll : object name  \n"
      "             : object name:cellname\n"
      "             : cell number range\n"
      "             : cell number\n"
      "             : all\n"
    << (*endDL);
  return;
}

void
flukaImpConstructor::writeBIASHelp(std::ostream& OX,
				   ENDL endDL) const
  /*!
    Write out the help for wLAM
    \param OX :: Output stream
    \param endDL :: End of line type
  */
{
  OX<<"wBIAS help :: \n";

  OX<<"-wBIAS cell particle  -- ::\n\n";
  OX<<"  bias/user : "
      "     particleType splitting cell:\n"
      "        objectAll : object name  \n"
      "             : object name:cellname\n"
      "             : cell number range\n"
      "             : cell number\n"
      "             : all\n"
    << (*endDL);
  return;
}

void
flukaImpConstructor::writeEMFHelp(std::ostream& OX,
				  ENDL endDL) const
  /*!
    Write out the help
    \param OX :: Output stream
    \param endDL :: End of line type
  */
{
  OX<<"wEMF help :: \n"
    " -- type : Mat/Cell : Values \n\n";
  OX<<"    emfcut - electron-transport-cut photon-trans-cut CELL \n"
      "    prodcut - e+/e-prod  gamma-prod  MAT \n"
      "    emffluo - (state) turns off/on x-ray fluorescence MAT \n"
      "    emfray - (state) turns off/on full rayleigh/compton CELL \n"
      "    elpothr - e+/e-brem-thresh MollerScat  e-photonuc MAT \n"
      "    photthr - compton photoelec gamma-pair MAT \n"
      "    pho2thr - rayliegh gamma-photonuc MAT \n"
      "    pairbrem - explit-pair-prod photon-bremst-prod MAT "
      "    photonuc - [FLAG] turn photonuclear on MAT \n"
      "    muphoton - [FLAG] mu-interaction MAT \n"  
      "    mulsopt - multscat-flag[-3:3] e+/e- multFlag[ -1:3]\n"
      "    lpb - (e+/e-) top Energy for e/e+  : \n"
      "               top Energy for photon CELL \n"
      "    lambbrem - brem-bias weight (e+/e-) [0.0 - 1.0] : \n"
      "               number of collisions\n"
      "    lambemf - brem-bias-weight (e+/e-) [0.0 - 1.0] : \n"
      "              compton-bias-weight (e+/e-) [0.0 - 1.0] : \n"
      "              number of collisions\n"
  << (*endDL);
  return;
}

} // Namespace flukaSystem
