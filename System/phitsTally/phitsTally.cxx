/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsTally/phitsTally.cxx
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
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "writeSupport.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "particleConv.h"
#include "phitsTally.h"

namespace phitsSystem
{

std::ostream&
operator<<(std::ostream& OX,const phitsTally& TX)
  /*!
    Generic writing function
    \param OX :: Output stream
    \param TX :: phitsTally
    \return Output stream
   */
{
  TX.write(OX);
  TX.writeAuxScore(OX);
  return OX;
}

phitsTally::phitsTally(const std::string& MK,const int ID)  :
  keyName(MK),outputUnit(ID)
  /*!
    Constructor 
    \param MK :: Keyname
    \param ID :: phitsTally ID number
  */
{}

phitsTally::phitsTally(const int ID)  :
  outputUnit(ID)
  /*!
    Constructor 
    \param ID :: phitsTally ID number
  */
{}

phitsTally::phitsTally(const phitsTally& A)  :
  keyName(A.keyName),outputUnit(A.outputUnit),
  comments(A.comments),auxParticle(A.auxParticle),
  doseType(A.doseType)
  /*!
    Copy constructor
    \param A :: phitsTally object to copy
  */
{}

phitsTally*
phitsTally::clone() const
  /*!
    Basic clone class
    \return new this
  */
{
  return new phitsTally(*this);
}

phitsTally&
phitsTally::operator=(const phitsTally& A) 
  /*!
    Assignment operator 
    \param A :: phitsTally object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      keyName=A.keyName;
      outputUnit=A.outputUnit;
      comments=A.comments;
      auxParticle=A.auxParticle;
      doseType=A.doseType;
    }
  return *this;
}

phitsTally::~phitsTally()
 /*!
   Destructor
 */
{}

void
phitsTally::setKeyName(const std::string& K)
  /*!
    Set the keyname
    \param K :: Keyname
  */
{
  keyName=K;
  return;
}

void
phitsTally::setComment(const std::string& C)
  /*!
    Set the comment line
    \param C :: New comment
  */
{
  comments=C;
  return;
}

const std::string&
phitsTally::getKeyName() const
  /*!
    Get full name including output number
  */
{
  return keyName;
}

void
phitsTally::setAuxParticles(const std::string& P)
  /*!
    Set the auxParticle [can be a range?]
    \param P :: auxParticle (or key name) 
  */
{
  auxParticle=P;
  return;
}

void
phitsTally::setBinary()
  /*!
    Set the tally to binary
  */
{
  ELog::RegMethod RegA("phitsTally","setBinary");

  outputUnit=-std::abs(outputUnit);
  return;
}

void
phitsTally::setEnergy(const bool,const double,
		      const double,const size_t)
  /*!
    Null op call for non-energy detectors
  */
{}

void
phitsTally::setAngle(const bool,const double,
		      const double,const size_t)
 /*!
   Null op call for non-angle detectors
 */
{}
  
void
phitsTally::setDoseType(const std::string& P,
			const std::string& D)
  /*!
    Set the auxParticle [can be a range?]
    \param P :: Particle type
    \param D :: set dose type
  */
{
  ELog::RegMethod RegA("phitsTally","setDoseType");

  static const std::set<std::string> validDose
    ({
      "EAP74","ERT74","EWT74",
      "EAPMP","ERTMP","EWTMP",
      "AMB74","AMBGS"
      });
  const phitsGenParticle& FG=phitsGenParticle::Instance();
  
  auxParticle=StrFunc::toUpperString(FG.nameToPHITS(P));
   const std::string Dupper=StrFunc::toUpperString(D);
  if (validDose.find(Dupper)==validDose.end())
    throw ColErr::InContainerError<std::string>(D,"Dose type not known");

  doseType=Dupper;
  return;
}
  
void
phitsTally::writeAuxScore(std::ostream&) const
  /*!
    Writes out the phitsTally depending on the 
    fields that have been set.
    \param OX :: Output Stream
  */
{
  return;
}

void
phitsTally::write(std::ostream&) const
  /*!
    Writes out the phitsTally depending on the 
    fields that have been set.
    \param OX :: Output Stream
  */
{
  return;
}

}  // NAMESPACE phitsSystem
