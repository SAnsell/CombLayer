/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsTally/phitsTallyModification.cxx
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
#include "OutputLog.h"
#include "Vec3D.h"

#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimPHITS.h"
#include "eType.h"
#include "aType.h"
#include "phitsTally.h"


namespace phitsSystem
{

std::set<phitsTally*>
getActiveTally(SimPHITS& Sim,const std::string& tName)
  /*!
    Get a set of points to matching tallys
    \param Sim :: Simulation model
    \param tName :: tally name [or wild card component]
    \return Points to tallys
   */
{
  ELog::RegMethod RegA("phitsTallyModification[F]","getActiveTally");

  std::set<phitsTally*> Out;
  SimPHITS::PTallyTYPE& tmap=Sim.getTallyMap();

  // map<string,tally>
  for(auto[ kName , tallyPtr ] : tmap)
    {
      std::string KN(kName);
      const size_t knSize(KN.size());

      if (!tName.empty() && tName.size()<knSize &&
	  tName.back()=='*')
	{
	  KN.erase(tName.size(),std::string::npos);
	  KN.back()='*';
	}
      if (KN==tName)
	Out.insert(tallyPtr);
    }
  if (Out.empty())
    throw ColErr::InContainerError<std::string>
      (tName,"Tally modification type not present:");

  return Out;
}
  
int
setBinaryOutput(SimPHITS& Sim,const std::string& tName) 
  /*!
    Set the binary file
    \param Sim :: System to access tally tables
    \param tNumber :: Tally number [0 for all]
    \return tally number [0 on fail]
  */
{
  ELog::RegMethod RegA("phitsTallyModificaiton[F]","setBinaryOutput");

  const std::set<phitsTally*> ATallySet=
    getActiveTally(Sim,tName);
  
  for(phitsTally* mc: ATallySet)
    mc->setBinary();

  return 1;
}
 
int
setParticle(SimPHITS& Sim,const std::string& tName,
	    const std::string& particle)
  /*!
    Get the last tally point based on the tallynumber
    \param Sim :: System to access tally tables
    \param tName :: Tally number [0 for all]
    \param particle :: auxillary particle 
    \return tally number [0 on fail]
  */
{
  ELog::RegMethod RegA("phitsTallyModificaiton[F]","setParticle");

  const std::set<phitsTally*> ATallySet=
    getActiveTally(Sim,tName);

  for(phitsTally* mc: ATallySet)
    mc->setParticle(particle);

  return static_cast<int>(ATallySet.size());
}
  

int
setEnergy(SimPHITS& Sim,const std::string& tName,
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
  ELog::RegMethod RegA("phitsTallyModificaiton[F]","setEnergy");

  const std::set<phitsTally*> ATallySet=
    getActiveTally(Sim,tName);

  const std::string eDistType((logFlag) ? "Log" : "Linear");
  const eType ET(eDistType,NA,EA,EB);
  for(phitsTally* mc: ATallySet)
    mc->setEnergy(ET);

  return static_cast<int>(ATallySet.size());
}

int
setAngle(SimPHITS& Sim,const std::string& tName,
	 const double aMin,const double aMax,
	 const size_t NA)
/*!
  Set the angle base of a group of tallies
    \param Sim :: System to access tally tables
    \param tName :: Tally number [0 for all]
    \param AA :: start energy
    \param AB :: end enegy
    \param NA :: number of points
    \return tally number [0 on fail]
  */
{
  ELog::RegMethod RegA("phitsTallyModificaiton[F]","setAngle");

  const std::set<phitsTally*> ATallySet=
    getActiveTally(Sim,tName);

  const std::string angleUnit=
    (aMin>=-1.0 && aMin<=1.0) ? "Cos" : "Deg";
  const aType AT(angleUnit,NA,aMin,aMax);  
  for(phitsTally* mc: ATallySet)
    mc->setAngle(AT);

  return static_cast<int>(ATallySet.size());
}

int
setVTKout(SimPHITS& Sim,const std::string& tName)
/*!
    Get the last tally point based on the tallynumber
    \param Sim :: System to access tally tables
    \param tName :: Tally number [0 for all]
    \return tally number [0 on fail]
  */
{
  ELog::RegMethod RegA("phitsTallyModificaiton[F]","setVTKout");

  const std::set<phitsTally*> ATallySet=
    getActiveTally(Sim,tName);

  for(phitsTally* mc: ATallySet)
    mc->setVTKout();

  return static_cast<int>(ATallySet.size());
}

}  // NAMESPACE phitsSystem

