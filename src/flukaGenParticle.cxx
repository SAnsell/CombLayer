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


flukaGenParticle::flukaGenParticle() : 
  flukaGen({
      {"all-part",201}, {"all-char",202}, {"all-neut",203}, {"all-nega",204},
      {"all-posi",205}, {"nucleons",206}, {"nuc&pi+-",207}, {"energy",208},
      {"pions+-",209}, {"beampart",210}, {"em-enrgy",211}, {"muons",212},
      {"e+&e-",213}, {"ap&an",214}, {"kaons",215}, {"strange",216},
      {"kaons+-",217}, {"had-char",218}, {"fissions",219}, {"he-fiss",220},
      {"le-fiss",221}, {"neu-bala ",222}, {"had-neut",223}, {"kaons0",224},
      {"c-mesons",225}, {"c-(a)bar",226}, {"charmed",227}, {"dose",228},
      {"unb-ener",229}, {"unb-emen",230}, {"x-moment",231}, {"y-moment",232},
      {"z-moment",233}, {"activity",234}, {"actomass",235}, {"si1mevne",236},
      {"hadgt20m ",237}, {"niel-dep",238}, {"dpa-sco",239}, {"dose-eq",240},
      {"dose-em",241}, {"net-chrg",242}, {"doseqlet",243}, {"res-niel",244},
      {"hehad-eq",245}, {"thneu-eq",246} 
      })
  /*!
    constructor
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
    \param particleName :: generic particle/type
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
    \param particleName :: generic particle/type
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

  const std::string PN=StrFunc::toLowerString(particleName);
  std::map<std::string,int>::const_iterator mc=
    flukaGen.find(PN);
  if (mc!=flukaGen.end())
    return mc->first;

  return particleConv::Instance().nameToFLUKA(PN);
}

double
flukaGenParticle::momentumFromKE(const std::string& particleName,
				 const double KE) const
  /*!
    Convert the kenetic energy to momenum
    \param particleName :: Particle name
    \param KE :: Kenetic energy [MeV/c2]
    \return momentum [MeV/c]
  */
{
  return particleConv::Instance().momentumFromKE(particleName,KE); 
}

double
flukaGenParticle::mass(const std::string& particleName) const
  /*!
    Convert the kenetic energy to momenum
    \param particleName :: Particle name
    \return mass [MeV/c^2]
  */
{
  return particleConv::Instance().mass(particleName); 
}

