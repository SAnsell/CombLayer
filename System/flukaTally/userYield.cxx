/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaTally/userYield.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "userYield.h"

namespace flukaSystem
{

userYield::userYield(const int outID) :
  flukaTally("yiel"+std::to_string(outID),outID),
  scoreLog(1),scoreTypeA("1"),scoreTypeB("2"),
  particle("208"),eLogFlag(0),aLogFlag(0),fluenceFlag(0),
  oneDirFlag(0),nE(10),energyA(0.0),energyB(1.0),nA(10),
  angleA(0),angleB(4.0*M_PI)
  /*!
    Constructor
    \param outID :: Identity number of tally [fortranOut]
  */
{}

userYield::userYield(const std::string& KN,const int outID) :
  flukaTally(KN,outID),scoreLog(1),scoreTypeA("1"),scoreTypeB("2"),
  particle("208"),eLogFlag(0),aLogFlag(0),fluenceFlag(0),
  oneDirFlag(0),nE(10),energyA(0.0),energyB(1.0),nA(10),
  angleA(0),angleB(4.0*M_PI)
  /*!
    Constructor
    \param KN :: KeyName
    \param outID :: Identity number of tally [fortranOut]
  */
{}

userYield::userYield(const userYield& A) : 
  flukaTally(A),scoreLog(A.scoreLog),
  scoreTypeA(A.scoreTypeA),scoreTypeB(A.scoreTypeB),
  particle(A.particle),eLogFlag(A.eLogFlag),aLogFlag(A.aLogFlag),
  fluenceFlag(A.fluenceFlag),oneDirFlag(A.oneDirFlag),
  nE(A.nE),energyA(A.energyA),energyB(A.energyB),
  nA(A.nA),angleA(A.angleA),angleB(A.angleB),cellA(A.cellA),
  cellB(A.cellB)
  /*!
    Copy constructor
    \param A :: userYield to copy
  */
{}

userYield&
userYield::operator=(const userYield& A)
  /*!
    Assignment operator
    \param A :: userYield to copy
    \return *this
  */
{
  if (this!=&A)
    {
      flukaTally::operator=(A);
      scoreTypeA=A.scoreTypeA;
      scoreTypeB=A.scoreTypeB;
      particle=A.particle;
      eLogFlag=A.eLogFlag;
      aLogFlag=A.aLogFlag;
      fluenceFlag=A.fluenceFlag;
      oneDirFlag=A.oneDirFlag;
      nE=A.nE;
      energyA=A.energyA;
      energyB=A.energyB;
      nA=A.nA;
      angleA=A.angleA;
      angleB=A.angleB;
      cellA=A.cellA;
      cellB=A.cellB;
    }
  return *this;
}
  
userYield*
userYield::clone() const
  /*!
    Clone object
    \return new (this)
  */
{
  return new userYield(*this);
}

userYield::~userYield()
  /*!
    Destructor
  */
{}

int
userYield::getLogType() const
  /*!
    Gets the flag based on the log type of energy/angle
    'return flag
  */
{
  const int signV((eLogFlag) ? -1 : 1);
  return (aLogFlag) ?  2*signV : signV;
}
  
void
userYield::setParticle(const std::string& P)
  /*!
    Set the mesh particle
    \param P :: Partile
  */
{
  particle=P;
  return;
}

void
userYield::setScoreType(const bool logFlag,
			const std::string& A,
			const std::string& B)
  /*!
    Set the scoring type
    \param logFlag :: log type to use for A [only]
    \param A :: ScoreTypeA 
    \param B :: ScoreTypeB
  */
{
  scoreLog=logFlag;
  scoreTypeA=A;
  scoreTypeB=B;
  return;
}

void
userYield::setAngle(const bool aFlag,const double aMin,
		    const double aMax,const size_t NA)
  /*!
    Set the angles 
    \param aFlag :: log flag [if true]
    \param aMin :: Min angle [min 0.001 if log]
    \param aMax :: Max angle [4pi]
    \param NA :: Number of points [min 3 if log]
  */
{
  ELog::RegMethod RegA("userYield","setAngle");

  const double minV((aFlag) ? 1e-6 : 0.0);
  const size_t minN((aFlag) ? 3 : 1);

  if (aMax>4*M_PI)
    throw ColErr::RangeError<double>(aMax,minV,4*M_PI,"aMax out of range");
  if (aMin<minV)
    throw ColErr::RangeError<double>(aMin,minV,4*M_PI,"aMin out of range");
  if (NA<minN)
    throw ColErr::SizeError<size_t>(NA,minN,"NA to small");

  aLogFlag=aFlag;
  angleA=aMin;
  angleB=aMax;
  nA=NA;
  return;
}
  
void
userYield::setEnergy(const bool eFlag,const double eMin,
		   const double eMax,const size_t NE)
  /*!
    Set the energys 
    \perem eFleg :: log fleg [if true]
    \perem eMin :: Min energy [min 0.001MeV if log]
    \perem eMax :: Max energy [MeV]
    \perem NE :: Number of points [min 3 if log]
  */
{
  ELog::RegMethod RegE("userYield","setEnergy");

  const double minV((eFlag) ? 1e-9 : 0.0);
  const size_t minN((eFlag) ? 3 : 1);

  if (eMax>1e6)
    throw ColErr::RangeError<double>(eMax,minV,1e6,"eMax out of range");
  if (eMin<minV)
    throw ColErr::RangeError<double>(eMin,minV,1e6,"eMin out of range");
  if (NE<minN)
    throw ColErr::SizeError<size_t>(NE,minN,"NE to small");

  eLogFlag=eFlag;
  energyA=eMin*1e-3;
  energyB=eMax*1e-3;
    
  nE=NE;
  return;
}

void
userYield::setCell(const int RA,const int RB)
  /*!
    Set the cells/regions
    \param RA :: Region A
    \param RB :: Region B
  */
{
  cellA=RA;
  cellB=RB;
  return;
}

int
userYield::getScore(const std::string& A)
  /*!
    get the score
   */
{
  ELog::RegMethod RegA("userYield","getScoreIndex");

  const std::map<std::string,int> nameMap({
      {"KE",1},
	{"Monmentum",2}});
  
  int out;
  if (StrFunc::convert(A,out))
    return out;
  std::map<std::string,int>::const_iterator mc=nameMap.find(A);
  if (mc!=nameMap.end())
    return mc->second;
  
  return 1;
}
  
int
userYield::getScoreIndex() const
  /*!
    Calculate the index score for user yield
    \return What 1 for fluka
   */
{
  ELog::RegMethod RegA("userYield","getScoreIndex");

  const int iE=getScore(scoreTypeA);
  const int iA=getScore(scoreTypeA);
  

  return iE+iA*100;
}
  
void
userYield::write(std::ostream& OX) const
  /*!
    Write out the mesh tally into the tally region
    \param OX :: Output stream
   */
{
  std::ostringstream cx;

  cx<<"USRYIELD "<<getLogType()<<" "<<
    StrFunc::toUpperString(particle)<<" ";
  cx<<outputUnit<<" R"<<cellA<<" R"<<cellB<<" 1.0 ";
  cx<<keyName;
  StrFunc::writeFLUKA(cx.str(),OX);

  cx.str("");
  cx<<"USRYIELD "<<energyB<<" "<<energyA<<" "<<nE<<" ";
  cx<<angleB<<" "<<angleA<<" "<<nA<<" &";
  StrFunc::writeFLUKA(cx.str(),OX);  
  return;
}

}  // NAMESPACE flukaSystem

