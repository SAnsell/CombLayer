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
   Process the actual insert 
   \param PC :: Physcis to insert to
   \param cellSize :: cell Unit size
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

  const flukaGenParticle& FG=flukaGenParticle::Instance();
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

  const std::string cellM=IParam.getValueError<std::string>
    ("wCUT",setIndex,1,"No cell/material/particle for wCUT ");

  const size_t cellSize(std::get<0>(mc->second));
  const int materialFlag(std::get<1>(mc->second));
  const std::string keyName(std::get<2>(mc->second));

  std::string VV[3];
  for(size_t i=0;i<cellSize;i++)
    VV[i]=IParam.getValueError<std::string>
      ("wCUT",setIndex,2+i,
       "No value["+std::to_string(i+1)+"] for wCUT ");      

  const std::set<int> activeCell=
    getActiveUnit(materialFlag,cellM);

  if (materialFlag<0)
    {
      const std::string& pName=FG.nameToFLUKA(cellM);
      insertParticle(PC,cellSize,pName,keyName,VV);
    }
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
  typedef std::tuple<size_t,bool,std::string> impTYPE;
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
    throw ColErr::InContainerError<std::string>(type,"imp type unknown");

  const std::string cellM=IParam.getValueError<std::string>
    ("wIMP",setIndex,1,"No cell/material for wIMP ");

  const size_t cellSize(std::get<0>(mc->second));
  const int materialFlag(std::get<1>(mc->second));
  const std::string keyName(std::get<2>(mc->second));

  std::string VV[3];
  for(size_t i=0;i<cellSize;i++)
    VV[i]=IParam.getValueError<std::string>
      ("wIMP",setIndex,2+i,
       "No value["+std::to_string(i+1)+"] for wIMP ");      

  if (materialFlag>=0)
    {
      const std::set<int> activeCell=
	getActiveUnit(materialFlag,cellM);
      if (activeCell.empty())
	throw ColErr::InContainerError<std::string>(cellM,"Empty cell");
      insertCell(PC,cellSize,activeCell,keyName,VV);
    }
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

  const std::string cellM=IParam.getValueError<std::string>
    ("wEXP",setIndex,1,"No cell/material for wEXP ");

  const size_t cellSize(std::get<0>(mc->second));
  const int materialFlag(std::get<1>(mc->second));
  const std::string keyName(std::get<2>(mc->second));

  std::string VV[3];
  for(size_t i=0;i<cellSize;i++)
    VV[i]=IParam.getValueError<std::string>
      ("wEXP",setIndex,2+i,
       "No value["+std::to_string(i+1)+"] for wEXP ");      

  if (materialFlag>0)
    {
      const std::set<int> activeCell=
	(!materialFlag) ? getActiveCell(cellM) : getActiveMaterial(cellM);
      if (activeCell.empty())
	throw ColErr::InContainerError<std::string>(cellM,"Empty cell");
      insertCell(PC,cellSize,activeCell,keyName,VV);
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
      { "mulsopt",emfTYPE(3,1,"mulsopt") }       // mat
    });
  
  // must have size
  const std::string type=IParam.getValueError<std::string>
    ("wEMF",setIndex,0,"No type for wEMF ");

  if (type=="help" || type=="Help")
    return writeEMFHelp(ELog::EM.Estream(),&ELog::endBasic);

  std::map<std::string,emfTYPE>::const_iterator mc=EMap.find(type);
  if (mc==EMap.end())
    throw ColErr::InContainerError<std::string>(type,"emf type unknown");

  const std::string cellM=IParam.getValueError<std::string>
    ("wEMF",setIndex,1,"No cell/material for wEMF ");


  const size_t cellSize(std::get<0>(mc->second));
  const int materialFlag(std::get<1>(mc->second));
  const std::string keyName(std::get<2>(mc->second));

 
  std::string VV[3];
  for(size_t i=0;i<cellSize;i++)
    VV[i]=IParam.getValueError<std::string>
      ("wEMF",setIndex,2+i,
       "No value["+std::to_string(i+1)+"] for wEMF: "+type);      

  if (materialFlag>0)
    {
      const std::set<int> activeCell=
	(!materialFlag) ? getActiveCell(cellM) : getActiveMaterial(cellM);
      if (activeCell.empty())
	throw ColErr::InContainerError<std::string>(cellM,"Empty cell");
      insertCell(PC,cellSize,activeCell,keyName,VV);
    }
  
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

  OX<<"    prodthr - energy-ctu PARTICLE \n"
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
flukaImpConstructor::writeEMFHelp(std::ostream& OX,
				  ENDL endDL) const
  /*!
    Write out the help
    \param OX :: Output stream
    \param endDL :: End of line type
  */
{
  OX<<"wEMF help :: \n"
    " -- type : values : Mat/Cell\n\n";
  OX<<"    emfcut - electron-transport-cut photon-trans-cut CELL \n"
      "    prodcut - e+/e-prod  gamma-prod  MAT \n"
      "    elpothr - e+/e-brem-thresh MollerScat  e-photonuc MAT \n"
      "    photthr - compton photoelec gamma-pair MAT \n"
      "    pho2thr - rayliegh gamma-photonuc MAT \n"
      "    pairbrem - explit-pair-prod photon-bremst-prod MAT "
      "    photonuc - [FLAG] turn photonuclear on MAT \n"
      "    muphoton - [FLAG] mu-interaction MAT \n"  
      "    mulsopt - multscat-flag[-3:3] e+/e- multFlag[ -1:3]\n"        
  << (*endDL);
  return;
}

} // Namespace flukaSystem
