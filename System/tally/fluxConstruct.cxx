/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/fluxConstruct.cxx
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
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Triple.h"
#include "support.h"
#include "NList.h"
#include "NRange.h"
#include "Tally.h"
#include "TallyCreate.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "MXcards.h"
#include "Zaid.h"
#include "Material.h"
#include "DBMaterial.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"

#include "inputParam.h"
#include "objectSupport.h" 
#include "fluxConstruct.h" 

namespace tallySystem
{

int
fluxConstruct::processFlux(SimMCNP& System,
			   const mainSystem::inputParam& IParam,
			   const size_t Index) 
  /*!
    Add flux tally (s) as needed
    \param System :: SimMCNP to add tallies
    \param IParam :: Main input parameters
    \param Index :: index of the -T card
  */
{
  ELog::RegMethod RegA("fluxConstruct","processFlux");

  const size_t NItems=IParam.itemCnt("tally",Index);
  if (NItems<4)
    throw ColErr::IndexError<size_t>(NItems,4,
				     "Insufficient items for tally");
  // PARTICLE TYPE
  const std::string PType(IParam.getValue<std::string>("tally",Index,1)); 
  const std::string MType(IParam.getValue<std::string>("tally",Index,2));
  const std::string cellKey(IParam.getValue<std::string>("tally",Index,3)); 

  // Get Material number:
  int matN(0);
  if (!StrFunc::convert(MType,matN))
    {
      if (MType=="All" || MType=="all")
	matN=-2;
      else if (MType=="AllNonVoid" || MType=="allNonVoid")
	matN=-1;
      else
	{
	  matN=ModelSupport::DBMaterial::Instance().getIndex(MType);
	}
    }
  
  const std::vector<int> cells=
    objectSupport::getCellSelection(System,matN,cellKey);

  if (cells.empty())
    throw ColErr::InContainerError<std::string>
      (cellKey+"/"+MType,"cell/mat not present in model");
  

  const int nTally=System.nextTallyNum(4);
  tallySystem::addF4Tally(System,nTally,PType,cells);
  tallySystem::Tally* TX=System.getTally(nTally); 
  TX->setPrintField("e f");
  const std::string Comment=
    "tally: "+std::to_string(nTally)+
    " mat : "+std::to_string(matN)+":"+
    cellKey;
  TX->setComment(Comment);
  return 0;
}


void
fluxConstruct::writeHelp(std::ostream& OX) 
  /*!
    Write out help
    \param OX :: output stream
  */
{
  OX<<"Flux tally :\n"
    "particles material(int) cells \n"
    " -- material can be: \n"
    "      zaid number \n"
    "      material name \n"
    "      material number \n"
    "      -1 :: [all] \n"
    "  -- cells can be \n"
    "          objectName [cellOffset cellCount]\n"
    "          objectName \n "
    "          CellNumber-CellNumber\n";
  return;
}
  

}  // NAMESPACE tallySystem
