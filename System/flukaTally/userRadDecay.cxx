/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaTally/userRadDecay.cxx
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
#include "userRadDecay.h"

namespace flukaSystem
{

userRadDecay::userRadDecay() :
  flukaTally("radDecay",0),
  nReplica(10),gammaTransCut(1.0)
  /*!
    Constructor
    \param outID :: Identity number of tally [fortranOut]
  */
{}

userRadDecay*
userRadDecay::clone() const
  /*!
    Clone object
    \return new (this)
  */
{
  return new userRadDecay(*this);
}

userRadDecay::~userRadDecay()
  /*!
    Destructor
  */
{}

void
userRadDecay::setIradTime(const double Flux,
			  const std::vector<double>& timeVec)
  /*!
    Sets the decay time based on a given flux of primary particle
    for alternating Value : 0 : Value : 0 etc..
    \param Flux :: Particle flux
    \param timeVec :: vector of times 
  */
{
  ELog::RegMethod RegA("userRadDecay","setIradTime");

  int current(1);
  for(const double DT : timeVec)
    {
      iradTime.push_back(DT,Flux * current);
      current= 1-current;
    }
  
  return;
}

void
userRadDecay::setDecayTime(const std::vector<double>& timeVec)
  /*!
    Sets the decay time based on a given flux of primary particle
    for alternating Value : 0 : Value : 0 etc..
    \param Flux :: Particle flux
    \param timeVec :: vector of times 
  */
{
  ELog::RegMethod RegA("userRadDecay","setDecayTime");

  for(const double TN : timeVec)
    {
    }
  decayTime.sort()
  return;
}
  
void
userRadDecay::write(std::ostream& OX) const
  /*!
    Write out decay information
    \param OX :: Output stream
   */
{
  std::ostringstream cx;

  cx<<"RADDECAY 1.0 1.0 "<<nReplica<<" -  - 1.0 ";
  StrFunc::writeFLUKA(cx.str(),OX);

  size_t index(0);
  cx.str("IRRPROFI ");
  for(const std::pair<double,double> IR : iradTime)
    {
      cx<<IR.first<<" "<<IR.second;
      index++;
      if ((index % 3)==0)
	{
	  StrFunc::writeFLUKA(cx.str(),OX);
	  cx.str("IRRPROFI ");
	}
    }
  if (index % 3)
    StrFunc::writeFLUKA(cx.str(),OX);


  index=0;
  cx.str("DCYTIMES ");
  for(const double DT : decayTime)
    {
      cx<<DT<<" ";
      index++;
      if ((index % 6)==0)
	{
	  StrFunc::writeFLUKA(cx.str(),OX);
	  cx.str("DCYTIMES ");
	}
    }
  if (index % 6)
    StrFunc::writeFLUKA(cx.str(),OX);
  
  return;
}

}  // NAMESPACE flukaSystem

