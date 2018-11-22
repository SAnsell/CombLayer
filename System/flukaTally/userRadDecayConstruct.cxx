/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaTally/userRadDecayConstruct.cxx
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
#include "userRadDecay.h"
#include "userRadDecayConstruct.h" 


namespace flukaSystem
{



void 
userRadDecayConstruct::createTally
   (SimFLUKA& System,
    const double activeTime,const double activeFlux,
    const double decayTime,
    const std::map<size_t,std::string>& tallyMap)
  /*!
    An amalgamation of values to determine what sort of mesh to put
    in the system.
    \param System :: SimFLUKA to add tallies
    \param activeTime :: Time to have main beam on [sec]
    \param activeFlux :: number of initial particles / sec
    \param decayTime :: Time for decay [sec]
  */
{
  ELog::RegMethod RegA("userRadDecayConstruct","createTally");

  userRadDecay UD();
  UD.setActiveTime(activeFlux,{activeTime});
  UD.setDecayTime({decayTime});
  
  System.addTally(UD);

  return;
}


void
userRadDecayConstruct::processBDX(SimFLUKA& System,
			     const mainSystem::inputParam& IParam,
			     const size_t Index) 
  /*!
    Add BDX tally (s) as needed
    - Input:
    -- particle FixedComp index
    -- particle cellA  cellB
    -- particle SurfMap name
    \param System :: SimFLUKA to add tallies
    \param IParam :: Main input parameters
    \param Index :: index of the -T card
  */
{
  ELog::RegMethod RegA("userRadDecayConstruct","processBdx");

  
  const std::string particleType=
    IParam.getValueError<std::string>("tally",Index,1,"tally:ParticleType");
  const std::string FCname=
    IParam.getValueError<std::string>("tally",Index,2,"tally:Object/Cell");
  const std::string FCindex=
    IParam.getValueError<std::string>("tally",Index,3,"tally:linkPt/Cell");

  size_t itemIndex(4);
  int cellA(0);
  int cellB(0);
  if (
      (!StrFunc::convert(FCname,cellA) ||
       !StrFunc::convert(FCindex,cellB) ||
       !checkLinkCells(System,cellA,cellB) ) &&
      
      !constructLinkRegion(System,FCname,FCindex,cellA,cellB)
      )
    
    {
      // special class because must give regions
      itemIndex+=2;
      const size_t regionIndexA=IParam.getDefValue(0,"tally",Index,4);
      const size_t regionIndexB=IParam.getDefValue(0,"tally",Index,5);

      if (!constructSurfRegion(System,FCname,FCindex,
			       regionIndexA,regionIndexB,cellA,cellB))
	throw ColErr::InContainerError<std::string>
	  (FCname+":"+FCindex,"No connecting surface on regions");
    }
  
  ELog::EM<<"Regions connected from "<<cellA<<" "<<cellB<<ELog::endDiag;  

  // This needs to be more sophisticated
  const int nextId=System.getNextFTape();
  
  const double EA=IParam.getDefValue<double>(1e-9,"tally",Index,itemIndex++);
  const double EB=IParam.getDefValue<double>(1000,"tally",Index,itemIndex++);
  const size_t NE=IParam.getDefValue<size_t>(200,"tally",Index,itemIndex++); 

  const double AA=IParam.getDefValue<double>(0.0,"tally",Index,itemIndex++);
  const double AB=IParam.getDefValue<double>(2*M_PI,"tally",Index,itemIndex++);
  const size_t NA=IParam.getDefValue<size_t>(1,"tally",Index,itemIndex++); 

  
  userRadDecayConstruct::createTally(System,particleType,nextId,
				cellA,cellB,
				1,EA,EB,NE,
				0,AA,AB,NA);
  
  return;      
}  
  
void
userRadDecayConstruct::writeHelp(std::ostream& OX) 
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
