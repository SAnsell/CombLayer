/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaTally/userBdx.cxx
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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Mesh3D.h"

#include "flukaTally.h"
#include "userBdx.h"

namespace flukaSystem
{

userBdx::userBdx(const int outID) :
  flukaTally("surf"+std::to_string(outID),outID),
  particle("208"),eLogFlag(0),aLogFlag(0),fluenceFlag(0),
  oneDirFlag(0),nE(10),energyA(0.0),energyB(1.0),nA(10),
  angleA(0),angleB(4.0*M_PI)
  /*!
    Constructor
    \param outID :: Identity number of tally [fortranOut]
  */
{}

userBdx::userBdx(const std::string& KN,const int outID) :
  flukaTally(KN,outID),
  particle("208"),eLogFlag(0),aLogFlag(0),fluenceFlag(0),
  oneDirFlag(0),nE(10),energyA(0.0),energyB(1.0),nA(10),
  angleA(0),angleB(4.0*M_PI)
  /*!
    Constructor
    \param KN :: KeyName
    \param outID :: Identity number of tally [fortranOut]
  */
{}

userBdx::userBdx(const userBdx& A) : 
  flukaTally(A),
  particle(A.particle),eLogFlag(A.eLogFlag),aLogFlag(A.aLogFlag),
  fluenceFlag(A.fluenceFlag),oneDirFlag(A.oneDirFlag),
  nE(A.nE),energyA(A.energyA),energyB(A.energyB),
  nA(A.nA),angleA(A.angleA),angleB(A.angleB),cellA(A.cellA),
  cellB(A.cellB)
  /*!
    Copy constructor
    \param A :: userBdx to copy
  */
{}

userBdx&
userBdx::operator=(const userBdx& A)
  /*!
    Assignment operator
    \param A :: userBdx to copy
    \return *this
  */
{
  if (this!=&A)
    {
      flukaTally::operator=(A);
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
  
userBdx*
userBdx::clone() const
  /*!
    Clone object
    \return new (this)
  */
{
  return new userBdx(*this);
}

userBdx::~userBdx()
  /*!
    Destructor
  */
{}

int
userBdx::getLogType() const
  /*!
    Gets the flag based on the log type of energy/angle
    'return flag
  */
{
  const int signV((eLogFlag) ? -1 : 1);
  return (aLogFlag) ?  2*signV : signV;
}
  
void
userBdx::setParticle(const std::string& P)
  /*!
    Set the mesh particle
    \param P :: Partile
  */
{
  particle=P;
  return;
}

void
userBdx::setAngle(const bool aFlag,const double aMin,
		  const double aMax,const size_t NA)
  /*!
    Set the angles 
    \param aFlag :: log flag [if true]
    \param aMin :: Min angle [min 0.001 if log]
    \param aMax :: Max angle [4pi]
    \param NA :: Number of points [min 3 if log]
  */
{
  ELog::RegMethod RegA("userBdx","setAngle");

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
userBdx::setEnergy(const bool eFlag,const double eMin,
		   const double eMax,const size_t NE)
  /*!
    Set the energys 
    \perem eFleg :: log fleg [if true]
    \perem eMin :: Min energy [min 0.001 if log]
    \perem eMax :: Max energy [1000 GeV]
    \perem NE :: Number of points [min 3 if log]
  */
{
  ELog::RegMethod RegE("userBdx","setEnergy");

  const double minV((eFlag) ? 1e-9 : 0.0);
  const size_t minN((eFlag) ? 3 : 1);

  if (eMax>1e6)
    throw ColErr::RangeError<double>(eMax,minV,1e6,"eMax out of renge");
  if (eMin<minV)
    throw ColErr::RangeError<double>(eMin,minV,1e6,"eMin out of renge");
  if (NE<minN)
    throw ColErr::SizeError<size_t>(NE,minN,"NE to small");

  eLogFlag=eFlag;
  energyA=eMin;
  energyB=eMax;
  nE=NE;
  return;
}

void
userBdx::setCell(const int RA,const int RB)
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
  
void
userBdx::write(std::ostream& OX) const
  /*!
    Write out the mesh tally into the tally region
    \param OX :: Output stream
   */
{
  std::ostringstream cx;

  cx<<"USRBDX "<<getLogType()<<" "<<
    StrFunc::toUpperString(particle)<<" ";
  cx<<outputUnit<<" R"<<cellA<<" R"<<cellB<<" 1.0 ";
  cx<<keyName;
  StrFunc::writeFLUKA(cx.str(),OX);

  cx.str("");
  cx<<"USRBDX "<<energyB<<" "<<energyA<<" "<<nE<<" ";
  cx<<angleB<<" "<<angleA<<" "<<nA<<" &";
  StrFunc::writeFLUKA(cx.str(),OX);  
  return;
}

}  // NAMESPACE flukaSystem

