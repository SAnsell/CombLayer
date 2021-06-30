/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaTally/flukaTally.cxx
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
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <complex>
#include <cmath>
#include <string>
#include <list>
#include <set>
#include <map>
#include <vector>
#include <iterator>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "stringCombine.h"
#include "writeSupport.h"
#include "flukaGenParticle.h"
#include "flukaTally.h"

namespace flukaSystem
{

std::ostream&
operator<<(std::ostream& OX,const flukaTally& TX)
  /*!
    Generic writing function
    \param OX :: Output stream
    \param TX :: flukaTally
    \return Output stream
   */
{
  TX.write(OX);
  TX.writeAuxScore(OX);
  return OX;
}

std::string
flukaTally::idForm(const std::string& baseName,const int A)
  /*!
    Calculate the id form based on making a unique two digit char
    \param baseName :: Prename
    \param A :: index to used
   */
{
  if (A<100) return baseName.substr(0,4)+std::to_string(A);

  return baseName.substr(0,3)+std::to_string(A);

}

  
flukaTally::flukaTally(const std::string& MK,
		       const int indexID,
		       const int outID)  :
  keyName(idForm(MK,std::abs(indexID))),
  ID(std::abs(indexID)),
  outputUnit(outID)
  /*!
    Constructor 
    \param MK :: Keyname
    \param indexID :: flukaTally ID number
    \param outID :: flukaTally fortran tape number
  */
{}

flukaTally::flukaTally(const flukaTally& A)  :
  keyName(A.keyName),
  ID(A.ID),outputUnit(A.outputUnit),
  comments(A.comments),auxParticle(A.auxParticle),
  doseType(A.doseType),userName(A.userName)
  /*!
    Copy constructor
    \param A :: flukaTally object to copy
  */
{}

flukaTally*
flukaTally::clone() const
  /*!
    Basic clone class
    \return new this
  */
{
  return new flukaTally(*this);
}

flukaTally&
flukaTally::operator=(const flukaTally& A) 
  /*!
    Assignment operator 
    \param A :: flukaTally object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      keyName=A.keyName;
      ID=A.ID;
      outputUnit=A.outputUnit;
      comments=A.comments;
      auxParticle=A.auxParticle;
      doseType=A.doseType;
      userName=A.userName;
    }
  return *this;
}

flukaTally::~flukaTally()
 /*!
   Destructor
 */
{}

void
flukaTally::setKeyName(const std::string& K)
  /*!
    Set the keyname
    \param K :: Keyname
  */
{
  keyName=K;
  return;
}

void
flukaTally::setComment(const std::string& C)
  /*!
    Set the comment line
    \param C :: New comment
  */
{
  comments=C;
  return;
}

const std::string&
flukaTally::getKeyName() const
  /*!
    Get full name including output number
  */
{
  return keyName;
}


void
flukaTally::setAuxParticles(const std::string& P)
  /*!
    Set the auxParticle [can be a range?]
    \param P :: auxParticle (or key name) 
  */
{
  auxParticle=P;
  return;
}

void
flukaTally::setAscii()
  /*!
    Set the tally to binary
  */
{
  ELog::RegMethod RegA("flukaTally","setAscii");

  outputUnit=std::abs(outputUnit);
  return;
}

void
flukaTally::setBinary()
  /*!
    Set the tally to binary
  */
{
  ELog::RegMethod RegA("flukaTally","setBinary");

  outputUnit=-std::abs(outputUnit);
  return;
}

void
flukaTally::setEnergy(const bool,const double,
		      const double,const size_t)
  /*!
    Null op call for non-energy detectors
  */
{}

void
flukaTally::setAngle(const bool,const double,
		      const double,const size_t)
 /*!
   Null op call for non-angle detectors
 */
{}
  
void
flukaTally::setDoseType(const std::string& P,
			const std::string& D)
  /*!
    Set the auxParticle [can be a range?]
    \param P :: Particle type
    \param D :: set dose type
  */
{
  ELog::RegMethod RegA("flukaTally","setDoseType");

  static const std::set<std::string> validDose
    ({
      "EAP74","ERT74","EWT74",
      "EAPMP","ERTMP","EWTMP",
      "AMB74","AMBGS"
      });
  const flukaGenParticle& FG=flukaGenParticle::Instance();
  
  auxParticle=StrFunc::toUpperString(FG.nameToFLUKA(P));
   const std::string Dupper=StrFunc::toUpperString(D);
  if (validDose.find(Dupper)==validDose.end())
    throw ColErr::InContainerError<std::string>(D,"Dose type not known");

  doseType=Dupper;
  return;
}
  
void
flukaTally::writeAuxScore(std::ostream&) const
  /*!
    Writes out the flukaTally depending on the 
    fields that have been set.
    \param OX :: Output Stream
  */
{
  return;
}

void
flukaTally::write(std::ostream& OX) const
  /*!
    Writes out the flukaTally depending on the 
    fields that have been set.
    \param OX :: Output Stream
  */
{
  if (!userName.empty())
    {
      std::ostringstream cx;
      cx<<"USRICALL "<<outputUnit<<" - - - - -  "<<userName;
      StrFunc::writeFLUKA(cx.str(),OX);
    }

  return;
}

}  // NAMESPACE flukaSystem
