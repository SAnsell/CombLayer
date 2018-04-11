/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/flukaGenParticle.cxx
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
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "particleConv.h"
#include "flukaGenParticle.h"

// mcnpChar : mcnpI : fluka : flukaI : phits : phitsI : mcplNumber : nucleons
flukaGenParticle::flukaGenParticle() : 
  flukaGen({
      {"ALL-PART",208}
    })
  /*!
    Constructor
  */
{}

const flukaGenParticle&
flukaGenParticle::Instance()
  /*!
    Singleton this
    \return flukaGenParticle object
   */
{
  static flukaGenParticle MR;
  return MR;
}

int 
flukaGenParticle::flukaITYP(const std::string& particleName) const
  /*
    Accessor to mcnpITYP number by fluka/phits/mcnp name
    \param particleName :: full name
    \return flukaITYP
  */
{
  std::map<std::string,int>::const_iterator mc=
    flukaGen.find(particleName);
  if (mc!=flukaGen.end())
    return mc->second;
  
  return particleConv::Instance().flukaITYP(particleName);
}

bool
flukaGenParticle::hasName(const std::string& particleName) const
  /*!
    See if we have the name in the data base
    \param particleName to search
  */
{
  if (flukaGen.find(particleName)!=flukaGen.end())
    return 1;

  return particleConv::Instance().hasName(particleName);
}

const std::string&
flukaGenParticle::nameToFLUKA(const std::string& particleName) const
  /*!
    Get FLUKA name base on particle name
    \param particleName :: generic name to find
    \return phits name
  */
{
  ELog::RegMethod RegA("flukaGenParticle","nameToFLUKA");

  const std::string PN=StrFunc::toUpperString(particleName);
  std::map<std::string,int>::const_iterator mc=
    flukaGen.find(particleName);
  if (mc!=flukaGen.end())
    return mc->first;
  
  return particleConv::Instance().nameToFLUKA(PN);
}
 
