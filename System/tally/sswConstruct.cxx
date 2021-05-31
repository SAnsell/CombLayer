/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/sswConstruct.cxx
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
#include "stringCombine.h"
#include "NList.h"
#include "NRange.h"
#include "Tally.h"
#include "TallyCreate.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "BaseMap.h"
#include "SurfMap.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"

#include "inputParam.h"

#include "sswTally.h" 
#include "sswConstruct.h" 

namespace tallySystem
{

void
sswConstruct::processSSW(SimMCNP& System,
			 const mainSystem::inputParam& IParam,
			 const size_t Index) 
/*!
    Add ssw tally as needed
    \param System :: SimMCNP to add tallies
    \param IParam :: Main input parameters
    \param Index :: index of the -T card
  */
{
  ELog::RegMethod RegA("sswConstruct","processSSW");

  const size_t NItems=IParam.itemCnt("tally",Index);
  if (NItems<4)
    throw ColErr::IndexError<size_t>(NItems,4,
				     "Insufficient items for tally");

  std::string eMess
    ("Insufficient item for activation["+StrFunc::makeString(Index)+"]");
        
  const std::string PType=
    IParam.getValueError<std::string>("tally",Index,1,eMess); 
  eMess+="with key["+PType+"]";

  std::vector<int> SList;
  if (PType=="object")
    {
      const std::string FCName=
        IParam.getValueError<std::string>("tally",Index,2,eMess);
      const std::string linkPt=
        IParam.getValueError<std::string>("tally",Index,3,eMess);
      const attachSystem::FixedComp* FCPtr=
        System.getObjectThrow<attachSystem::FixedComp>
        (FCName,"FixedComp");
      
      const long int sideIndex(FCPtr->getSideIndex(linkPt));
      const std::set<int> OutSurf=
        FCPtr->getMainRule(sideIndex).getSurfSet();
      for(const int CN : OutSurf)
        SList.push_back(CN);
    }
  else if (PType=="surfMap" || PType=="SurfMap")
    {
      const std::string SMName=
        IParam.getValueError<std::string>("tally",Index,2,eMess);
      const std::string surfObj=
        IParam.getValueError<std::string>("tally",Index,3,eMess);
      const size_t index=
        IParam.getDefValue<size_t>(0,"tally",Index,4);

      const attachSystem::SurfMap* SMPtr=
        System.getObjectThrow<attachSystem::SurfMap>(SMName,"SurfMap");

      if (index)
	SList.push_back(SMPtr->getSurf(surfObj,index-1));
      else
	SList=SMPtr->getSurfs(surfObj);
    }
  else
    throw ColErr::InContainerError<std::string>(PType,"PType not known");

  tallySystem::sswTally* SSWX=tallySystem::addSSWTally(System);
  SSWX->addSurfaces(SList);

  return;
}


void
sswConstruct::writeHelp(std::ostream& OX) 
  /*!
    Write out help
    \param OX :: output stream
  */
{
  OX<<"SSW tally :\n"
    "Format :: keyWord components \n"
    " -- object ObjectName linkPt \n"
    " -- surfMap ObjectName surfName [index] \n";
  
  return;
}
  

}  // NAMESPACE tallySystem
