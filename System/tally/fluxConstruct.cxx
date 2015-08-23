/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/fluxConstruct.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include <boost/format.hpp>

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

#include "TallySelector.h" 
#include "basicConstruct.h" 
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
			   const size_t Index,
			   const bool renumberFlag) const
  /*!
    Add heat tally (s) as needed
    \param System :: Simulation to add tallies
    \param IParam :: Main input parameters
    \param Index :: index of the -T card
    \param renumberFlag :: Is this a renumber call
  */
{
  ELog::RegMethod RegA("fluxConstruct","processFlux");

  const size_t NItems=IParam.itemCnt("tally",Index);

  
  if (NItems<3)
    throw ColErr::IndexError<size_t>(NItems,3,
				     "Insufficient items for tally");
  // PARTICLE TYPE
  const std::string PType(IParam.getValue<std::string>("tally",Index,1)); 
  const std::string MType(IParam.getValue<std::string>("tally",Index,2)); 
  if (MType=="cell")
    return processFluxCell(System,IParam,Index,renumberFlag);
  // Process a Ranged Heat:
 
  boost::format Cmt("tally: %d Mat %d Range(%d,%d)");
  int matN(0);

  // Get Material number:
  if (!StrFunc::convert(MType,matN))
    {
      // Failed to convert to a number :: Must convert
      // the string to a material number [else throws]
      matN=ModelSupport::DBMaterial::Instance().getIndex(MType);
    }

  int RA,RB;
  // if flag true : then valid range
  const int flag=
    convertRegion(IParam,"tally",Index,3,RA,RB);
  // work for later:

  if (flag<0 && !renumberFlag) return 1;

  ELog::EM<<"Cells == "<<RA<<" "<<RB<<ELog::endDiag;
  const int nTally=System.nextTallyNum(4);
  // Find cells  [ Must handle -ve / 0 ]
  const std::vector<int> cells=getCellSelection(System,matN,RA,RB);

  tallySystem::addF4Tally(System,nTally,PType,cells);
  tallySystem::Tally* TX=System.getTally(nTally); 
  TX->setPrintField("e f");
  const std::string Comment=(Cmt % nTally % matN % RA % RB ).str();
  TX->setComment(Comment);
  return 0;
}

int
fluxConstruct::processFluxCell(Simulation& System,
			       const mainSystem::inputParam& IParam,
			       const size_t Index,
			       const bool renumberFlag) const
  /*!
    Process a flux on a cell
    \param System :: Simulation to use
    \param IParam :: Input system
    \param Index :: index of the -T card
    \param renumberFlag :: Is this a renumber call
    \return renumber required status [1 for true]
  */
{
  ELog::RegMethod RegA("fluxConstruct","processFluxCell");

  const ModelSupport::objectRegister& OR= 
    ModelSupport::objectRegister::Instance();

  const size_t NItems=IParam.itemCnt("tally",Index);
  if (NItems<5)
    {
      ELog::EM<<"Require : cell : Object matN"<<ELog::endCrit;
      throw ColErr::IndexError<size_t>(NItems,5,
				       "Insufficient items for tally");
    }

  // Particles
  const std::string PType(IParam.getValue<std::string>("tally",Index,1)); 

  // CellRegion can be object name or number:
  const std::string CellRegion
    (IParam.getValue<std::string>("tally",Index,3)); 

  // returns 0 if failed to find
  const int cellOffset=OR.getCell(CellRegion);
  const int cellRange=(cellOffset) ? 
    OR.getRange(CellRegion) : 1000000;
  size_t nCount((cellOffset) ? 4 : 3);
  
  std::vector<int> cellVec;
  int cellNum,RA,RB;
  int initTally(1);

  while(nCount<NItems)
    {
      const std::string CVal=
	IParam.getValue<std::string>("tally",Index,nCount);

      if (StrFunc::convert(CVal,cellNum) &&
	  System.existCell(cellNum+cellOffset))
	cellVec.push_back(cellNum+cellOffset);
      else if (basicConstruct::convertRange(CVal,RA,RB)) // X-Y
	{
	  RB=std::max<int>(cellRange,RB);
	  for(;RA<=RB;RA++)
	    if (System.existCell(RA+cellOffset))
	      cellVec.push_back(RA+cellOffset);
	}
      else if (CVal=="All" || CVal=="all")   // Everything
	{
	  for(RA=1;RA<cellRange;RA++)
	    if (System.existCell(RA+cellOffset))
	      cellVec.push_back(RA+cellOffset);
	}
      else if (initTally)
	{
	  ELog::EM<<"Failed to build flux tally ::"<<ELog::endCrit;
	  ELog::EM<<"Requires: cell Object [cellNum / cellNum-cellNum]"
		  <<ELog::endDiag;
	  ELog::EM<<"CVale = "<<CVal<<ELog::endCrit;
	  ELog::EM<<"Line == "<<IParam.getFull("tally",Index)<<ELog::endErr;
	}
      initTally=0;
      nCount++;
    }  
  ELog::EM<<"CellRegion== "<<cellVec.size()<<ELog::endDiag;  
  const int nTally=System.nextTallyNum(4);

  tallySystem::addF4Tally(System,nTally,PType,cellVec);
  tallySystem::Tally* TX=System.getTally(nTally); 
  TX->setPrintField("e f");
  boost::format Cmt("tally: %d Cell %s ");
  const std::string Comment=(Cmt % nTally % CellRegion ).str();
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
    "particles material(int) objects \n"
    "particles material(int) Range(low-high)\n"
    "particles cell objectNam offsets \n"
    " -- material can be: \n"
    "      zaid number \n"
    "      material name \n"
    "      material number \n"
    "      -1 :: [all] \n"
    "  -- cell [keywords] \n"
    "          objectName [cellOffset cellCount]\n";
    "          objectName all\n";
    "  -- cell  \n"
    "          cellOffset cellCount\n";
  return;
}
  

}  // NAMESPACE tallySystem
