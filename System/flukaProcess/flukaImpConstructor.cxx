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
#include "cellValueSet.h"
#include "flukaPhysics.h"
#include "flukaProcess.h"
#include "flukaImpConstructor.h"

namespace flukaSystem
{
  
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

  
  // cell/mat : tag name /  scale V1 / scale V2 [if used]
  typedef std::tuple<size_t,bool,std::string> impTYPE;
  static const std::map<std::string,impTYPE> IMap
    ({
      { "all",impTYPE(1,1,"all") },   // cell: 
      { "hadron",impTYPE(1,1,"hadron") },  
      { "electron",impTYPE(1,1,"electron") },
      { "low",impTYPE(1,1,"low") } 
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
  const bool materialFlag(std::get<1>(mc->second));
  const std::string keyName(std::get<2>(mc->second));

  std::string VV[3];
  for(size_t i=0;i<cellSize;i++)
    VV[i]=IParam.getValueError<std::string>
      ("wIMP",setIndex,2+i,
       "No value["+std::to_string(i+1)+"] for wIMP ");      

  std::set<int> activeCell=
    (!materialFlag) ? getActiveCell(cellM) : getActiveMaterial(cellM);

  if (activeCell.empty())
    throw ColErr::InContainerError<std::string>(cellM,"Empty cell");

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
      { "pho2thr",emfTYPE(2,1,"pho2thr") },  // photo-nuclear
      { "pairbrem",emfTYPE(2,1,"pairbrem") }, // mat   GeV : GeV
      { "photonuc",emfTYPE(0,1,"photonuc") },     // mat
      { "muphoton",emfTYPE(0,1,"muphoton") }      // mat 
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
  const bool materialFlag(std::get<1>(mc->second));
  const std::string keyName(std::get<2>(mc->second));

 
  std::string VV[3];
  for(size_t i=0;i<cellSize;i++)
    VV[i]=IParam.getValueError<std::string>
      ("wEMF",setIndex,2+i,
       "No value["+std::to_string(i+1)+"] for wEMF: "+type);      
  
  std::set<int> activeCell=
    (!materialFlag) ? getActiveCell(cellM) : getActiveMaterial(cellM);

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
      "         : all\n";
  OX<< (*endDL);
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
    " - types \n"
    " -- cut \n"
    "      Cell-Range electronCut[MeV] gammaCut[MeV]\n"
    " -- pairbrem \n"
    "      Mat-Range   \n"
    " -- elpothr \n"
    "      compton photoelc gammpair Mat-Range   \n"
    " -- prodcut \n"
    "      compton photoelc gammpair Mat-Range   \n"
    " -- pho2-thr \n"
    "      photonuc-threshhold Mat-Range   \n";
  OX << (*endDL);
  return;
}

} // Namespace flukaSystem
