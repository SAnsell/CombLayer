/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaTally/resnucConstruct.cxx
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
#include "surfRegister.h"
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
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"
#include "LinkSupport.h"
#include "inputParam.h"

#include "Object.h"
#include "SimFLUKA.h"
#include "particleConv.h"
#include "flukaGenParticle.h"
#include "TallySelector.h"
#include "flukaTally.h"

#include "resnuclei.h"
#include "resnucConstruct.h" 


namespace flukaSystem
{

void 
resnucConstruct::createTally(SimFLUKA& System,
			     const int fortranTape,
			     const int cellA)

/*!
    An amalgamation of values to determine what sort of mesh to put
    in the system.
    \param System :: SimFLUKA to add tallies
    \param fortranTape :: output stream
    \param CellA :: initial region
  */
{
  ELog::RegMethod RegA("resnucConstruct","createTally");

  resnuclei UD(fortranTape);
  UD.setCell(cellA);
  System.addTally(UD);

  return;
}


void
resnucConstruct::processResNuc(SimFLUKA& System,
			     const mainSystem::inputParam& IParam,
			     const size_t Index) 
  /*!
    Add TRACK tally (s) as needed
    - Input:
    -- FixedComp/CellMap name 
    -- cellA  
    \param System :: SimFLUKA to add tallies
    \param IParam :: Main input parameters
    \param Index :: index of the -T card
  */
{
  ELog::RegMethod RegA("resnucConstruct","processResNuc");

  const std::string objectName=
    IParam.getValueError<std::string>("tally",Index,1,"tally:objectName");

  const std::vector<int> cellVec=
    System.getObjectRange(objectName);

  for(const int CN : cellVec)
    {
      // This needs to be more sophisticated
      const int nextId=System.getNextFTape();
      resnucConstruct::createTally(System,nextId,CN);
    }
  
  return;      
}  
  
void
resnucConstruct::writeHelp(std::ostream& OX) 
  /*!
    Write out help
    \param OX :: Output stream
  */
{
  OX<<
    "resnuc objectName \n";
  return;
}

}  // NAMESPACE flukaSystem
