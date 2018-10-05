/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaTally/userDumpConstruct.cxx
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
#include "Triple.h"
#include "support.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LinkSupport.h"
#include "inputParam.h"

#include "SimFLUKA.h"
#include "particleConv.h"
#include "TallySelector.h"
#include "meshConstruct.h"
#include "flukaTally.h"
#include "userDump.h"
#include "userDumpConstruct.h" 


namespace flukaSystem
{

void 
userDumpConstruct::createTally(SimFLUKA& System,
			       const int dType,
			       const int fortranTape,
			       const std::string& outFile)
  /*!
    An amalgamation of values to determine what sort of mesh to put
    in the system.
    \param System :: SimFLUKA to add tallies
    \param dType :: dump type
    \param fortranTape :: output stream
  */
{
  ELog::RegMethod RegA("userDumpConstruct","createTally");

  userDump UD(fortranTape);
  UD.setDumpType(dType);
  UD.setOutName(outFile);
  System.addTally(UD);

  return;
}

int
userDumpConstruct::convertTallyType(const std::string& DT)
  /*!
    Construct to convert words into an index
    \param DT :: name of output type
    \return particle name [upper case] or string of number
  */
{
  ELog::RegMethod RegA("userDumpConstruct","convertTallyType");

  const static std::map<std::string,int> DMap(
    {
      {"source",1},
      {"trajectory",2},
      {"local",3},
      {"continuous",4},
      {"sourceLoss",5},
      {"trajLoss",6},
      {"user",7}
    });
    
  std::map<std::string,int>::const_iterator mc=DMap.find(DT);
  if (mc==DMap.end())
    throw ColErr::InContainerError<std::string>(DT,"dumptype string");

  return mc->second;
}


void
userDumpConstruct::processDump(SimFLUKA& System,
			       const mainSystem::inputParam& IParam,
			       const size_t Index) 
/*!
    Add mesh tally (s) as needed
    \param System :: SimMCNP to add tallies
    \param IParam :: Main input parameters
    \param Index :: index of the -T card
  */
{
  ELog::RegMethod RegA("userDumpConstruct","processDump");
  
  const std::string tallyType=
    IParam.getValueError<std::string>("tally",Index,1,"tallyType");
  const int dumpIndex=
    userDumpConstruct::convertTallyType(tallyType);

  // This needs to be more sophisticated
  const int nextId=System.getNextFTape();
  
  const std::string OutFile=
    IParam.getDefValue<std::string>("dump","tally",Index,2); 

  userDumpConstruct::createTally(System,dumpIndex,nextId,OutFile);
  
  return;      
}
  
  
void
userDumpConstruct::writeHelp(std::ostream& OX) 
  /*!
    Write out help
    \param OX :: Output stream
  */
{
  OX<<
    "recordType filename \n"
    "  --recordType avaiable:\n"
    "    source : trajectory : local : continuous\n"
    "    sourceLoss : trajLoss : user";

  return;
}

}  // NAMESPACE flukaSystem
