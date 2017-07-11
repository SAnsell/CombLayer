/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/fluxConstruct.cxx
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
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "support.h"
#include "stringCombine.h"
#include "NList.h"
#include "NRange.h"
#include "Tally.h"
#include "TallyCreate.h"
#include "Quaternion.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "objectRegister.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "MXcards.h"
#include "Zaid.h"
#include "Material.h"
#include "DBMaterial.h"
#include "Simulation.h"

#include "inputParam.h"
#include "objectSupport.h" 
#include "TallySelector.h" 
#include "fluxConstruct.h" 

namespace tallySystem
{

fluxConstruct::fluxConstruct() 
  /// Constructor
{}

fluxConstruct::fluxConstruct(const fluxConstruct&) 
  /// Copy Constructor
{}

fluxConstruct&
fluxConstruct::operator=(const fluxConstruct&) 
  /// Assignment operator
{
  return *this;
}

int
fluxConstruct::processFlux(Simulation& System,
			   const mainSystem::inputParam& IParam,
			   const size_t Index) const
  /*!
    Add flux tally (s) as needed
    \param System :: Simulation to add tallies
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
      (cellKey+"/"+StrFunc::makeString(MType),"cell/mat not present in model");
  

  const int nTally=System.nextTallyNum(4);
  tallySystem::addF4Tally(System,nTally,PType,cells);
  tallySystem::Tally* TX=System.getTally(nTally); 
  TX->setPrintField("e f");
  const std::string Comment=
    "tally: "+StrFunc::makeString(nTally)+
    " mat : "+StrFunc::makeString(matN)+":"+
    cellKey;
  TX->setComment(Comment);
  return 0;
}


void
fluxConstruct::writeHelp(std::ostream& OX) const
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
