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

pName::pName(const std::string& MChar,const int mcnpN,
	     const std::string& flukaN,
	     const std::string& phitsN,const int pI,
	     const int mcplN,const int N) :
  mcnpName(MChar),mcnpITYP(mcnpN),flukaName(flukaN),
      phitsName(phitsN),phitsITYP(pI),
      mcplNumber(mcplN),nucleon(N)
  /*
    Constructor 
    \param MChar :: MCNP Name
    \param mcnpN :: MCNP number
    \param flukaN :: FLUKA name
    \param phitsN :: PHITS name
    \param pI :: PHITS ityp number
    \param mcplN :: kf/mcpl number
    \param N :: nucleon number
  */
{}

pName::pName(const pName& A) :
      mcnpName(A.mcnpName),mcnpITYP(A.mcnpITYP),
      flukaName(A.flukaName),phitsName(A.phitsName),
      phitsITYP(A.phitsITYP),mcplNumber(A.mcplNumber),
      nucleon(A.nucleon)
  /*
    Copy Constructor 
    \param A :: Object to copy
  */
{}

// mcnpChar : mcnpI : fluka : phits : phitsI : mcplNumber : nucleons
particleConv::particleConv() : 
  indexLookup{
  {"h",{"h", 9,  "proton",  "proton",   1,  2212,     1}},
  {"n",{"n", 1,  "neutron", "neutron",  2,  2112,     1}},   
  {"/",{"/", 20, "pion+",   "pion+",    3,  211,      1}},
  {"z",{"z", 21, "pion0",   "pion0",    4,  111,      1}},
  {"*",{"*", 1,  "pion-",   "pion-",    5,  -211,     1}},
  {"!",{"!", 16, "muon+",   "muon+",    6,  -13,      1}},
  {"|",{"|", 4,  "muon-",   "muon-",    7,  13,       1}},
  {"k",{"k", 22, "kaon+",   "kaon+",    8,  321,      1}},
  {"%",{"%", 23, "kaon0",   "kaon0",    9,  311,      1}},  // short version
  {"^",{"^", 24, "kaon0",   "kaon0",    9,  311,      1}},  // long version
  {"?",{"?", 36, "kaon-",   "kaon-",    10, -321,     1}},
  {"e",{"e", 3,  "electron","electron", 11, 11,       1}},
  {"f",{"f", 8,  "positron","positron", 13, -11,      1}},
  {"p",{"p", 2,  "photon",  "photon",   14, 22,       1}},
  {"d",{"d", 31, "deuteron","deuteron", 15, 1000002,  2}},
  {"t",{"t", 32, "triton",  "triton",   16, 1000003,  3}},
  {"s",{"s", 33, "3he",     "3he",      17, 2000003,  3}},
  {"a",{"a", 34, "alpha",   "alpha",    18, 2000004,  4}},

   {"g",{"g",19,"antiproton","antiproton", 11, -2212, 1}}       // no direct phits

}
  /*!
    Constructor
  */
{}

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

 // MCNP CHAR:
const pName&
particleConv::getPItem(const std::string& MC) const
  /*!
    Get  the PName item from the system
    \param MC :: Particle char [mcnp]
    \return pName found
  */
{
  std::map<std::string,pName>::const_iterator mc;
  mc=indexLookup.find(MC);
  if (mc == indexLookup.end())
    throw ColErr::InContainerError<std::string>
      (MC,"MCNPname not in particleDataBase");
  return mc->second;
}

  
const pName&
particleConv::getPHITSPItem(const std::string& particleName) const
  /*!
    Get  the PName item from the system
    \param particleName :: PHITS 
    \return pName found
  */
{
  ELog::RegMethod RegA("particleConv","getPHITSItem");
  
  typedef std::map<std::string,pName>  PMAP;
  PMAP::const_iterator mc=
    std::find_if(indexLookup.begin(),indexLookup.end(),
		 [&particleName](const PMAP::value_type& PN) -> bool
		 {
		   return (particleName == PN.second.phitsName);
		 } );

  if (mc==indexLookup.end())
    throw ColErr::InContainerError<std::string>
      (particleName,"ParticleName number not in particle list");

  return mc->second;
}

const pName&
particleConv::getFLUKAPItem(const std::string& particleName) const
  /*!
    Get  the PName item from the system
    \param particleName :: FLUKA name
    \return pName found
  */
{
  ELog::RegMethod RegA("particleConv","getFLUKAItem");
  
  typedef std::map<std::string,pName>  PMAP;
  PMAP::const_iterator mc=
    std::find_if(indexLookup.begin(),indexLookup.end(),
		 [&particleName](const PMAP::value_type& PN) -> bool
		 {
		   return (particleName == PN.second.flukaName);
		 } );

  if (mc==indexLookup.end())
    throw ColErr::InContainerError<std::string>
      (particleName,"ParticleName number not in particle list");

  return mc->second;
}

int
particleConv::hasFlukaName(const std::string& particleName) const
  /*!
    Get  the PName item from the 
    \param particleName :: FLUKA name
    \return true if found
  */
{
  ELog::RegMethod RegA("particleConv","hasFlukaName");
  
  typedef std::map<std::string,pName> PMAP;
  PMAP::const_iterator mc=
    std::find_if(indexLookup.begin(),indexLookup.end(),
		 [&particleName](const PMAP::value_type& PN) -> bool
		 {
		   return (particleName == PN.second.flukaName);
		 } );

  return (mc==indexLookup.end()) ? 0 : 1;
}



const pName&
particleConv::getMCNPitypePItem(const int particleID) const
  /*!
    Get  the PName item from the system
    \param particleID :: mcnpITYPE number
    \return pName found
  */
{
  ELog::RegMethod RegA("particleConv","getMCNPitypePItem");
  
  typedef std::map<std::string,pName>  PMAP;
  PMAP::const_iterator mc=
    std::find_if(indexLookup.begin(),indexLookup.end(),
		 [&particleID](const PMAP::value_type& PN) -> bool
		 {
		   return (particleID == PN.second.mcnpITYP);
		 } );

  if (mc==indexLookup.end())
    throw ColErr::InContainerError<int>
      (particleID,"ParticleID number not in particle list");

  return mc->second;
}
  
const std::string&
particleConv::phitsType(const char MChar) const
  /*
    Accessor to phitsType 
    \param MChar :: MCNP Name
    \return phits named particle
  */
{
  return phitsType(std::string(1,MChar));
}

int 
particleConv::phitsITYP(const char MChar) const
  /*
    Accessor to physIType
    \param MChar :: MCNP Name
    \return phits ityp number
  */
{
  return phitsITYP(std::string(1,MChar));
}

int 
particleConv::nucleon(const char MChar) const
  /*
    Accessor to physIType
    \param MChar :: MCNP Name
    \return phits ityp number
  */
{
  return nucleon(std::string(1,MChar));
}

const std::string&
particleConv::phitsType(const std::string& MC) const
  /*
    Accessor to phitsType 
    \param MC :: MCNP Name
    
  */
{
  const pName& PItem=getPItem(MC);
  return PItem.phitsName;
}

int 
particleConv::phitsITYP(const std::string& MC) const
  /*
    Accessor to phits ITYP
    \param MC :: MCNP Name
    \return ITYP
  */
{ 
  const pName& PItem=getPItem(MC);
  return PItem.phitsITYP;
}

int 
particleConv::nucleon(const std::string& MC) const
  /*
    Accessor to nucleon number
    \param MC :: MCNP Name
    \return nucleon number
  */
{
  const pName& PN = getPItem(MC);
  return PN.nucleon;
}

int 
particleConv::mcnpITYP(const std::string& particleName) const
  /*
    Accessor to mcnpITYP number by full name
    \param particleName :: full name
    \return mcnpITYP
  */
{
  const pName& PN=getPHITSPItem(particleName);
  return PN.mcnpITYP;
}

const std::string&
particleConv::mcnpToPHITS(const int mcnpNum) const
  /*!
    Convert mcnp number to Phits
    \param mcnpNum :: mcnpNumber
    \return phits string
  */
{
  ELog::RegMethod RegA("particleConv","mcnpToPHITS");

  typedef std::map<std::string,pName>  PMAP;

  PMAP::const_iterator mc=
    std::find_if(indexLookup.begin(),indexLookup.end(),
		 [&mcnpNum](const PMAP::value_type& PN) -> bool
		 {
		   return (mcnpNum == PN.second.mcnpITYP);
		 } );
  
  if (mc==indexLookup.end())
    throw ColErr::InContainerError<int>
      (mcnpNum,"MCNP number not in particle list");
  
  return mc->second.phitsName;  
}

 
