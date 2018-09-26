/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsTally/phitsTallyModification.cxx
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
#include "Simulation.h"
#include "SimPHITS.h"
#include "phitsTally.h"

#include "phitsTallyModification.h"

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

  for(SimPHITS::PTallyTYPE::value_type& mc : tmap)
    {
      std::string KN=mc.second->getKeyName();
      if (tName.back()=='*')
	{
	  // method to make KN ==> stuff*
	  KN.erase(tName.size(),std::string::npos);
	  KN.back()='*';
	}
      if (KN==tName)
	{
	  Out.insert(mc.second);
	}
    }
  return Out;
}
int
setParticleType(SimPHITS& Sim,const int tNumber,
                const std::string& partType) 
  /*!
    Get the last tally point based on the tallynumber
    \param Sim :: System to access tally tables
    \param tNumber :: Tally number [0 for all]
    \param partType :: particle type
    \return tally number [0 on fail]
  */
{
  ELog::RegMethod RegA("phitsTallyModificaiton[F]","setParticleType");

  
  SimPHITS::PTallyTYPE& tmap=Sim.getTallyMap();
  int fnum(0);
  for(SimPHITS::PTallyTYPE::value_type& mc : tmap)
    {
      if (tNumber==0 || mc.first==tNumber ||
          (tNumber<0 && (mc.first % 10) == -tNumber))
	{
	  //          mc.second->setParticles(partType);
          fnum++;
	}
    }
  return fnum;
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

  for(phitsTally* mc: ATallySet)
    mc->setEnergy(logFlag,EA,EB,NA);

  return static_cast<int>(ATallySet.size());
}

int
setAngle(SimPHITS& Sim,const std::string& tName,
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
  ELog::RegMethod RegA("phitsTallyModificaiton[F]","setEnergy");

  const std::set<phitsTally*> ATallySet=
    getActiveTally(Sim,tName);

  for(phitsTally* mc: ATallySet)
    mc->setAngle(logFlag,AA,AB,NA);

  return static_cast<int>(ATallySet.size());
}

}  // NAMESPACE phitsSystem

