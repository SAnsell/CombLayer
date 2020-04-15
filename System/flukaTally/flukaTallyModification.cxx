/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaTally/flukaTallySelector.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimFLUKA.h"
#include "flukaTally.h"

#include "flukaTallyModification.h"

namespace flukaSystem
{

std::set<flukaTally*>
getActiveTally(const SimFLUKA& Sim,const std::string& tName)
  /*!
    Get a set of points to matching tallys
    \param Sim :: Fluka simulation
    \param tName :: tally name [or wild card component]
    \return Set of tally pointer
   */
{
  ELog::RegMethod RegA("flukaTallyModification[F]","getActiveTally");

  std::set<flukaTally*> Out;
  const SimFLUKA::FTallyTYPE& tmap=Sim.getTallyMap();

	  
  for(const SimFLUKA::FTallyTYPE::value_type& mc : tmap)
    {
      std::string KN=mc.second->getKeyName();
      const size_t knSize(KN.size());

      if (!tName.empty() && tName.size()<knSize &&
	  tName.back()=='*')
	{
	  KN.erase(tName.size(),std::string::npos);
	  KN.back()='*';
	}
      if (KN==tName)
	Out.insert(mc.second);
    }
  if (Out.empty())
    throw ColErr::InContainerError<std::string>
      (tName,"Tally modification type not present:");
  return Out;
}

int
setBinaryOutput(SimFLUKA& Sim,const std::string& tName) 
  /*!
    Set the binary file
    \param Sim :: System to access tally tables
    \param tNumber :: Tally number [0 for all]
    \return tally number [0 on fail]
  */
{
  ELog::RegMethod RegA("flukaTallyModificaiton[F]","setBinaryOutput");

  const std::set<flukaTally*> ATallySet=
    getActiveTally(Sim,tName);

  if (ATallySet.empty())
    throw ColErr::InContainerError<std::string>
      (tName,"Unknown tally");
  
  for(flukaTally* mc: ATallySet)
    mc->setBinary();

  return 1;
}
  

  
int
setDoseType(SimFLUKA& Sim,const std::string& tName,
	    const std::string& particle,
	    const std::string& doseType) 
 /*!
    If the tally is a does-eq type tally then adjust
    the type of dose scoring. Can also set dose scoring
    for a single particle.
    \param Sim :: System to access tally tables
    \param tName :: Tally number [0 for all]
    \param particle :: auxillary particle 
    \param doseType :: Dose type
    \return tally number [0 on fail]
  */
{
  ELog::RegMethod RegA("flukaTallyModificaiton[F]","setDoseType");

  const std::set<flukaTally*> ATallySet=
    getActiveTally(Sim,tName);

  if (ATallySet.empty())
    throw ColErr::InContainerError<std::string>
      (tName,"Unknown tally");
  
  for(flukaTally* mc: ATallySet)
    mc->setDoseType(particle,doseType);

  return static_cast<int>(ATallySet.size());
}

int
setAuxParticle(SimFLUKA& Sim,const std::string& tName,
	       const std::string& particle)
/*!
    Get the last tally point based on the tallynumber
    \param Sim :: System to access tally tables
    \param tName :: Tally number [0 for all]
    \param particle :: auxillary particle 
    \return tally number [0 on fail]
  */
{
  ELog::RegMethod RegA("flukaTallyModificaiton[F]","setAuxParticle");

  const std::set<flukaTally*> ATallySet=
    getActiveTally(Sim,tName);

  for(flukaTally* mc: ATallySet)
    mc->setAuxParticles(particle);

  return static_cast<int>(ATallySet.size());
}

int
setUserName(SimFLUKA& Sim,const std::string& tName,
	    const std::string& userName)
/*!
    Get the last tally point based on the tallynumber
    \param Sim :: System to access tally tables
    \param tName :: Tally number [0 for all]
    \param userName :: Auxillary name
    \return tally number size
  */
{
  ELog::RegMethod RegA("flukaTallyModificaiton[F]","setUserName");

  const std::set<flukaTally*> ATallySet=
    getActiveTally(Sim,tName);

  for(flukaTally* mc: ATallySet)
    mc->setUserName(userName);

  return static_cast<int>(ATallySet.size());
}
  

int
setEnergy(SimFLUKA& Sim,const std::string& tName,
	  const double EA,const double EB,
	  const size_t NA,const bool logFlag)
/*!
    Get the last tally point based on the tallynumber
    \param Sim :: System to access tally tables
    \param tName :: Tally number [0 for all]
    \param EA :: start energy
    \param EB :: end enegy
    \param NA :: number of points
    \param logFlag :: log points
    \return tally number [0 on fail]
  */
{
  ELog::RegMethod RegA("flukaTallyModificaiton[F]","setEnergy");

  const std::set<flukaTally*> ATallySet=
    getActiveTally(Sim,tName);

  for(flukaTally* mc: ATallySet)
    mc->setEnergy(logFlag,EA,EB,NA);

  return static_cast<int>(ATallySet.size());
}

int
setAngle(SimFLUKA& Sim,const std::string& tName,
	 const double AA,const double AB,
	 const size_t NA,const bool logFlag)
/*!
    Get the last tally point based on the tallynumber
    \param Sim :: System to access tally tables
    \param tName :: Tally number [0 for all]
    \param AA :: start energy
    \param AB :: end enegy
    \param NA :: number of points
    \param logFlag :: log points
    \return tally number [0 on fail]
  */
{
  ELog::RegMethod RegA("flukaTallyModificaiton[F]","setEnergy");

  const std::set<flukaTally*> ATallySet=
    getActiveTally(Sim,tName);

  for(flukaTally* mc: ATallySet)
    mc->setAngle(logFlag,AA,AB,NA);

  return static_cast<int>(ATallySet.size());
}

}  // NAMESPACE flukaSystem
