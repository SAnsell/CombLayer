/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/flukaImpConstructor.cxx
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
#include "Triple.h"
#include "NList.h"
#include "NRange.h"
#include "support.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "objectRegister.h"
#include "inputParam.h"
#include "strValueSet.h"
#include "cellValueSet.h"
#include "flukaGenParticle.h"
#include "flukaPhysics.h"
#include "flukaProcess.h"
#include "flukaImpConstructor.h"

namespace flukaSystem
{


void
flukaImpConstructor::insertParticle(flukaPhysics& PC,
				    const size_t cellSize,
				    const std::string& pName,
				    const std::string& keyName,
				    const std::string* VV) const
 /*!
   Process the actual insert 
   \param PC :: Physcis to insert to
   \param cellSize :: cell Unit size
   \param pName :: particle name
   \param keyName :: component keyname
   \param VV :: variables
 */
{
  ELog::RegMethod RegA("flukaImpConstructor","insertParticle");
  
  switch (cellSize)
    {
    case 0:
      PC.setFlag(keyName,pName);
      break;
    case 1:
      PC.setImp(keyName,pName,VV[0]);
      break;
    case 2:
      PC.setEMF(keyName,pName,VV[0],VV[1]);
      break;
    case 3:
      PC.setTHR(keyName,pName,VV[0],VV[1],VV[2]);
      break;
    }
  return;
}

void
flukaImpConstructor::insertCell(flukaPhysics& PC,
				const size_t cellSize,
				const std::set<int>& activeCell,
				const std::string& keyName,
				const std::string* VV) const
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
	PC.setImp(keyName,MN,VV[0]);
      break;
    case 2:
      for(const int MN : activeCell)
	PC.setEMF(keyName,MN,VV[0],VV[1]);
      break;
    case 3:
      for(const int MN : activeCell)
	PC.setTHR(keyName,MN,VV[0],VV[1],VV[2]);
      break;
    }
  return;
}				    


void
flukaImpConstructor::processGeneral(flukaPhysics& PC,
				    const mainSystem::inputParam& IParam,
				    const size_t setIndex,
				    const std::string& keyName,
				    const impTYPE& cutTuple) const
  /*!
    Handler for constructor of physics cards
    \param PC :: Physics cards
    \param IParam :: input stream
    \param setIndex :: index value for item
    \param keyName :: wTYPE card
    \param cutTuple :: system for cellStr construction
  */
{
  ELog::RegMethod RegA("flukaImpConstructor","processGeneral");

  const std::string cellM=IParam.getValueError<std::string>
    (keyName,setIndex,1,"No cell/material for "+keyName);

  const size_t cellSize(std::get<0>(cutTuple));
  const int materialFlag(std::get<1>(cutTuple));
  const std::string cardName(std::get<2>(cutTuple));

  std::string VV[3];
  for(size_t i=0;i<cellSize;i++)
    VV[i]=IParam.getValueError<std::string>
      (keyName,setIndex,2+i,
       "No value["+std::to_string(i+1)+"] for "+keyName);      


  if (materialFlag>=0)
    {
      const std::set<int> activeCell=getActiveUnit(materialFlag,cellM);
      if (activeCell.empty())
	throw ColErr::InContainerError<std::string>(cellM,"Empty cell:");
      insertCell(PC,cellSize,activeCell,cardName,VV);
    }
  else
    {
      const flukaGenParticle& FG=flukaGenParticle::Instance();
      const std::string& pName=FG.nameToFLUKA(cellM);
      insertParticle(PC,cellSize,pName,cardName,VV);
    }
  return;
}

void
flukaImpConstructor::processCUT(flukaPhysics& PC,
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

  processGeneral(PC,IParam,setIndex,"wCUT",mc->second);
  return;
}
 
void
flukaImpConstructor::processMAT(flukaPhysics& PC,
				const mainSystem::inputParam& IParam,
				const size_t setIndex)
 /*!
    Set individual wMAT based on IParam
    This is to set mat-prop mainly for correct to density 
    so that we correctly loose the correct amount of dE/dx 
    but can enhance the bremstauhlung collision etc.

    \param PC :: PhysicsCards
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

  processGeneral(PC,IParam,setIndex,"wMAT",mc->second);
  return;
}

void
flukaImpConstructor::processUnit(flukaPhysics& PC,
				 const mainSystem::inputParam& IParam,
				 const size_t setIndex)
/*!
    Set individual IMP based on Iparam
    \param PC :: PhysicsCards
    \param IParam :: input stream
    \param setIndex :: index for the importance set
  */
{
  ELog::RegMethod RegA("flukaImpConstructor","processUnit");

  // cell/mat : tag name 
  typedef std::tuple<size_t,int,std::string> impTYPE;
  static const std::map<std::string,impTYPE> IMap
    ({
      { "all",impTYPE(1,0,"all") },   // cell: 
      { "hadron",impTYPE(1,0,"hadron") },  
      { "electron",impTYPE(1,0,"electron") },
      { "low",impTYPE(1,0,"low") }
    });

  const std::string type=IParam.getValueError<std::string>
    ("wIMP",setIndex,0,"No type for wIMP ");

  if (type=="help" || type=="Help")
    return writeIMPHelp(ELog::EM.Estream(),&ELog::endBasic);

  std::map<std::string,impTYPE>::const_iterator mc=IMap.find(type);
  if (mc==IMap.end())
    throw ColErr::InContainerError<std::string>(type,"wIIMP imp type unknown");

  processGeneral(PC,IParam,setIndex,"wIMP",mc->second);
  return;
}

void
flukaImpConstructor::processEXP(flukaPhysics& PC,
				const mainSystem::inputParam& IParam,
				const size_t setIndex)
  /*!
    Set individual EXP based on Iparam
    \param PC :: PhysicsCards
    \param IParam :: input stream
    \param setIndex :: index for the importance set
  */
{
  ELog::RegMethod RegA("flukaImpConstructor","processEXP");

  // cell/mat : tag name 
  typedef std::tuple<size_t,bool,std::string> impTYPE;
  static const std::map<std::string,impTYPE> IMap
    ({
      { "exp",impTYPE(1,0,"exptrans") },   // cell:
      { "particle",impTYPE(1,0,"exppart") }   // cell: 
    });

  const std::string type=IParam.getValueError<std::string>
    ("wEXP",setIndex,0,"No type for wEXP ");
    
  if (type=="help" || type=="Help")
    return writeEXPHelp(ELog::EM.Estream(),&ELog::endBasic);

  std::map<std::string,impTYPE>::const_iterator mc=IMap.find(type);
  if (mc==IMap.end())
    throw ColErr::InContainerError<std::string>(type,"exp type unknown");

  processGeneral(PC,IParam,setIndex,"wEXT",mc->second);
  return;
}


void
flukaImpConstructor::processLAM(flukaPhysics& PC,
				const mainSystem::inputParam& IParam,
				const size_t setIndex)
/*!
    Set individual LAM based on IParam
    \param PC :: PhysicsCards
    \param IParam :: input stream
    \param setIndex :: index for the importance set
  */
{
  ELog::RegMethod RegA("flukaImpConstructor","processLAM");
  
  const flukaGenParticle& FG=flukaGenParticle::Instance();
  // cell/mat : tag name 
  typedef std::tuple<size_t,int,std::string> lamTYPE;
  static const std::map<std::string,lamTYPE> IMap
    ({
      { "length",lamTYPE(1,-1,"lamlength") }   // material 
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
  const int materialFlag(std::get<1>(mc->second));
  const std::string cardName(std::get<2>(mc->second));

  std::string VV[4];
  for(size_t i=0;i<cellSize;i++)
    VV[i+1]=IParam.getValueError<std::string>
      ("wLAM",setIndex,3+i,
       "No value["+std::to_string(i+3)+"] for wLAM");      

  const std::set<int> activeMat=getActiveUnit(1,cellM);
  if (activeMat.empty())
    throw ColErr::InContainerError<std::string>(cellM,"Empty Materials:");
  
  //  VV[0]=FG.nameToFLUKA(partName);
  for(const int CN : activeMat)
    {
      ELog::EM<<"Part name == "<<partName<<ELog::endDiag;
      VV[0]=std::to_string(CN);
      insertParticle(PC,cellSize+1,partName,cardName,VV);
    }
  return;
}

void
flukaImpConstructor::processEMF(flukaPhysics& PC,
				const mainSystem::inputParam& IParam,
				const size_t setIndex)
/*!
    Set individual EMFCUT based on IParam
    \param PC :: PhysicsCards
    \param IParam :: input stream
    \param setIndex :: index for the importance set
  */
{
  ELog::RegMethod RegA("flukaImpConstructor","processEMF");

  // cell/mat : tag name /  scale V1 / scale V2 [if used]
  typedef std::tuple<size_t,bool,std::string> emfTYPE;

  static const std::map<std::string,emfTYPE> EMap
    ({
      { "cut",emfTYPE(2,0,"emfcut") },   // cell: S2 : -GeV : GeV
      { "emfcut",emfTYPE(2,0,"emfcut") },   // cell: S2 : -GeV : GeV
      { "prodcut",emfTYPE(2,1,"prodcut") }, 
      { "elpothr",emfTYPE(3,1,"elpothr") },  //
      { "photthr",emfTYPE(3,1,"photthr") },  // comp/photoeelec/gamma-pair
      { "pho2thr",emfTYPE(2,1,"pho2thr") },  // photo-nuclear
      { "pairbrem",emfTYPE(2,1,"pairbrem") }, // mat   GeV : GeV
      { "photonuc",emfTYPE(0,1,"photonuc") },     // mat
      { "muphoton",emfTYPE(0,1,"muphoton") },      // mat
      { "emffluo",emfTYPE(0,1,"emffluo") },      // mat
      { "mulsopt",emfTYPE(3,1,"mulsopt") },       // mat
      { "lpb",emfTYPE(2,0,"lpb") },        // regions
      { "lambbrem",emfTYPE(2,1,"lambbrem") },      // mat
      { "lambemf",emfTYPE(2,1,"lambemf") },      // mat
      { "plambias",emfTYPE(1,1,"plambias") },    // primary lam-bias
      { "lambias",emfTYPE(1,1,"lambias") }      // mat

    });
  
  // must have size
  const std::string type=IParam.getValueError<std::string>
    ("wEMF",setIndex,0,"No type for wEMF ");

  if (type=="help" || type=="Help")
    return writeEMFHelp(ELog::EM.Estream(),&ELog::endBasic);

  std::map<std::string,emfTYPE>::const_iterator mc=EMap.find(type);
  if (mc==EMap.end())
    throw ColErr::InContainerError<std::string>(type,"wEMF type unknown");

  processGeneral(PC,IParam,setIndex,"wEMF",mc->second);
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
      "    lpbbias[LeadParticleBias] - type e+/e-thresh photon-thresh  CELL \n"
      "    prodcut - e+/e-prod  gamma-prod  MAT \n"
      "    emffluo - [FLAG] turns off x-ray fluorescence MAT \n"
      "    elpothr - e+/e-brem-thresh MollerScat  e-photonuc MAT \n"
      "    photthr - compton photoelec gamma-pair MAT \n"
      "    pho2thr - rayliegh gamma-photonuc MAT \n"
      "    pairbrem - explit-pair-prod photon-bremst-prod MAT "
      "    photonuc - [FLAG] turn photonuclear on MAT \n"
      "    muphoton - [FLAG] mu-interaction MAT \n"  
      "    mulsopt - multscat-flag[-3:3] e+/e- multFlag[ -1:3]\n"
      "    lpb - (e+/e-) top Energy for e/e+  : \n"
      "               top Energy for photon \n"
      "    lambbrem - brem-bias weight (e+/e-) [0.0 - 1.0] : \n"
      "               number of collisions\n"
      "    lambemf - brem-bias-weight (e+/e-) [0.0 - 1.0] : \n"
      "              compton-bias-weight (e+/e-) [0.0 - 1.0] : \n"
      "              number of collisions\n"
  << (*endDL);
  return;
}

} // Namespace flukaSystem
