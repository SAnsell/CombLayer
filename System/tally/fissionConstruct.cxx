/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/fissionConstruct.cxx
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
#include <boost/format.hpp>

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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"

#include "inputParam.h"

#include "fissionConstruct.h" 

namespace tallySystem
{

int
fissionConstruct::convertRange(const std::string& Word,
			       int& RA,int &RB)
  /*!
    Convert a pair range into two numbers:
    Range given as X - Y
    \param Word :: Unit to convert
    \param RA :: First nubmer
    \param RB :: Second number
    \return true/false
  */
{
  ELog::RegMethod RegA("fissionConstruct","convertRange");

  int A,B;
  size_t ALen=StrFunc::convPartNum(Word,A);
  if (!ALen) return 0;
  for(;ALen<Word.size() && Word[ALen]!='-';ALen++) ;
  if (ALen==Word.size()) return 0;
  
  if (!StrFunc::convert(Word.substr(ALen),B))
    return 0;
  RA=A;
  RB=B;
  return 1;
}

int
fissionConstruct::processPower(SimMCNP& System,
			       const mainSystem::inputParam& IParam,
			       const size_t Index) 
  /*!
    Process a fission power on a cell
    \param System :: SimMCNP to use
    \param IParam :: Input system
    \param Index :: index of the -T card
    \return renumber required status [1 for true]
  */
{
  ELog::RegMethod RegA("fissionConstruct","processPower");

  const size_t NItems=IParam.itemCnt("tally",Index);
  if (NItems<4)
    throw ColErr::IndexError<size_t>(NItems,3,
				     "Insufficient items for tally");

  // CellRegion can be object name or number:
  const std::string CellRegion
    (IParam.getValue<std::string>("tally",Index,2)); 

  // returns 0 if failed to find
  const int cellOffset=System.getFirstCell(CellRegion);
  
  
  size_t nCount((cellOffset) ? 3 : 2);
  
  std::vector<int> cellVec;
  int cellNum,RA,RB;
  int initTally(1);
  while(nCount<NItems)
    {
      std::string CVal=
	IParam.getValue<std::string>("tally",Index,nCount);

      if (StrFunc::convert(CVal,cellNum) &&
	  System.existCell(cellNum+cellOffset))
	cellVec.push_back(cellNum+cellOffset);
      else if (convertRange(CVal,RA,RB))
	{
	  for(;RA<=RB;RA++)
	    if (System.existCell(RA+cellOffset))
	      cellVec.push_back(RA+cellOffset);
	}
      else if (initTally)
	ELog::EM<<"Failed to build flux tally ::"
		<<IParam.getFull("tally",Index)<<ELog::endErr;
      initTally=0;
      nCount++;
    }  

  const int nTally=System.nextTallyNum(7);

  tallySystem::addF7Tally(System,nTally,cellVec);
  tallySystem::Tally* TX=System.getTally(nTally); 
  TX->setPrintField("e f");
  boost::format Cmt("tally: %d Cell %s ");
  const std::string Comment=(Cmt % nTally % CellRegion ).str();
  TX->setComment(Comment);

  return 0;
}

void
fissionConstruct::writeHelp(std::ostream& OX) 
  /*!
    Write out the help
    \param OX :: Output stream
   */
{
  OX<<"Fission tally options:\n"
    <<"object cellrange";
  return;
}


}  // NAMESPACE tallySystem
