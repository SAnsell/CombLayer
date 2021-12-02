/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/particleConv.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "RefCon.h"
#include "polySupport.h"
#include "particleConv.h"

pName::pName(const std::string& mcnpN,const int mI,
	     const std::string& flukaN,const int fI,
	     const std::string& phitsN,const int pI,
	     const int mcplN,const double M,const int N) :
  mcnpName(mcnpN),mcnpITYP(mI),
  flukaName(flukaN),flukaITYP(fI),
  phitsName(phitsN),phitsITYP(pI),
  mcplNumber(mcplN),mass(M),nucleon(N)
  /*
    Constructor 
    \param mcnpN :: MCNP Name
    \param mI :: MCNP itype number
    \param flukaN :: FLUKA name
    \param fI :: FLUKA itype number
    \param phitsN :: PHITS name
    \param pI :: PHITS ityp number
    \param mcplN :: kf/mcpl number
    \param M :: particle mass [MeV/c2]
    \param N :: nucleon number
  */
{}

pName::pName(const pName& A) :
  mcnpName(A.mcnpName),mcnpITYP(A.mcnpITYP),
  flukaName(A.flukaName),flukaITYP(A.flukaITYP),
  phitsName(A.phitsName),phitsITYP(A.phitsITYP),
  mcplNumber(A.mcplNumber),mass(A.mass),
  nucleon(A.nucleon)
  /*
    Copy Constructor 
    \param A :: Object to copy
  */
{}

// mcnpChar : mcnpI : fluka : flukaI : phits : phitsI : mcplNumber : mass : nucleons
particleConv::particleConv() : 
  particleVec({
  {"+", -1,  "all",       0,  "all",       -1,      -1,   0.0,          0},
  {"v", 0,  "void",       0,  "void",       0,       0,   0.0,          0},
  {"h", 9,  "proton",     1,  "proton",     1,    2212,   938.2720813,  1},
  {"n", 1,  "neutron",    8,  "neutron",    2,    2112,   939.5654133,  1},   
  {"/", 20, "pion+",     13,  "pion+",      3,     211,   139.57018,    1},
  {"z", 21, "pizero",    23,  "pion0",      4,     111,   134.9770,     1},
  {"*", 11, "pion-",     14,  "pion-",      5,    -211,   139.57018,    1},
  {"!", 16, "muon+",     10,  "muon+",      6,     -13,   105.6583745,   1},
  {"|", 4,  "muon-",     11,  "muon-",      7,      13,   105.6583745,   1},
  {"k", 22, "kaon+",     15,  "kaon+",      8,     321,   493.677,    1},
  {"%", 23, "kaonzero",  24,  "kaon0",      9,     311,   493.677,    1},  // k0 version
  {"^", 24, "kaonlong",  12,  "kaon0",      9,     130,   497.611,    1},  // long version
  {"?", 36, "kaon-",     16,  "kaon-",     10,    -321,   493.677,    1},
  {"e", 3,  "electron",   3,  "electron",  12,      11,   0.5109989461, 1},
  {"f", 8,  "positron",   4,  "positron",  13,     -11,   0.5109989461, 1},
  {"p", 2,  "photon",     7,  "photon",    14,      22,   0.0,          1},
  {"d", 31, "deuteron",  -3,  "deuteron",  15, 1000002,   1875.612793,  2},
  {"t", 32, "triton",    -4,  "triton",    16, 1000003,   2808.921112,   3},
  {"s", 33, "3-helium",  -5,  "3he",       17, 2000003,   2808.391586,    3},
  {"a", 34, "4-helium",  -6,  "alpha",     18, 2000004,   3727.379378,    4},
  {"g", 19, "antiproton", 2,  "antiproton",11,   -2212,   938.2720813,   1}       // no direct phits
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
particleConv::getMCPLIndex(const int ID) const
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
		   return (ID == PN.mcplNumber);
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
particleConv::getMCPLPItem(const int mcplIndex) const
  /*!
    Access a particle type from mcplIndex number
    \param mcplIndex :: Particle mcpl number
    \return pName found
  */
{
  const size_t index=getMCPLIndex(mcplIndex);
  if (!index)
    throw ColErr::InContainerError<int>
      (mcplIndex,"mcplIndex not in particleDataBase");

  return particleVec[index-1];
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
    Accessor to phitsITYP number by fluka/phits/mcnp name
    \param particleName :: full name
    \return phitsITYP (phits id number)
  */
{
  const pName& PN=getNamePItem(particleName);
  return PN.phitsITYP;
}

int 
particleConv::mcplITYP(const std::string& particleName) const
  /*
    Accessor to mcplITYP number by fluka/phits/mcnp name
    \param particleName :: full name
    \return mcplITYP (mcpl id number)
  */
{
  const pName& PN=getNamePItem(particleName);
  return PN.mcplNumber;
}


int 
particleConv::nucleon(const std::string& particleName) const
  /*
    Accessor to nucleon number by name
    \param particleName :: full name
    \return nucleon count
  */
{
  const pName& PN = getNamePItem(particleName);
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
  const size_t index=getMCNPIndex(ID);
  if (!index)
    throw ColErr::InContainerError<int>(ID,"ID no in particleDataBase");
  return particleVec[index-1].flukaName;
}

const std::string&
particleConv::mcplToFLUKA(const int ID) const
  /*!
    Get FLUKA name base on mcnp id number
    \param ID :: mcpl ID number
    \return fluka name						
  */
{
  const size_t index=getMCPLIndex(ID);
  if (!index)
    throw ColErr::InContainerError<int>(ID,"MCPL ID no in particleDataBase");
  return particleVec[index-1].flukaName;
}
 
const std::string&
particleConv::mcplToPHITS(const int ID) const
  /*!
    Get PHTIS name base on mcnp id number
    \param ID :: mcpl ID number
    \return fluka name						
  */
{
  const size_t index=getMCPLIndex(ID);
  if (!index)
    throw ColErr::InContainerError<int>(ID,"MCPL ID no in particleDataBase");
  return particleVec[index-1].phitsName;
}
 

const std::string&
particleConv::mcplToMCNP(const int particleIndex) const
  /*!
    Get MCNP name base on particle index (mcpl)
    \param particleIndex :: mcpl index to find
    \return mcnp name
  */
{
  const pName& PN = getMCPLPItem(particleIndex);
  return PN.mcnpName;
}

const std::string&
particleConv::nameToMCNP(const std::string& particleName) const
  /*!
    Get MCNP name base on particle name
    \param particleName :: generic name to find
    \return phits name
  */
{
  const pName& PN = getNamePItem(particleName);
  return PN.mcnpName;
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

template<typename T>
std::string
particleConv::mcnpParticleList(const T& particles) const
  /*!
    Convert a set into a comma separated list
    \param PSet :: stl container to convert (int)
    \return 
   */
{
  ELog::RegMethod RegA("particleConv","mcnpxParticleList");
  std::string separator;
  std::ostringstream cx;
  for(const int pIndex : particles)
    {
      const std::string mcnpPart=
	mcplToMCNP(pIndex);
      cx<<separator<<mcnpPart;
      separator=",";
    }
  
  return cx.str();
}

template<typename T>
std::string
particleConv::phitsParticleList(const T& particles) const
  /*!
    Convert a set into a comma separated list
    \param PSet :: stl container to convert (int)
    \return 
   */
{
  ELog::RegMethod RegA("particleConv","mcnpxParticleList");
  std::string separator;
  std::ostringstream cx;
  for(const int pIndex : particles)
    {
      const std::string mcnpPart=
	mcplToPHITS(pIndex);
      cx<<separator<<mcnpPart;
      separator=" ";
    }
  
  return cx.str();
}

double
particleConv::momentumFromKE(const std::string& particleName,
			     const double KE) const
  /*!
    Convert the kenetic energy to momenum
    Convertions from E_tot=m(1+(p/m)^2)^0.5
    \param KE :: Kinetic energy [MeV/c2]
    \return momentum [MeV/c]
  */
{
  const pName& PN = getNamePItem(particleName);
  return sqrt(KE*KE+2.0*PN.mass*KE);
}

double
particleConv::mcplMomentumFromKE(const int mcplIndex,
				 const double KE) const
  /*!
    Convert the kenetic energy to momenum
    Convertions from E_tot=m(1+(p/m)^2)^0.5
    \param mcplNum :: MCPL number 
    \param KE :: Kinetic energy [MeV/c2]
    \return momentum [MeV/c]
  */
{
  const pName& PN = getMCPLPItem(mcplIndex);
  return sqrt(KE*KE+2.0*PN.mass*KE);
}


double
particleConv::mcplKEWavelength(const int mcplNum,const double KE) const
  /*!
    Convert the kinetic energy to wavelength
    \param mcplNum :: MCPL number 
    \param KE :: Kinetic energy [MeV]
    \return wavelength [Angstrom]
    \todo -- do I need a low energy check for numerical accuracy?
  */
{
  const pName& PN = getMCPLPItem(mcplNum);
  
  return RefCon::hc_e/sqrt(KE*KE+2.0*KE*PN.mass);
}

double
particleConv::KEWavelength(const std::string& particleName,
			     const double KE) const
  /*!
    Convert the kinetic energy to wavelength
    \param particleName :: generic name to find
    \param KE :: Kinetic energy [MeV]
    \return wavelength [Angstrom]
    \todo -- do I need a low energy check for numerical accuracy?
  */
{
  const pName& PN = getNamePItem(particleName);
  
  return RefCon::hc_e/sqrt(KE*KE+2.0*KE*PN.mass);
}

double
particleConv::wavelengthKE(const std::string& particleName,
			     const double wave) const
  /*!
    Convert the kinetic energy to wavelength
    \param particleName :: generic name to find
    \param wave :: Wavelength [Angstrom]
    \return KE :: kinetic energy [MeV]
    \todo -- do I need a low energy check for numerical accuracy?
  */
{
  const pName& PN = getNamePItem(particleName);

  std::pair<double,double> AB;
  const size_t ansN=solveRealQuadratic
    (wave*wave,2*wave*PN.mass,-RefCon::hc_e*RefCon::hc_e,AB);
  return (!ansN) ? 0.0 : AB.second;
}

double
particleConv::mcplWavelengthKE(const int mcplNum,const double wave) const
  /*!
    Convert the kinetic energy to wavelength
    \param mcplNum :: mcplNumber
    \param wave :: Wavelenght [Angstrom]
    \return KE :: kinetic energy [MeV]
    \todo -- do I need a low energy check for numerical accuracy?
  */
{
  const pName& PN = getMCPLPItem(mcplNum);

  std::pair<double,double> AB;
  const size_t ansN=solveRealQuadratic
    (wave*wave,2*wave*PN.mass,-RefCon::hc_e*RefCon::hc_e,AB);

  return (!ansN) ? 0.0 : AB.second;
}

double
particleConv::mcplMass(const int mcplNum) const
  /*!
    Accessor to mass from mcpl number
    \param mcplNum :: mcpl index number
    \return rest mass [MeV/c2]
   */
{
  const pName& PN = getMCPLPItem(mcplNum);
  return PN.mass;
}

double
particleConv::mass(const std::string& particleName) const
  /*!
    Accessor to mass from name
    \param particleName :: Particle Name
    \return rest mass [MeV/c2]
   */
{
  const pName& PN = getNamePItem(particleName);
  return PN.mass;
}

///\cond template 

template std::string
particleConv::mcnpParticleList(const std::set<int>&) const;

template std::string
particleConv::phitsParticleList(const std::set<int>&) const;

///\endcond template 
