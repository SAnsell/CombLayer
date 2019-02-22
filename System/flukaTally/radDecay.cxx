/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaTally/radDecay.cxx
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <iterator>
#include <array>
#include <memory>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "writeSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Mesh3D.h"

#include "flukaTally.h"
#include "flukaTallyModification.h"
#include "radDecay.h"

namespace flukaSystem
{

radDecay::radDecay() :
  nReplica(10),gammaTransCut(1.0),
  iradFlux(1.0)
  /*!
    Constructor
  */
{}

radDecay::radDecay(const radDecay& A) : 
  nReplica(A.nReplica),biasCard(A.biasCard),
  gammaTransCut(A.gammaTransCut),
  iradTime(A.iradTime),decayTime(A.decayTime),
  detectors(A.detectors)
  /*!
    Copy constructor
    \param A :: radDecay to copy
  */
{}

radDecay&
radDecay::operator=(const radDecay& A)
  /*!
    Assignment operator
    \param A :: radDecay to copy
    \return *this
  */
{
  if (this!=&A)
    {
      nReplica=A.nReplica;
      biasCard=A.biasCard;
      gammaTransCut=A.gammaTransCut;
      iradFlux=A.iradFlux;
      iradTime=A.iradTime;
      decayTime=A.decayTime;
      detectors=A.detectors;
    }
  return *this;
}

radDecay*
radDecay::clone() const
  /*!
    Clone object
    \return new (this)
  */
{
  return new radDecay(*this);
}

radDecay::~radDecay()
  /*!
    Destructor
  */
{}

void
radDecay::setIradTime(const std::vector<double>& timeVec)
  /*!
    Sets the decay time based on a given flux of primary particle
    for alternating Value : 0 : Value : 0 etc..
    \param Flux :: Particle flux
    \param timeVec :: vector of times 
  */
{
  ELog::RegMethod RegA("radDecay","setIradTime");

  int current(1);
  for(const double DT : timeVec)
    {
      iradTime.push_back(std::pair<double,double>(DT,iradFlux * current));
      current= 1-current;
    }
  
  return;
}


void
radDecay::setDecayTime(const std::vector<double>& timeVec)
  /*!
    Sets the decay time based on a given flux of primary particle
    for alternating Value : 0 : Value : 0 etc..
    \param Flux :: Particle flux
    \param timeVec :: vector of times 
  */
{
  ELog::RegMethod RegA("radDecay","setDecayTime");

  double prevTime(0);
  for(const double TN : timeVec)
    {
      prevTime = (TN<0) ? prevTime-TN : TN;
      decayTime.push_back(prevTime);
    }
  // need to be in order
  std::sort(decayTime.begin(),decayTime.end());
  return;
}

void
radDecay::addDetectors(const std::string& TName,
		       const size_t Index)
  /*!
    Emplace a detector to use 
    \param TName :: Tally name [can use wildcard]
    \param Index :: Index of decay time
   */
{
  detectors.emplace(TName,Index);
  return;
}
  
void
radDecay::write(const SimFLUKA& System,std::ostream& OX) const
  /*!
    Write out decay information			
    \param System :: Simulation for additional tallies
    \param OX :: Output stream
   */
{
  if (!decayTime.empty())
    {
      std::ostringstream cx;
      
      cx<<"RADDECAY 1.0 1.0 "<<nReplica<<" -  - 1.0 ";
      StrFunc::writeFLUKA(cx.str(),OX);
      
      size_t index(0);
      cx.str("");
      cx<<"IRRPROFI ";
      if (iradTime.empty())
	ELog::EM<<"Empty IRRAD "<<ELog::endDiag;
      for(const std::pair<double,double> IR : iradTime)
	{
	  cx<<IR.first<<" "<<IR.second*iradFlux;
	  index++;
	  if ((index % 3)==0)
	    {
	      StrFunc::writeFLUKA(cx.str(),OX);
	      cx.str("");
	      cx<<"IRRPROFI ";
	    }
	}
      if (index % 3)
	StrFunc::writeFLUKA(cx.str(),OX);
      
      
      index=0;
      cx.str("");
      cx<<"DCYTIMES ";
      for(const double DT : decayTime)
	{
	  cx<<DT<<" ";
	  index++;
	  if ((index % 6)==0)
	    {
	      StrFunc::writeFLUKA(cx.str(),OX);
	      cx.str("");
	      cx<<"DCYTIMES ";
	    }
	}
      if (index % 6)
	StrFunc::writeFLUKA(cx.str(),OX);

      for(const std::pair<std::string,size_t>& TN : detectors)
	{
	  const std::set<flukaTally*> activeTally=
	    flukaSystem::getActiveTally(System,TN.first);
	  for(const flukaTally* FPtr : activeTally)
	    {
	      cx.str("");
	      cx<<"DCYSCORE "<<TN.second<<" - - "
		<<FPtr->getKeyName()<<" "
		<<FPtr->getKeyName()<<" 1.0 "
		<<FPtr->getType();
	      StrFunc::writeFLUKA(cx.str(),OX);	      
	    }
	}
	  
      
      
    }
  return;
}

}  // NAMESPACE flukaSystem

