/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/flukaTallyBuilder.cxx
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
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "support.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "MainProcess.h"
#include "inputParam.h"

#include "userBinConstruct.h"
#include "userDumpConstruct.h"
#include "flukaTallyBuilder.h"

namespace flukaSystem
{

void
tallySelection(SimFLUKA& System,
	       const mainSystem::inputParam& IParam)
  /*!
    An amalgumation of values to determine what sort of tallies to put
    in the system.
    \param System :: SimFLUKA to add tallies
    \param IParam :: Input Parameters
  */
{
  ELog::RegMethod RegA("flukaTallyBuilder","tallySelection(basic)");

  for(size_t i=0;i<IParam.setCnt("tally");i++)
    {
      const std::string TType=
	IParam.getValue<std::string>("tally",i,0);
      
      const size_t NItems=IParam.itemCnt("tally",i);
      const std::string HType=(NItems>1) ?
	IParam.getValue<std::string>("tally",i,1) : "help";
      
      if (TType=="help" || TType=="?")
	helpTallyType(HType);
      
      else if (TType=="mesh")
	userBinConstruct::processMesh(System,IParam,i);
      else if (TType=="dump")
	userDumpConstruct::processDump(System,IParam,i);
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
  ELog::RegMethod("TallyConstructor","helpTallyType");

  if (HType=="mesh")
    {}
  else
    {
      ELog::EM<<"Tally Types:\n\n";
      ELog::EM<<"-- mesh : \n";
      ELog::EM<<"-- dump : \n";
    }
  
  ELog::EM<<ELog::endBasic;
  return;
}


} // NAMESPACE flukaSystem
