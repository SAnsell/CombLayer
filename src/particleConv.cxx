/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/particleConv.cxx
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
#include "particleConv.h"

pName::pName(const std::string& mcnpN,const int mI,
	     const std::string& flukaN,const int fI,
	     const std::string& phitsN,const int pI,
	     const int mcplN,const int N) :
  mcnpName(mcnpN),mcnpITYP(mI),
  flukaName(flukaN),flukaITYP(fI),
  phitsName(phitsN),phitsITYP(pI),
  mcplNumber(mcplN),nucleon(N)
  /*
    Constructor 
    \param mcnpN :: MCNP Name
    \param mI :: MCNP itype number
    \param flukaN :: FLUKA name
    \param fI :: FLUKA itype number
    \param phitsN :: PHITS name
    \param pI :: PHITS ityp number
    \param mcplN :: kf/mcpl number
    \param N :: nucleon number
  */
{}

pName::pName(const pName& A) :
  mcnpName(A.mcnpName),mcnpITYP(A.mcnpITYP),
  flukaName(A.flukaName),flukaITYP(A.flukaITYP),
  phitsName(A.phitsName),phitsITYP(A.phitsITYP),
  mcplNumber(A.mcplNumber),nucleon(A.nucleon)
  /*
    Copy Constructor 
    \param A :: Object to copy
  */
{}

// mcnpChar : mcnpI : fluka : flukaI : phits : phitsI : mcplNumber : nucleons
particleConv::particleConv() : 
  particleVec({
  {"h", 9,  "proton",     1,  "proton",     1,    2212,       1},
  {"n", 1,  "neutron",    8,  "neutron",    2,    2112,       1},   
  {"/", 20, "pion+",     13,  "pion+",      3,     211,       1},
  {"z", 21, "pizero",    23,  "pion0",      4,     111,       1},
  {"*", 11,  "pion-",    14,  "pion-",      5,    -211,       1},
  {"!", 16, "muon+",     10,  "muon+",      6,     -13,       1},
  {"|", 4,  "muon-",     11,  "muon-",      7,      13,       1},
  {"k", 22, "kaon+",     15,  "kaon+",      8,     321,       1},
  {"%", 23, "kaonzero",  24,  "kaon0",      9,     311,       1},  // k0 version
  {"^", 24, "kaonlong",  12,  "kaon0",      9,     130,       1},  // long version
  {"?", 36, "kaon-",     16,  "kaon-",     10,    -321,       1},
  {"e", 3,  "electron",   3,  "electron",  11,      11,       1},
  {"f", 8,  "positron",   4,  "positron",  13,     -11,       1},
  {"p", 2,  "photon",     7,  "photon",    14,      22,       1},
  {"d", 31, "deuteron",  -3,  "deuteron",  15, 1000002,       2},
  {"t", 32, "triton",    -4,  "triton",    16, 1000003,       3},
  {"s", 33, "3-helium",  -5,   "3he",      17, 2000003,       3},
  {"a", 34, "4-helium",  -6,  "alpha",     18, 2000004,       4},
  {"g", 19, "antiproton", 2,  "antiproton",11,   -2212,       1}       // no direct phits
    })
  /*!
    Constructor
  */
{
  for(size_t index=0;index<particleVec.size();index++)
    {
      const pName& PN=particleVec[index];
      mcnpIndex.emplace(PN.mcnpName,index);
      flukaIndex.emplace(PN.flukaName,index);
      phitsIndex.emplace(PN.phitsName,index);
    }
}

const particleConv&
particleConv::Instance()
  /*!
    Singleton this
    \return particleConv object
   */
{
  static particleConv MR;
  return MR;
}

size_t
particleConv::getMCNPIndex(const int ID) const
  /*!
    Get the PName index item from the system
    \param ID :: Particle id
    \return index+1 
  */
{
  std::vector<pName>::const_iterator vc=
    std::find_if(particleVec.begin(),particleVec.end(),
	    [&ID](const pName& PN) -> bool
		 {
		   return (ID == PN.mcnpITYP);
		 } );

  if (vc==particleVec.end()) return 0;
  return static_cast<size_t>(vc-particleVec.begin())+1;
}

size_t
particleConv::getPHITSIndex(const int ID) const
  /*!
    Get the PName index item from the system
    \param ID :: Particle id
    \return index+1 
  */
{
  std::vector<pName>::const_iterator vc=
    std::find_if(particleVec.begin(),particleVec.end(),
	    [&ID](const pName& PN) -> bool
		 {
		   return (ID == PN.phitsITYP);
		 } );
  if (vc==particleVec.end()) return 0;
  return static_cast<size_t>(vc-particleVec.begin())+1;
}

size_t
particleConv::getFLUKAIndex(const int ID) const
  /*!
    Get the PName index item from the system
    \param ID :: Particle id
    \return index+1 
  */
{
  std::vector<pName>::const_iterator vc=
    std::find_if(particleVec.begin(),particleVec.end(),
	    [&ID](const pName& PN) -> bool
		 {
		   return (ID == PN.flukaITYP);
		 } );
  if (vc==particleVec.end()) return 0;
  return static_cast<size_t>(vc-particleVec.begin())+1;
}

size_t
particleConv::getMCNPIndex(const std::string& MC) const
  /*!
    Get the PName index item from the system
    \param MC :: Particle char [mcnp]
    \return index+1 
  */
{
  std::map<std::string,size_t>::const_iterator mc;
  mc=mcnpIndex.find(MC);
  if (mc == mcnpIndex.end()) return 0;
  return mc->second+1;
}

size_t
particleConv::getFLUKAIndex(const std::string& MC) const
  /*!
    Get the PName index item from the system
    \param MC :: Particle [fluka]
    \return index+1 
  */
{
  std::map<std::string,size_t>::const_iterator mc;
  mc=flukaIndex.find(MC);
  if (mc == flukaIndex.end()) return 0;
  return mc->second+1;
}

size_t
particleConv::getPHITSIndex(const std::string& MC) const
  /*!
    Get the PName index item from the system
    \param MC :: Particle char [mcnp]
    \return index+1 
  */
{
  std::map<std::string,size_t>::const_iterator mc;
  mc=phitsIndex.find(MC);
  if (mc == phitsIndex.end()) return 0;
  return mc->second+1;
}

 // MCNP CHAR:
const pName&
particleConv::getMCNPpItem(const std::string& MC) const
  /*!
    Get  the PName item from the system
    \param MC :: Particle char [mcnp]
    \return pName found
  */
{
  std::map<std::string,size_t>::const_iterator mc;
  mc=mcnpIndex.find(MC);
  if (mc == mcnpIndex.end())
    throw ColErr::InContainerError<std::string>
      (MC,"MCNP particle not in particleDataBase");
  return particleVec[mc->second];
}


const pName&
particleConv::getPHITSpItem(const std::string& MC) const
  /*!
    Get  the PName item from the system
    \param MC :: Particle char [phits]
    \return pName found
  */
{
  std::map<std::string,size_t>::const_iterator mc;
  mc=phitsIndex.find(MC);
  if (mc == phitsIndex.end())
    throw ColErr::InContainerError<std::string>
      (MC,"PHITS particle not in particleDataBase");
  return particleVec[mc->second];
}

const pName&
particleConv::getFLUKApItem(const std::string& MC) const
  /*!
    Get  the PName item from the system
    \param MC :: Particle char [fluka]
    \return pName found
  */
{
  std::map<std::string,size_t>::const_iterator mc;
  mc=flukaIndex.find(MC);
  if (mc == flukaIndex.end())
    throw ColErr::InContainerError<std::string>
      (MC,"FLUKA particle not in particleDataBase");
  return particleVec[mc->second];
}

size_t
particleConv::getNameIndex(const std::string& particleName) const
  /*!
    Access a particle name from any name
    \param particleName :: Particle to look for
    \return index+1 / 0 on failuire
  */
{
  size_t index=getMCNPIndex(particleName);
  if (!index)
    index=getFLUKAIndex(particleName);
  if (!index)
    index=getPHITSIndex(particleName);
  return index;
}
  


const pName&
particleConv::getNamePItem(const std::string& particleName) const
  /*!
    Access a particle name from any name
    \param particleName :: Particle to look for
    \return pName found
  */
{
  const size_t index=getNameIndex(particleName);
  if (!index)
    throw ColErr::InContainerError<std::string>
      (particleName,"Particle not in particleDataBase");

  return particleVec[index-1];
}

int 
particleConv::mcnpITYP(const std::string& particleName) const
  /*
    Accessor to mcnpITYP number by fluka/phits/mcnp name
    \param particleName :: full name
    \return mcnpITYP
  */
{
  const pName& PN=getNamePItem(particleName);
  return PN.mcnpITYP;
}

int 
particleConv::flukaITYP(const std::string& particleName) const
  /*
    Accessor to mcnpITYP number by fluka/phits/mcnp name
    \param particleName :: full name
    \return mcnpITYP
  */
{
  const pName& PN=getNamePItem(particleName);
  return PN.flukaITYP;
}

int 
particleConv::phitsITYP(const std::string& particleName) const
  /*
    Accessor to mcnpITYP number by fluka/phits/mcnp name
    \param particleName :: full name
    \return mcnpITYP
  */
{
  const pName& PN=getNamePItem(particleName);
  return PN.phitsITYP;
}


int 
particleConv::nucleon(const std::string& particleName) const
  /*
    Accessor to nucleon number by name
    \param particleName :: full name
    \return nucleon count
  */
{
  const pName& PN=getFLUKApItem(particleName);
  return PN.nucleon;
}

bool
particleConv::hasName(const std::string& particleName) const
  /*!
    See if we have the name in the data base
    \param particleName to search
  */
{
  const size_t index=getNameIndex(particleName);
  return (index) ? 1 : 0;
}

const std::string&
particleConv::mcnpToFLUKA(const int ID) const
  /*!
    Get PHITS name base on mcnp id number
    \param ID :: MCNP ID
  */
{
  const size_t index=getFLUKAIndex(ID);
  if (!index)
    throw ColErr::InContainerError<int>(ID,"ID no in particleDataBase");
  return particleVec[index-1].flukaName;
}
 

const std::string&
particleConv::nameToPHITS(const std::string& particleName) const
  /*!
    Get PHITS name base on particle name
    \param particleName :: generic name to find
    \return phits name
  */
{
  const pName& PN = getNamePItem(particleName);
  return PN.phitsName;
}

const std::string&
particleConv::nameToFLUKA(const std::string& particleName) const
  /*!
    Get FLUKA name base on particle name
    \param particleName :: generic name to find
    \return phits name
  */
{
  const pName& PN = getNamePItem(particleName);
  return PN.flukaName;
}
 
