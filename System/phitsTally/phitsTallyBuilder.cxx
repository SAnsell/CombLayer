/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsTally/phitsTallyBuilder.cxx
 *
 * Copyright (c) 2004-2025 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimPHITS.h"

#include "tgshowConstruct.h"
#include "tcrossConstruct.h"
#include "ttrackMeshConstruct.h"
#include "phitsTallyBuilder.h"

namespace phitsSystem
{

void
tallySelection(SimPHITS& System,
	       const mainSystem::inputParam& IParam)
  /*!
    An amalgumation of values to determine what sort of tallies to put
    in the system.
    \param System :: SimPHITS to add tallies
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("phitsTallyBuilder","tallySelection(basic)");
  
  System.populateCells();
  System.createObjSurfMap();

  for(size_t i=0;i<IParam.setCnt("tally");i++)
    {      
      const size_t NItems=IParam.itemCnt("tally",i);
      if (NItems==0)
	ELog::EM<<"-T needs arguments. Call '-T help' for help."
		<< ELog::endErr;

      const std::string tallyName=IParam.getValue<std::string>("tally",i,0);
      const std::string TType = (NItems > 1) ?
	IParam.getValue<std::string>("tally",i,1) : "";

      if (tallyName=="help" || tallyName=="?")
	helpTallyType(TType);
      else if (TType=="gshow")
	tgshowConstruct::processMesh(System,IParam,i);
      else if (TType=="surface")
	tcrossConstruct::processSurface(System,IParam,i);
      else if (TType=="mesh")
	ttrackMeshConstruct::processMesh(System,IParam,i);
      else
	ELog::EM<<"Unable to understand tally type :"<<TType<<ELog::endErr;
    }

  //if (IParam.flag("Txml"))
    //   tallySystem::addXMLtally(System,IParam.getValue<std::string>("Txml"));
      
  return;
}

void  
helpTallyType(const std::string& HType) 
  /*!
    Simple help for types
    \param HType :: specialization if present that help is required for
  */
{
  ELog::RegMethod("phitsTallyBuilder[F]","helpTallyType");

  if (HType=="mesh")
    {}
  else
    {
      ELog::EM<<"Tally Types:\n\n";
      ELog::EM<<"-- gshow : \n";
      ELog::EM<<"-- surface : \n";
      ELog::EM<<"-- mesh : \n";
      ELog::EM<<"-- dump : \n";
    }
  
  ELog::EM<<ELog::endBasic;
  return;
}


} // NAMESPACE phitsSystem
