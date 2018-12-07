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
#include "radDecay.h"
#include "userRadDecayConstruct.h" 


namespace flukaSystem
{

void
userRadDecayConstruct::processRadDecay(SimFLUKA& System,
				       const mainSystem::inputParam& IParam,
				       const size_t Index) 
  /*!
    Add radiaoactive tally (s) as needed
    - Input:
    -- active time flux :: time and flux for activation in particles/sec
    -- decay [time1 time2] :: time bins [-ve for addition of time] 

    \note SUPPORTS ONLY ONE DECAY TIME AT CURRENT

    \param System :: SimFLUKA to add tallies
    \param IParam :: Main input parameters
    \param Index :: index of the -T card
  */
{
  ELog::RegMethod RegA("userRadDecayConstruct","processRadDecay");

  
  std::map<size_t,std::string> tallyMap;

  radDecay* RadPtr=System.getRadDecay();
  
  const std::string keyName=
    IParam.getValueError<std::string>("tally",Index,1,"tally:keyName");

  if (keyName=="decay")
    {
      const double decayTime=
	IParam.getValueError<double>("tally",Index,2,"tally:decayTime");
      RadPtr->setDecayTime({decayTime});
    }
  else if (keyName=="activeFlux")
    {
      const double activeFlux=
	IParam.getValueError<double>("tally",Index,2,"tally:activeFlux");
      RadPtr->setIradFlux(activeFlux);
    }
  else if (keyName=="activeTime")
    {
      std::vector<double> ATime;
      const size_t NItems=IParam.itemCnt("tally",Index);
      for(size_t i=2;i<NItems;i++)
	{
	  const double activeTime=
	    IParam.getValueError<double>("tally",Index,i,"tally:activeTime");
	  ATime.push_back(activeTime);
	}
      RadPtr->setIradTime(ATime);
    }
  else if (keyName=="NR")
    {
      const size_t NR=
	IParam.getValueError<size_t>("tally",Index,2,"tally:NReplica");
      
      RadPtr->setNReplica(NR);
    }
  else if (keyName=="tallyName")
    {
      const size_t timeIndex=
	IParam.getValueError<size_t>("tally",Index,2,"tally:timeIndex");
      const std::string tallyName=
	IParam.getValueError<std::string>("tally",Index,3,"tally:tally-name");
      
      RadPtr->addDetectors(tallyName,timeIndex);
    }
  else
    throw ColErr::InContainerError<std::string>(keyName," RadDecy type");
  
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
